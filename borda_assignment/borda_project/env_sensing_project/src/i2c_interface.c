#include "i2c_interface.h"
#include "bme280.h"
#include <fcntl.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define BME280_ADDR 0x76

static int initialized = 0;

/**
 * @brief Opens the I2C device file.
 * @param device_path Path to the I2C device (e.g., "/dev/i2c-1")
 * @return File descriptor on success, -1 on failure
 */
int i2c_open(const char *device_path) {
    return open(device_path, O_RDWR);
}

/**
 * @brief Sets the I2C slave address for subsequent communication.
 * @param fd File descriptor of the I2C device
 * @param addr 7-bit I2C address of the slave device
 * @return 0 on success, -1 on failure
 */
int i2c_set_slave(int fd, uint8_t addr) {
    return ioctl(fd, I2C_SLAVE, addr);
}

/**
 * @brief Writes a single byte to a specific register.
 * @param fd I2C device file descriptor
 * @param reg Register address to write to
 * @param data Data byte to write
 * @return 0 on success, -1 on failure
 */
int i2c_write_byte(int fd, uint8_t reg, uint8_t data) {
    uint8_t buffer[2] = {reg, data};
    if (write(fd, buffer, 2) != 2) {
        perror("I2C write error");
        return -1;
    }
    return 0;
}

/**
 * @brief Reads a single byte from a specific register.
 * @param fd I2C device file descriptor
 * @param reg Register address to read from
 * @param data Pointer to store the read byte
 * @return 0 on success, -1 on failure
 */
int i2c_read_byte(int fd, uint8_t reg, uint8_t *data) {
    if (write(fd, &reg, 1) != 1) {
        perror("I2C write error");
        return -1;
    }

    int bytes_read = read(fd, data, 1);
    if (bytes_read != 1) {
        perror("I2C read error");
        return -1;
    }

    printf("DEBUG: Register 0x%02X read → 0x%02X\n", reg, *data);
    return 0;
}

/**
 * @brief Reads multiple bytes starting from a specific register.
 * @param fd I2C device file descriptor
 * @param reg Starting register address
 * @param buf Buffer to store the read data
 * @param len Number of bytes to read
 * @return 0 on success, -1 on failure
 */
int i2c_read_bytes(int fd, uint8_t reg, uint8_t *buf, uint8_t len) {
    if (write(fd, &reg, 1) != 1) {
        perror("I2C multi-write error");
        return -1;
    }
    if (read(fd, buf, len) != len) {
        perror("I2C multi-read error");
        return -1;
    }
    return 0;
}

/**
 * @brief Closes the I2C device.
 * @param fd I2C device file descriptor
 * @return 0 on success, -1 on failure
 */
int i2c_close(int fd) {
    return close(fd);
}

/**
 * @brief Simulates sensor readings based on the device address and sensor type.
 * This is a mock function to allow development without actual hardware.
 * @param device_address I2C address of the simulated device
 * @param type Type of the sensor to simulate
 * @return Simulated sensor reading, or -1.0 if invalid
 */
float i2c_sensor_read(uint8_t device_address, sensor_type_t type) {
    // Initialize random generator once
    if (!initialized) {
        srand(time(NULL));
        initialized = 1;
    }

    // Simulated data sources
    if (device_address == BME280_ADDR) {
        switch (type) {
            case SENSOR_HUMIDITY:
                return bme280_read_humidity();  // Simulated humidity
            case SENSOR_PRESSURE:
                return bme280_read_pressure();  // Simulated pressure
            default:
                return -1.0f; // Temperature is not handled here
        }
    }

    // Simulated CO₂ sensor
    if (device_address == 0x5A && type == SENSOR_CO2) {
        return 400 + rand() % 200;  // Random value between 400–600 ppm
    }

    // Simulated light sensor
    if (device_address == 0x62 && type == SENSOR_LIGHT) {
        return 100 + rand() % 900;  // Random value between 100–1000 lux
    }

    // Unknown device or sensor type
    return -1.0f;
}
