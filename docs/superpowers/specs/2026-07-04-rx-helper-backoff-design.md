# Design: ReceiveMatchingFrame helper + poll backoff

## Context

`IoHomeControl.cpp` contains ~15 call sites that call `xQueueReceive(sRxIoQueue, ...)` directly. Most consume one frame and immediately fail the exchange if it doesn't match — they don't retry within the remaining timeout. This causes two problems:

1. Ambient RF traffic (frames between other devices, or frames addressed to us from an unexpected peer) lands in `sRxIoQueue` during an active exchange, steals the slot, and aborts the exchange prematurely.
2. Useful frames (e.g. autonomous CMD_71 status updates) that arrive during an exchange are silently discarded rather than being handled by `ProcessReceivedFrameTask`.

Additionally, `UpdateDevicesStatusTask` retries failed polls with a flat 60-second delay regardless of how many consecutive failures a device has had. This produces double-cluster error bursts in logs (hourly poll fails → all devices retry together 60 s later → fail again) and hammers unreachable devices unnecessarily.

Graylog analysis (2026-07-03): 1044 "didn't receive final response!" errors in 7 days, in hourly clusters, primarily from Screen_Tom_Huis (70D78E, noted as off/out of range).

## Architecture note

`sMutex` serialises all active communication. `ProcessReceivedFrameTask` and `UpdateDevicesStatusTask` (via `SendAndReceive`) are mutually exclusive — they cannot hold the mutex simultaneously. This means:
- During any exchange, all incoming frames go to `sRxIoQueue` and cannot be processed by `ProcessReceivedFrameTask` until the mutex is released.
- Any frames stashed during an exchange and re-queued before mutex release will be processed correctly and in order by `ProcessReceivedFrameTask` on the next iteration.

---

## Feature 1: ReceiveMatchingFrame helper

### Purpose

Replace ad-hoc single-shot `xQueueReceive` + node_id check patterns at active-exchange call sites with a deadline loop that drains irrelevant frames rather than failing on the first one.

### Scope

Targets exchange call sites only — `SendAndReceive` (lines 1983, 2007), `AuthenticateReceivedRequest` (line 2062), and equivalent nested receives in pairing/key-learning flows. The general `ProcessReceivedFrameTask` dispatch loop is not in scope.

### Function signature

Private static function in `IoHomeControl.cpp`:

```cpp
static bool ReceiveMatchingFrame(
    const uint8_t    *expected_src,  // node ID expected as frame source (3 bytes)
    const uint8_t    *expected_dst,  // node ID expected as frame destination (3 bytes)
    int               expected_cmd, // command ID to match, or -1 for any
    TickType_t        timeout,
    RxFrameQueueItem &result,
    RxFrameQueueItem *stash,
    int              &stash_count);
```

### Behaviour

```
deadline = now + timeout
while now < deadline:
    wait = deadline - now  (minimum 1 tick)
    if xQueueReceive(sRxIoQueue, frame, wait) == false:
        return false  // genuine timeout
    if frame.dest_node != expected_dst:
        discard silently  // overheard traffic, not for us
    elif frame.src_node != expected_src OR (expected_cmd != -1 AND frame.cmd != expected_cmd):
        stash(frame)      // addressed to us, but wrong peer or wrong command
    else:
        result = frame
        return true       // matched
return false
```

### Stash

- Capacity: `RX_STASH_MAX 4` (compile-time constant)
- If stash is full when a stashable frame arrives: drop the oldest entry (shift down), stash the new one
- Caller declares `RxFrameQueueItem stash[RX_STASH_MAX]` and `int stash_count = 0` on the stack

### Flush

Companion static function, called by the caller after the exchange completes (before `xSemaphoreGive`):

```cpp
static void FlushStash(RxFrameQueueItem *stash, int count);
```

- Uses `xQueueSendToFront` with 0 timeout per item to preserve arrival order
- If queue is full, the item is dropped silently (acceptable: queue depth 10, stash max 4)
- Must be called whether the exchange succeeded or failed

### Frame classification

| Condition | Action |
|---|---|
| `dest_node != expected_dst` | Discard — overheard ambient traffic |
| `dest_node == expected_dst`, wrong src or cmd | Stash — addressed to us, handle later |
| `dest_node == expected_dst`, src and cmd match | Return as result |

### Call site adoption

Priority call sites (highest impact):
- `SendAndReceive` lines 1983 and 2007 — replace both single-shot receives
- `AuthenticateReceivedRequest` line 2062 — replace single-shot receive

Secondary call sites (consistency):
- `DiscoverAndPairDevice` lines 894, 907
- `PairAsDevice` lines 1385, 1396

Each adoption: remove existing `xQueueReceive` + manual node_id check, replace with `ReceiveMatchingFrame` call + `FlushStash` after exchange.

---

## Feature 2: Per-device poll backoff

### Purpose

Eliminate the double-cluster error pattern by backing off retry intervals for devices that repeatedly fail to respond, rather than always retrying in 60 seconds.

### Data structure

Add one field to `IoDevice` in `iohome_device.hpp`:

```cpp
uint8_t consecutive_poll_failures = 0;
```

- Transient: not serialised to LittleFS, not included in MQTT/WebSocket payloads
- Resets to 0 on reboot and on successful poll

### Backoff constants

Add to `IoHomeControl.cpp` alongside existing `STATUS_UPDATE_*` constants:

```cpp
constexpr int64_t STATUS_UPDATE_BACKOFF_1_US = 60'000'000LL;     // 1 failure  → 60 s
constexpr int64_t STATUS_UPDATE_BACKOFF_2_US = 300'000'000LL;    // 2 failures → 5 min
constexpr int64_t STATUS_UPDATE_BACKOFF_3_US = 1'800'000'000LL;  // 3 failures → 30 min
// 4+ failures: reuse STATUS_UPDATE_MAX_TIME_US = 3 600 000 000 (1 hour)
```

### Logic change in UpdateDevicesStatusTask

On failure (replaces existing single-line `next_status_update_timestamp` assignment):

```cpp
dev->second.consecutive_poll_failures++;
int64_t backoff;
switch (dev->second.consecutive_poll_failures) {
    case 1:  backoff = STATUS_UPDATE_BACKOFF_1_US; break;
    case 2:  backoff = STATUS_UPDATE_BACKOFF_2_US; break;
    case 3:  backoff = STATUS_UPDATE_BACKOFF_3_US; break;
    default: backoff = STATUS_UPDATE_MAX_TIME_US;  break;
}
dev->second.next_status_update_timestamp = esp_timer_get_time() + backoff;
IO_LOGW("UpdateDevicesStatusTask: {} poll failed (consecutive: {})",
        devId, dev->second.consecutive_poll_failures);
```

On success (before existing `next_status_update_timestamp` logic):

```cpp
dev->second.consecutive_poll_failures = 0;
```

### Log improvements

- Failure log: `LOGE` → `LOGW` (retries are automatic; only log as error after all `SendAndReceive` retries exhausted, which already happens inside `SendAndReceive`)
- Failure log gains device ID and consecutive count — previously logged nothing identifiable

### Effect on Screen_Tom_Huis

```
Before: hourly poll → fail → 60s → fail → 60s → fail → 1hr → repeat (double cluster)
After:  hourly poll → fail(1) → 60s → fail(2) → 5min → fail(3) → 30min → fail(4+) → 1hr
```

After 4 failures the device settles into the same 1-hour rhythm as a healthy device. If it comes back online, the next hourly poll succeeds and the counter resets.

---

## What this does NOT address

- **Screen_Tom_Huis genuine timeout**: the helper does not fix a device that doesn't complete authentication. The backoff reduces noise from that device; removing it from `sDeviceMap` would eliminate the errors entirely.
- **Unknown-source filtering**: discarding frames from devices not in `sDeviceMap`/`sRemoteMap` is a separate feature at `ProcessReceivedFrameTask` level, not part of this spec.
- **Hardware address filtering**: SX1276 supports single-byte address filtering only; io-homecontrol uses 3-byte node IDs, so hardware filtering is not applicable.
