#ifndef BME280_H
#define BME280_H

#include <stdint.h>

#define BME280_ADDR 0x76
#define BME280_REG_ID 0xD0

int bme280_read_chip_id(int fd, uint8_t *chip_id);
int bme280_configure(int fd);
int bme280_read_raw_temp(int fd, int32_t *raw_temp);
int bme280_read_calibration(int fd, uint16_t *T1, int16_t *T2, int16_t *T3);
float bme280_calibrate_temp(int32_t raw_temp, uint16_t T1, int16_t T2, int16_t T3);
float bme280_read_temperature(void); // opsiyonel — eğer simüle ediyorsan
float bme280_read_humidity(void);    // simülasyon
float bme280_read_pressure(void);    // simülasyon


#endif // BME280_H
