#include "circular_buffer.h"

/**
 * @brief Initializes the circular buffer.
 *        Sets head to 0, count to 0, and clears buffer contents.
 * @param cb Pointer to the circular buffer structure
 */
void cb_init(circular_buffer_t *cb) {
    cb->head = 0;
    cb->count = 0;
    for (uint8_t i = 0; i < BUFFER_SIZE; i++) {
        cb->data[i] = 0.0f;
    }
}

/**
 * @brief Adds a new element to the circular buffer.
 *        Overwrites the oldest data if the buffer is full.
 * @param cb Pointer to the circular buffer structure
 * @param value New float value to insert
 */
void cb_push(circular_buffer_t *cb, float value) {
    cb->data[cb->head] = value;
    cb->head = (cb->head + 1) % BUFFER_SIZE;

    // Increase count until buffer is full
    if (cb->count < BUFFER_SIZE) {
        cb->count++;
    }
}

/**
 * @brief Copies all valid entries from the circular buffer to an external array
 *        in correct (oldest-to-newest) order.
 * @param cb Pointer to the circular buffer structure
 * @param out_array Output array to fill with values
 * @return Number of valid entries copied
 */
uint8_t cb_get_all(const circular_buffer_t *cb, float *out_array) {
    for (uint8_t i = 0; i < cb->count; i++) {
        uint8_t index = (cb->head + BUFFER_SIZE - cb->count + i) % BUFFER_SIZE;
        out_array[i] = cb->data[index];
    }
    return cb->count;
}
