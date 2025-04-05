#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include "circular_buffer.h"

#define PRODUCE_INTERVAL 1      // Production interval (seconds)
#define BUFFER_CAPACITY 10      // Max buffer size

circular_buffer_t buffer;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;

/**
 * @brief Producer thread function.
 *        Generates mock sensor data every second and adds it to the shared buffer.
 *        Waits if the buffer is full.
 */
void* producer_thread(void* arg) {
    srand(time(NULL));
    while (1) {
        sleep(PRODUCE_INTERVAL);

        // Generate random sensor data
        sensor_data_t data = {
            .temperature = 25.0 + (rand() % 100) / 10.0,
            .humidity    = 40.0 + (rand() % 300) / 10.0,
            .co2         = 400 + rand() % 200,
            .timestamp   = time(NULL)
        };

        pthread_mutex_lock(&mutex);

        // Wait until buffer has space
        while (cb_is_full(&buffer)) {
            pthread_cond_wait(&not_full, &mutex);
        }

        cb_push(&buffer, data);
        printf("🟢 Producer: Temp=%.2f Hum=%.2f CO₂=%.2f\n",
               data.temperature, data.humidity, data.co2);

        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&not_empty);  // Notify consumer that data is available
    }
    return NULL;
}

/**
 * @brief Consumer thread function.
 *        Continuously consumes data from the buffer and prints it.
 *        Waits if the buffer is empty.
 */
void* consumer_thread(void* arg) {
    while (1) {
        pthread_mutex_lock(&mutex);

        // Wait until there is data in the buffer
        while (cb_is_empty(&buffer)) {
            pthread_cond_wait(&not_empty, &mutex);
        }

        sensor_data_t data = cb_pop(&buffer);
        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&not_full);  // Notify producer that space is available

        // Format and print timestamped output
        char time_str[26];
        ctime_r(&data.timestamp, time_str);
        time_str[strcspn(time_str, "\n")] = '\0';

        printf("🔵 Consumer: [%s] Temp=%.2f°C | Hum=%.2f%% | CO₂=%.2f ppm\n",
               time_str, data.temperature, data.humidity, data.co2);

        // Simulate filtering/processing delay
        usleep(500 * 1000);  // 500 ms
    }
    return NULL;
}

/**
 * @brief Initializes buffer and starts producer and consumer threads.
 */
int main() {
    cb_init(&buffer);

    pthread_t producer, consumer;
    pthread_create(&producer, NULL, producer_thread, NULL);
    pthread_create(&consumer, NULL, consumer_thread, NULL);

    pthread_join(producer, NULL);
    pthread_join(consumer, NULL);

    return 0;
}
