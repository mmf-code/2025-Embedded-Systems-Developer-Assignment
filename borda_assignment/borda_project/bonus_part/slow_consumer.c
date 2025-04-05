#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include "circular_buffer.h"

#define PRODUCE_INTERVAL 1      // Interval between each produced item (in seconds)
#define BUFFER_CAPACITY 10      // Maximum capacity of the buffer

circular_buffer_t buffer;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;

/**
 * @brief Producer thread function
 *        Periodically generates synthetic sensor data and adds it to the buffer.
 *        If the buffer is full, the data is dropped and logged to file.
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

        if (cb_is_full(&buffer)) {
            // Buffer is full → log dropped data to file
            FILE *logf = fopen("buffer_overflow.log", "a");
            if (logf) {
                char time_str[26];
                ctime_r(&data.timestamp, time_str);
                time_str[strcspn(time_str, "\n")] = '\0';  // Remove newline

                fprintf(logf, "[%s] ⚠️ Buffer full — dropped: T=%.2f H=%.2f CO₂=%.2f\n",
                        time_str, data.temperature, data.humidity, data.co2);
                fclose(logf);
            }

            printf("⚠️  Producer: Buffer full, data dropped (T=%.2f)\n", data.temperature);
        } else {
            cb_push(&buffer, data);
            printf("🟢 Producer: Temp=%.2f Hum=%.2f CO₂=%.2f\n",
                   data.temperature, data.humidity, data.co2);
            pthread_cond_signal(&not_empty);
        }

        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

/**
 * @brief Consumer thread function
 *        Waits for data to appear in the buffer and consumes it.
 *        Simulates slower processing to demonstrate producer-consumer imbalance.
 */
void* consumer_thread(void* arg) {
    while (1) {
        pthread_mutex_lock(&mutex);
        while (cb_is_empty(&buffer)) {
            pthread_cond_wait(&not_empty, &mutex);
        }

        sensor_data_t data = cb_pop(&buffer);
        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&not_full);

        // Format timestamp
        char time_str[26];
        ctime_r(&data.timestamp, time_str);
        time_str[strcspn(time_str, "\n")] = '\0';

        printf("🔵 Consumer: [%s] Temp=%.2f°C | Hum=%.2f%% | CO₂=%.2f ppm\n",
               time_str, data.temperature, data.humidity, data.co2);

        // Simulated slow consumer (2 sec), to create backlog
        usleep(2000 * 1000);
    }
    return NULL;
}

/**
 * @brief Initializes the circular buffer and starts producer and consumer threads.
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
