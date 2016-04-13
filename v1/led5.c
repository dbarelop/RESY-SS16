#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "raspberry_gpio.h"

#define BUTTON_PIN 17
#define LED_PIN 18
#define LED_FREQ 5      // Blinking frequency in Hz

typedef struct params {
    pthread_mutex_t mutex;
    int *blink;
} params_t;

void cleanup() {
    printf("Releasing control of GPIO pin #%d and #%d...\n", BUTTON_PIN, LED_PIN);
    gpio_unexport(BUTTON_PIN);
    gpio_unexport(LED_PIN);
}

void sigint_handler(int sig) {
    printf("Detected CTROL+C, cleaning up and exiting...\n");
    cleanup();
    exit(0);
}

void *blink_function(void *arg) {
    struct timespec request;
    request.tv_sec = 0;
    request.tv_nsec = 1 / LED_FREQ * 1000000;

    params_t params = *((params_t *) arg);
    pthread_mutex_t mutex = params.mutex;
    int *blink = params.blink;

    // Configure LED GPIO
    printf("Exporting control of GPIO pin #%d (LED)...\n", LED_PIN);
    if (gpio_export(LED_PIN) == -1) {
        cleanup();
        exit(1);
    }
    printf("Setting direction of GPIO pin #%d (LED) to input...\n", LED_PIN);
    if (gpio_direction(LED_PIN, OUT) == -1) {
        cleanup();
        exit(1);
    }

    // Blink led
    int led_status = LOW;
    while (1) {
        pthread_mutex_lock(&mutex);
        int blink_val = *blink;
        pthread_mutex_unlock(&mutex);
        if (blink_val) {
            led_status = led_status == LOW ? HIGH : LOW;
            if (gpio_write(LED_PIN, led_status) == -1) {
                cleanup();
                exit(1);
            }
        }
        clock_nanosleep(CLOCK_REALTIME, 0, &request, NULL);
    }
}

int main() {
    params_t params;
    pthread_mutex_t mutex;
    int blink;
    pthread_t tid;

    // Catch SIGINT signal
    signal(SIGINT, sigint_handler);

    // Create thread for blinking led
    pthread_mutex_init(&mutex, NULL);
    blink = 1;
    params.mutex = mutex;
    params.blink = &blink;
    if (pthread_create(&tid, NULL, &blink_function, &params)) {
        fprintf(stderr, "Error creating thread\n");
        return 1;
    }

    // Configure button GPIO
    printf("Exporting control of GPIO pin #%d (button)...\n", BUTTON_PIN);
    if (gpio_export(BUTTON_PIN) == -1) {
        cleanup();
        return 1;
    }
    printf("Setting direction of GPIO pin #%d (button) to input...\n", BUTTON_PIN);
    if (gpio_direction(BUTTON_PIN, IN) == -1) {
        cleanup();
        return 1;
    }

    // Check whether button is pressed (pool every 500 ms)
    struct timespec request;
    request.tv_sec = 0;
    request.tv_nsec = 500 * 1000;
    while (1) {
        int button_val = gpio_read(BUTTON_PIN);
        if (button_val == -1) {
            cleanup();
            return 1;
        } else {
            printf("Button pressed, toggling blinking...\n");
            pthread_mutex_lock(&mutex);
            blink = !blink;
            pthread_mutex_unlock(&mutex);
        }
        clock_nanosleep(CLOCK_REALTIME, 0, &request, NULL);
    }
}
