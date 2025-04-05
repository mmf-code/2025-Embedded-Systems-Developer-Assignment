#include "circular_buffer.h"
#include <stdio.h>  // for printf (used in debug/logging)

/**
 * @brief Initializes the circular buffer by resetting indices and count.
 */
void cb_init(circular_buffer_t *cb) {
    cb->head = 0;
    cb->tail = 0;
    cb->count = 0;
}

/**
 * @brief Checks if the buffer is empty.
 * @return true if empty, false otherwise
 */
bool cb_is_empty(circular_buffer_t *cb) {
    return cb->count == 0;
}

/**
 * @brief Pushes a new item to the buffer if it's not full.
 *        Does nothing if the buffer is full.
 * @param cb Pointer to circular buffer
 * @param item Data to insert
 */
void cb_push(circular_buffer_t *cb, sensor_data_t item) {
    if (cb_is_full(cb)) return;

    cb->data[cb->head] = item;
    cb->head = (cb->head + 1) % BUFFER_SIZE;
    cb->count++;
}

/**
 * @brief Pops the oldest item from the buffer if it's not empty.
 *        Returns an empty item (zeroed) if buffer is empty.
 * @param cb Pointer to circular buffer
 * @return The oldest sensor data item
 */
sensor_data_t cb_pop(circular_buffer_t *cb) {
    sensor_data_t item = {0};
    if (cb_is_empty(cb)) return item;

    item = cb->data[cb->tail];
    cb->tail = (cb->tail + 1) % BUFFER_SIZE;
    cb->count--;
    return item;
}

/**
 * @brief Checks if the buffer is full.
 *        Also logs a message when buffer reaches full capacity.
 *        Useful for triggering overflow alerts or diagnostics.
 * @param cb Pointer to circular buffer
 * @return true if full, false otherwise
 */
bool cb_is_full(circular_buffer_t *cb) {
    if (cb->count == BUFFER_SIZE) {
        printf("💥 Buffer is full! Logging should be triggered!\n");
    }
    return cb->count == BUFFER_SIZE;
}

// Alternative implementations for cb_is_full() can be uncommented if needed:
// bool cb_is_full(const circular_buffer_t *cb) { return cb->count == BUFFER_SIZE; }
