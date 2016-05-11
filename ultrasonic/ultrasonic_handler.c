#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <time.h>
#include <unistd.h>
#include "ultrasonic_handler.h"

#define SLEEP_TIME_NS 250e6

int ultrasonic_fd;

void sigint_handler(int sig) {
    printf("Detected CTROL+C, cleaning up and exiting...\n");
    close(ultrasonic_fd);
    exit(0);
}

int main(int argc, char **argv) {
    key_t key;
    struct shmid_ds shmid_ds;
    int distance, shmid;
    unsigned long long buf;
    struct timespec sleeptime;
    struct ultrasonic_distance *ultrasonic_distance_shared;

    signal(SIGINT, sigint_handler);
    sleeptime.tv_sec = 0;
    sleeptime.tv_nsec = SLEEP_TIME_NS;
    ultrasonic_fd = open("/dev/ultrasonic", O_RDONLY);

    key = 9003;
    if ((shmid = shmget(key, sizeof(struct ultrasonic_distance), IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(1);
    }
    if ((ultrasonic_distance_shared = shmat(shmid, NULL, 0)) < 0) {
        perror("shmat");
        exit(1);
    }
    
    while (1) {
        read(ultrasonic_fd, &buf, sizeof(buf));
        printf("%llu ns\n", buf);
        while (shmctl(shmid, SHM_LOCK, &shmid_ds) == -1);
        ultrasonic_distance_shared->distance = buf;
        shmctl(shmid, SHM_UNLOCK, &shmid_ds);
        clock_nanosleep(CLOCK_MONOTONIC, 0, &sleeptime, NULL);
    }

    return 0;
}
