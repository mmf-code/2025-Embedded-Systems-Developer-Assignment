#ifndef STATS_H
#define STATS_H

#include <stdint.h>

// Stat sonuçlarını saklayacak yapı
typedef struct {
    float min;
    float max;
    float mean;
    float std_dev;
    float median;
} stats_t;

// Hesaplama fonksiyonu
void compute_statistics(const float *data, uint8_t count, stats_t *result);

#endif // STATS_H
