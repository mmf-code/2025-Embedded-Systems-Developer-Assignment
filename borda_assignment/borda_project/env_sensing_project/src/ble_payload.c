#include "ble_payload.h"
#include <time.h>

/**
 * @brief Encodes environmental sensor statistics into a BLE advertising payload.
 * 
 * Payload structure:
 * - Byte 0   : Rolling counter (increments every packet)
 * - Byte 1-2 : Timestamp (2 bytes, Unix time % 65536)
 * - Byte 3-26: For each sensor (temp, hum, CO2), 4 statistics:
 *              [std_dev, max, min, median] × 2 bytes each = 8 bytes per sensor
 * 
 * Total payload size: 27 bytes
 * 
 * @param payload     Pointer to the buffer to fill (must be at least 27 bytes)
 * @param temp_stats  Pointer to temperature statistics
 * @param hum_stats   Pointer to humidity statistics
 * @param co2_stats   Pointer to CO₂ statistics
 */
void encode_ble_advertising_data(uint8_t *payload,
                                 const stats_t *temp_stats,
                                 const stats_t *hum_stats,
                                 const stats_t *co2_stats) {
    static uint8_t counter = 0;

    // Generate a timestamp as 2-byte truncated UNIX time
    time_t now = time(NULL);
    uint16_t timestamp = (uint16_t)(now % 65536);

    payload[0] = counter++;              // Packet counter (rolls over at 255)
    payload[1] = timestamp & 0xFF;       // Timestamp LSB
    payload[2] = (timestamp >> 8) & 0xFF;// Timestamp MSB

    // Array of sensor statistics for iteration
    const stats_t *sensors[3] = { temp_stats, hum_stats, co2_stats };

    for (int i = 0; i < 3; i++) {
        const stats_t *s = sensors[i];

        // Use scaling for fixed-point encoding:
        // Temp/Humidity are in °C and %, so we scale to store as integers
        // CO2 values are already integers, no scaling needed
        float scale = (i == 2) ? 1.0f : 100.0f;

        uint16_t std   = (uint16_t)(s->std_dev  * scale);
        uint16_t max_v = (uint16_t)(s->max      * scale);
        uint16_t min_v = (uint16_t)(s->min      * scale);
        uint16_t med   = (uint16_t)(s->median   * scale);

        // Offset calculation: 3 bytes header + (sensor_index * 8 bytes)
        int offset = 3 + i * 8;

        // Encode each 2-byte value in little-endian format
        payload[offset]     = (uint8_t)( std       & 0xFF);
        payload[offset + 1] = (uint8_t)((std >> 8) & 0xFF);

        payload[offset + 2] = (uint8_t)( max_v       & 0xFF);
        payload[offset + 3] = (uint8_t)((max_v >> 8) & 0xFF);

        payload[offset + 4] = (uint8_t)( min_v       & 0xFF);
        payload[offset + 5] = (uint8_t)((min_v >> 8) & 0xFF);

        payload[offset + 6] = (uint8_t)( med         & 0xFF);
        payload[offset + 7] = (uint8_t)((med >> 8)   & 0xFF);
    }
}
