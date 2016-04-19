#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "raspberry_gpio.h"

int gpio_export(int pin) {
    int fd = open("/sys/class/gpio/export", O_WRONLY);
    if (fd == -1) {
        fprintf(stderr, "* Failed to open export file\n");
        return -1;
    }
    char buffer[3];
    ssize_t bytes_written = snprintf(buffer, 3, "%d", pin);
    write(fd, buffer, bytes_written);
    close(fd);

    return 0;
}

int gpio_unexport(int pin) {
    int fd = open("/sys/class/gpio/unexport", O_WRONLY);
    if (fd == -1) {
        fprintf(stderr, "* Failed to open unexport file\n");
        return -1;
    }
    char buffer[3];
    ssize_t bytes_written = snprintf(buffer, 3, "%d", pin);
    write(fd, buffer, bytes_written);
    close(fd);

    return 0;
}

int gpio_direction(int pin, int dir) {
    static const char directions[] = "in\0out";
    char path[64];
    snprintf(path, 64, "/sys/class/gpio/gpio%d/direction", pin);
    int fd = open(path, O_WRONLY);
    if (fd == -1) {
        fprintf(stderr, "* Failed to open gpio direction file\n");
        return -1;
    }
    if (write(fd, &directions[IN == dir ? 0 : 3], IN == dir ? 2 : 3) == -1) {
        fprintf(stderr, "* Failed to set gpio direction\n");
        return -1;
    }
    close(fd);

    return 0;
}

int gpio_read(int pin) {
    char path[64];
    snprintf(path, 64, "/sys/class/gpio/gpio%d/value", pin);
    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        fprintf(stderr, "* Failed to open gpio file for reading\n");
        return -1;
    }
    char value[3];
    if (read(fd, value, 3) == -1) {
        fprintf(stderr, "* Failed to read from gpio\n");
        return -1;
    }
    close(fd);

    return atoi(value);
}

int gpio_write(int pin, int value) {
    static const char values[] = "01";
    char path[64];
    snprintf(path, 64, "/sys/class/gpio/gpio%d/value", pin);
    int fd = open(path, O_WRONLY);
    if (fd == -1) {
        fprintf(stderr, "* Failed to open gpio file for writing\n");
        return -1;
    }
    if (write(fd, &values[LOW == value ? 0 : 1], 1) == -1) {
        fprintf(stderr, "* Failed to write to gpio\n");
        return -1;
    }
    close(fd);

    return 0;
}
