#ifndef BLE_PAYLOAD_H
#define BLE_PAYLOAD_H

#include <stdint.h>
#include "stats.h"

#define BLE_PAYLOAD_SIZE 27  // 1 (counter) + 2 (zaman) + 3*(4 istatistik*2 bayt)

void encode_ble_advertising_data(uint8_t *payload,
                                 const stats_t *temp_stats,
                                 const stats_t *hum_stats,
                                 const stats_t *co2_stats);

#endif
