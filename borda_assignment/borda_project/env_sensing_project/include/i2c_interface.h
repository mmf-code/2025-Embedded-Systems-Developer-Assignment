#ifndef I2C_INTERFACE_H
#define I2C_INTERFACE_H

#include <stdint.h>

typedef enum {
    SENSOR_TEMP,
    SENSOR_HUMIDITY,
    SENSOR_PRESSURE,
    SENSOR_CO2,
    SENSOR_LIGHT
} sensor_type_t;

float i2c_sensor_read(uint8_t device_address, sensor_type_t type);


int i2c_open(const char *device_path);
int i2c_set_slave(int fd, uint8_t addr);
int i2c_write_byte(int fd, uint8_t reg, uint8_t data);
int i2c_read_byte(int fd, uint8_t reg, uint8_t *data);
int i2c_read_bytes(int fd, uint8_t reg, uint8_t *buf, uint8_t len);
int i2c_close(int fd);

#endif // I2C_INTERFACE_H
