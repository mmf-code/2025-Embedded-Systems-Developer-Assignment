#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "i2c_interface.h"
#include "bme280.h"
#include "median_filter.h"
#include "circular_buffer.h"
#include "stats.h"
#include "ble_payload.h"

#define WINDOW_SIZE 5                     // Median filter window size
#define I2C_DEV "/dev/i2c-1"              // I2C device path on Linux
#define MEASUREMENT_INTERVAL_SEC 1       // Sensor read interval
#define BLE_UPDATE_INTERVAL_SEC 3        // BLE payload update interval

volatile bool keep_running = true;

/**
 * @brief Signal handler for graceful termination via Ctrl+C
 */
void handle_sigint(int sig) {
    (void)sig;
    keep_running = false;
    printf("\n🛑 Terminating program...\n");
}

int main() {
    signal(SIGINT, handle_sigint);

    // Open I2C interface
    int fd = i2c_open(I2C_DEV);
    if (fd < 0) {
        perror("Failed to open I2C device");
        return 1;
    }

    // Set slave address for BME280
    if (i2c_set_slave(fd, BME280_ADDR) < 0) {
        perror("Failed to set I2C address");
        i2c_close(fd);
        return 1;
    }

    // Check sensor ID to ensure it's a BME280
    uint8_t chip_id;
    if (bme280_read_chip_id(fd, &chip_id) != 0 || chip_id != 0x60) {
        printf("❌ BME280 sensor not detected!\n");
        i2c_close(fd);
        return 1;
    }
    printf("✔️ BME280 sensor found. ID: 0x%02X\n", chip_id);

    // Configure BME280 and read calibration data
    bme280_configure(fd);
    uint16_t T1;
    int16_t T2, T3;
    if (bme280_read_calibration(fd, &T1, &T2, &T3) != 0) {
        printf("❌ Failed to read calibration data.\n");
        i2c_close(fd);
        return 1;
    }

    // Buffers for filtering and storage
    float temp_filter_buf[WINDOW_SIZE] = {0};
    uint8_t temp_index = 0, temp_count = 0;

    circular_buffer_t temp_cb, hum_cb, co2_cb;
    cb_init(&temp_cb);
    cb_init(&hum_cb);
    cb_init(&co2_cb);

    int tick = 0;

    while (keep_running) {
        // Read raw temperature value from BME280
        int32_t raw_temp;
        if (bme280_read_raw_temp(fd, &raw_temp) != 0) {
            printf("❌ Failed to read raw temperature.\n");
            sleep(MEASUREMENT_INTERVAL_SEC);
            continue;
        }

        // Convert raw temperature using calibration values
        float temp = bme280_calibrate_temp(raw_temp, T1, T2, T3);

        // Read simulated humidity and CO₂ values from mock I2C devices
        float hum  = i2c_sensor_read(0x76, SENSOR_HUMIDITY);
        float co2  = i2c_sensor_read(0x5A, SENSOR_CO2);

        // Apply moving median filter to temperature
        float median_temp = apply_median_filter(temp, temp_filter_buf, WINDOW_SIZE, &temp_index, &temp_count);

        // Store filtered values in circular buffers
        cb_push(&temp_cb, median_temp);
        cb_push(&hum_cb, hum);
        cb_push(&co2_cb, co2);

        // Print raw values
        printf("\n📥 New Measurement\n");
        printf("🌡️  Temperature : %.2f °C\n", temp);
        printf("💧 Humidity    : %.2f %%\n", hum);
        printf("🫁 CO₂         : %.2f ppm\n", co2);

        // Every BLE_UPDATE_INTERVAL_SEC seconds, update BLE packet
        if (++tick % BLE_UPDATE_INTERVAL_SEC == 0) {
            float buf_temp[BUFFER_SIZE], buf_hum[BUFFER_SIZE], buf_co2[BUFFER_SIZE];
            uint8_t count_temp = cb_get_all(&temp_cb, buf_temp);
            uint8_t count_hum  = cb_get_all(&hum_cb, buf_hum);
            uint8_t count_co2  = cb_get_all(&co2_cb, buf_co2);

            stats_t stats_temp, stats_hum, stats_co2;
            compute_statistics(buf_temp, count_temp, &stats_temp);
            compute_statistics(buf_hum,  count_hum,  &stats_hum);
            compute_statistics(buf_co2,  count_co2,  &stats_co2);

            // Prepare BLE advertising payload
            uint8_t payload[BLE_PAYLOAD_SIZE];
            encode_ble_advertising_data(payload, &stats_temp, &stats_hum, &stats_co2);

            // Write payload to file for external BLE advertiser to read
            FILE *f = fopen("payload.bin", "wb");
            if (f) {
                fwrite(payload, sizeof(uint8_t), BLE_PAYLOAD_SIZE, f);
                fclose(f);
            }

            // Print computed statistics
            printf("📡 BLE Updated\n");
            printf("📊 Temp → Mean: %.2f  Min: %.2f  Max: %.2f  Med: %.2f  Std: %.2f\n",
                stats_temp.mean, stats_temp.min, stats_temp.max, stats_temp.median, stats_temp.std_dev);
            printf("📊 Hum  → Mean: %.2f  Min: %.2f  Max: %.2f  Med: %.2f  Std: %.2f\n",
                stats_hum.mean, stats_hum.min, stats_hum.max, stats_hum.median, stats_hum.std_dev);
            printf("📊 CO₂  → Mean: %.2f  Min: %.2f  Max: %.2f  Med: %.2f  Std: %.2f\n",
                stats_co2.mean, stats_co2.min, stats_co2.max, stats_co2.median, stats_co2.std_dev);
        }

        sleep(MEASUREMENT_INTERVAL_SEC);
    }

    i2c_close(fd);
    printf("✅ Program exited successfully.\n");
    return 0;
}
