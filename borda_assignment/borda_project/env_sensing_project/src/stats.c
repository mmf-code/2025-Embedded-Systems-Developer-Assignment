#include "stats.h"
#include <math.h>   // for sqrtf
#include <string.h> // for memcpy
#include <stdlib.h> // for qsort

/**
 * @brief Comparison function for qsort to sort float values in ascending order.
 */
static int compare_floats(const void *a, const void *b) {
    float fa = *(const float*)a;
    float fb = *(const float*)b;
    return (fa > fb) - (fa < fb);
}

/**
 * @brief Computes basic statistics (mean, std deviation, min, max, median) 
 *        for a given array of float values.
 * 
 * @param data   Pointer to the array of input data
 * @param count  Number of elements in the array
 * @param result Pointer to the stats_t structure where results will be stored
 */
void compute_statistics(const float *data, uint8_t count, stats_t *result) {
    if (count == 0) return;

    float sum = 0.0f;
    result->min = data[0];
    result->max = data[0];

    // Compute min, max, and sum
    for (uint8_t i = 0; i < count; i++) {
        if (data[i] < result->min) result->min = data[i];
        if (data[i] > result->max) result->max = data[i];
        sum += data[i];
    }

    result->mean = sum / count;

    // Compute variance for standard deviation
    float variance = 0.0f;
    for (uint8_t i = 0; i < count; i++) {
        float diff = data[i] - result->mean;
        variance += diff * diff;
    }
    result->std_dev = sqrtf(variance / count);

    // Copy and sort data to compute median
    float sorted[32];  // Maximum expected buffer size
    memcpy(sorted, data, sizeof(float) * count);
    qsort(sorted, count, sizeof(float), compare_floats);

    // Compute median value
    if (count % 2 == 1) {
        result->median = sorted[count / 2];
    } else {
        result->median = (sorted[count / 2 - 1] + sorted[count / 2]) / 2.0f;
    }
}
