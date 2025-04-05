#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include <stdbool.h>
#include <stdint.h>
#include <time.h>


typedef struct {
    float temperature;
    float humidity;
    float co2;
    time_t timestamp;
} sensor_data_t;

#define BUFFER_SIZE 3 // slow_consumer için 10'dan 3 e düşürdüm

typedef struct {
    sensor_data_t data[BUFFER_SIZE];
    int head;
    int tail;
    int count;
} circular_buffer_t;

void cb_init(circular_buffer_t *cb);
bool cb_is_empty(circular_buffer_t *cb);
bool cb_is_full(circular_buffer_t *cb);
void cb_push(circular_buffer_t *cb, sensor_data_t item);
sensor_data_t cb_pop(circular_buffer_t *cb);

#endif
