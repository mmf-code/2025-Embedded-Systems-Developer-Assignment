#include "bme280.h"
#include "i2c_interface.h"
#include <unistd.h>
#include <stdio.h>

/**
 * @brief Reads the BME280 chip ID to verify sensor presence.
 * @param fd I2C file descriptor
 * @param chip_id Pointer to store the read chip ID
 * @return 0 on success, -1 on failure
 */
int bme280_read_chip_id(int fd, uint8_t *chip_id) {
    return i2c_read_byte(fd, BME280_REG_ID, chip_id);
}

/**
 * @brief Configures the BME280 sensor with predefined control register values.
 *        These include humidity oversampling, measurement control, and config settings.
 * @param fd I2C file descriptor
 * @return 0 on success
 */
int bme280_configure(int fd) {
    // Humidity oversampling x1
    i2c_write_byte(fd, 0xF2, 0x01);

    // Temp and pressure oversampling x1, mode = normal
    i2c_write_byte(fd, 0xF4, 0x27);

    // Standby time 1000ms, filter off
    i2c_write_byte(fd, 0xF5, 0xA0);

    sleep(1); // Wait for settings to take effect
    return 0;
}

/**
 * @brief Reads raw temperature data (20-bit) from the sensor registers.
 * @param fd I2C file descriptor
 * @param raw_temp Pointer to store the raw temperature value
 * @return 0 on success, -1 on failure
 */
int bme280_read_raw_temp(int fd, int32_t *raw_temp) {
    uint8_t data[3];
    if (i2c_read_bytes(fd, 0xFA, data, 3) != 0)
        return -1;

    // Combine MSB, LSB, XLSB (20-bit value)
    *raw_temp = ((int32_t)data[0] << 12) | ((int32_t)data[1] << 4) | (data[2] >> 4);
    return 0;
}

/**
 * @brief Reads temperature calibration parameters from the sensor.
 *        These are needed for accurate compensation.
 * @param fd I2C file descriptor
 * @param T1,T2,T3 Pointers to store calibration parameters
 * @return 0 on success, -1 on failure
 */
int bme280_read_calibration(int fd, uint16_t *T1, int16_t *T2, int16_t *T3) {
    uint8_t calib[6];
    if (i2c_read_bytes(fd, 0x88, calib, 6) != 0)
        return -1;

    *T1 = (uint16_t)(calib[1] << 8 | calib[0]);
    *T2 = (int16_t)(calib[3] << 8 | calib[2]);
    *T3 = (int16_t)(calib[5] << 8 | calib[4]);

    return 0;
}

/**
 * @brief Applies compensation algorithm to raw temperature using BME280 formula.
 *        Uses T1, T2, T3 calibration coefficients.
 * @param adc_T Raw temperature from bme280_read_raw_temp()
 * @param T1,T2,T3 Calibration parameters
 * @return Compensated temperature in degrees Celsius
 */
float bme280_calibrate_temp(int32_t adc_T, uint16_t T1, int16_t T2, int16_t T3) {
    int32_t var1 = ((((adc_T >> 3) - ((int32_t)T1 << 1))) * ((int32_t)T2)) >> 11;
    int32_t var2 = (((((adc_T >> 4) - ((int32_t)T1)) *
                      ((adc_T >> 4) - ((int32_t)T1))) >> 12) *
                    ((int32_t)T3)) >> 14;

    int32_t t_fine = var1 + var2;
    float T = (t_fine * 5 + 128) >> 8;
    return T / 100.0f;
}

#include <stdlib.h>
#include <time.h>

static int bme280_sim_init = 0;

/**
 * @brief Simulates BME280 temperature value for testing without real hardware.
 * @return Simulated temperature in Celsius
 */
float bme280_read_temperature(void) {
    if (!bme280_sim_init) {
        srand(time(NULL));
        bme280_sim_init = 1;
    }
    return 24.0 + (rand() % 500) / 100.0;  // Range: 24.00 - 29.00 °C
}

/**
 * @brief Simulates BME280 humidity value for testing without real hardware.
 * @return Simulated humidity in percentage
 */
float bme280_read_humidity(void) {
    return 40.0 + (rand() % 300) / 10.0;   // Range: 40.0 - 70.0 %
}

/**
 * @brief Simulates BME280 pressure value for testing without real hardware.
 * @return Simulated pressure in hPa
 */
float bme280_read_pressure(void) {
    return 1000.0 + (rand() % 100);        // Range: 1000 - 1099 hPa
}
