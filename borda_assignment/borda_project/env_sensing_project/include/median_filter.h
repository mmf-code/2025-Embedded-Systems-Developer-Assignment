#ifndef MEDIAN_FILTER_H
#define MEDIAN_FILTER_H

#include <stdint.h>

#define MAX_WINDOW_SIZE 11  // Gerekirse büyütülebilir

// Medyan filtreleme fonksiyonu
float apply_median_filter(float new_sample, float *buffer, uint8_t window_size, uint8_t *index, uint8_t *count);

#endif // MEDIAN_FILTER_H
