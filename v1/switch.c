#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "raspberry_gpio.h"

#define BUTTON_PIN 17

void cleanup() {
    printf("Releasing control of GPIO pin #%d...\n", BUTTON_PIN);
    gpio_unexport(BUTTON_PIN);
}

void sigint_handler(int sig) {
    printf("Detected CTROL+C, cleaning up and exiting...\n");
    cleanup();
    exit(0);
}

int main() {
    int n = 0;
    signal(SIGINT, sigint_handler);
    printf("Exporting control of GPIO pin #%d...\n", BUTTON_PIN);
    if (gpio_export(BUTTON_PIN) == -1) {
        return 1;
    }
    printf("Setting direction of GPIO pin #%d to input...\n", BUTTON_PIN);
    if (gpio_direction(BUTTON_PIN, IN) == -1) {
        cleanup();
        return 1;
    }
    printf("Waiting for button press...\n");
    while (1) {
        int val = gpio_read(BUTTON_PIN);
        if (val == -1) {
            cleanup();
            return 1;
        } else if (val == 0) {
            printf("Button pressed; n = %d\n", ++n);
        }
        usleep(500 * 1000);
    }
}
