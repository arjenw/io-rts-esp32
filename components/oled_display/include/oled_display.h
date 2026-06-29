#pragma once
#include "sdkconfig.h"
#include "esp_err.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_OLED_ENABLED

esp_err_t oled_init(void);

/* Screen layout (rows 0-7):
 *  0  "io-homecontrol"       title
 *  1  "--------------------" separator
 *  2  "TX> AABBCC 0x31"      last TX: device + cmd  (TX section row 1/2)
 *  3  (blank)                                         (TX section row 2/2)
 *  4  (blank)
 *  5  "--------------------" separator
 *  6  "RX< AABBCC FE" + bars last RX + RSSI bars     (RX section row 1/2)
 *  7  "Ready"                transient status msg     (RX section row 2/2)
 */
void oled_show_tx(const char *cmd_name, const char *device_id);
void oled_show_rx(const char *device_id, const char *cmd_hex,
                  int position_pct, int rssi);
void oled_show_status(const char *msg);

#else

static inline esp_err_t oled_init(void)                           { return ESP_OK; }
static inline void      oled_show_tx(const char *n, const char *d)            { (void)n; (void)d; }
static inline void      oled_show_rx(const char *d, const char *c, int p, int r) { (void)d; (void)c; (void)p; (void)r; }
static inline void      oled_show_status(const char *m)                       { (void)m; }

#endif

#ifdef __cplusplus
}
#endif
