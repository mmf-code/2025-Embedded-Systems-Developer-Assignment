#include "median_filter.h"
#include <string.h>  // for memcpy
#include <stdlib.h>  // for qsort

/**
 * @brief Comparison function for qsort, used to sort float values.
 */
static int compare_floats(const void *a, const void *b) {
    float fa = *(const float*)a;
    float fb = *(const float*)b;
    return (fa > fb) - (fa < fb);
}

/**
 * @brief Applies a moving median filter to a stream of float values.
 * 
 * The filter stores the latest N samples in a circular buffer, sorts them, and returns the median.
 * This helps to remove spikes and noise in sensor readings.
 * 
 * @param new_sample  New incoming data point
 * @param buffer      Circular buffer holding past values
 * @param window_size Size of the median filter window (max: MAX_WINDOW_SIZE)
 * @param index       Pointer to current index in the buffer (will be updated)
 * @param count       Pointer to the current sample count (will be updated up to window_size)
 * @return Median value of the current buffer
 */
float apply_median_filter(float new_sample, float *buffer, uint8_t window_size, uint8_t *index, uint8_t *count) {
    // Insert the new sample at the current index and move the index forward circularly
    buffer[*index] = new_sample;
    (*index) = ((*index) + 1) % window_size;

    // Increase the sample count up to the window size
    if (*count < window_size)
        (*count)++;

    // Create a sorted copy of the current buffer contents
    float sorted[MAX_WINDOW_SIZE];
    memcpy(sorted, buffer, sizeof(float) * (*count));
    qsort(sorted, *count, sizeof(float), compare_floats);

    // Return median value
    if (*count % 2 == 1) {
        return sorted[*count / 2];
    } else {
        return (sorted[*count / 2 - 1] + sorted[*count / 2]) / 2.0f;
    }
}
