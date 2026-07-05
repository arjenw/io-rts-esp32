---

## First-time flash — ESP-IDF web upload tool

Select the `full.bin` for your board from the assets above — this is a single merged image (bootloader + partition table + app + web UI) that can be flashed at address `0x0`.

| Board | Asset |
|---|---|
| Heltec ESP32 | `heltec-<version>-full.bin` |
| LilyGo LoRa32 V1.6.1 (ESP32) | `lilygo-lora32-<version>-full.bin` |
| LilyGo T3-S3 (ESP32-S3) | `lilygo-t3s3-<version>-full.bin` |

1. Open the [ESP-IDF web flash tool](https://espressif.github.io/esptool-js/)
2. Connect the ESP32 via USB and put it in flash mode (hold BOOT, press EN/RESET, release BOOT)
3. Click **Connect** and select the serial port
4. Set address to `0x0`
5. Choose the `full.bin` for your board and click **Program** — flashing takes about 30 seconds
6. Press EN/RESET to boot

After the first serial flash, all subsequent updates can be done wirelessly via the device web UI (OTA tab → select the `firmware.bin` for your board).

---

## First-time WiFi setup

After flashing, the device starts a setup access point when no WiFi credentials are configured or the network is unreachable.

| Setting | Value |
|---|---|
| SSID | `io-rts-setup` |
| Password | `io-rts-esp32` |

Connect to this AP and navigate to `http://192.168.4.1` to enter your WiFi credentials. The device reboots and connects to your network automatically.
