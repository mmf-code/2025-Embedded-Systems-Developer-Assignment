CC=gcc
CFLAGS=-Wall -Iinclude

SRC = src/main.c src/bme280.c src/i2c_interface.c src/median_filter.c src/circular_buffer.c src/stats.c src/ble_payload.c

all:
	$(CC) $(CFLAGS) $(SRC) -lm -o env_sensor

clean:
	rm -f env_sensor
