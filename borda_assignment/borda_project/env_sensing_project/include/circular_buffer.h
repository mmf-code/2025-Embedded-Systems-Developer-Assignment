#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include <stdint.h>

#define BUFFER_SIZE 50

typedef struct {
    float data[BUFFER_SIZE];
    uint8_t head;
    uint8_t count;
} circular_buffer_t;

void cb_init(circular_buffer_t *cb);
void cb_push(circular_buffer_t *cb, float value);
uint8_t cb_get_all(const circular_buffer_t *cb, float *out_array);

#endif // CIRCULAR_BUFFER_H
