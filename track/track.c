#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "track.h"


int main() {
	key_t key;
	struct shmid_ds shmid_ds;
	int leds, l_one, l_two, l_three, l_four;
	int shmid;
	int n = 4;
	struct timespec sleeptime;
	unsigned int buf;
	struct led_status *struct_led_status_shared;
	struct led_status *struct_led_status_local;
	
	struct_led_status_local = (struct led_status*) malloc( sizeof( struct led_status));
	leds = open("/dev/track", O_RDONLY);
	printf("%d\n", leds);
	read(leds, &buf, n);
	printf("%d\n", buf);
	
	key = 9002;
  	if ((shmid = shmget(key, sizeof(struct led_status), IPC_CREAT | 0666)) < 0) {
	    perror("shmget");
	    exit(1);
	}
	if ((struct_led_status_shared = shmat(shmid, NULL, 0))== (struct led_status*) -1) {
	    perror("shmat");
	    exit(1);
	}
	struct_led_status_local = struct_led_status_shared;
	struct_led_status_local -> led_one = 0;
	struct_led_status_local -> led_two = 0;
	struct_led_status_local -> led_three = 0;
	struct_led_status_local -> led_four = 0;

	while(1) {
		read(leds, &buf, n);
		if ( (buf - 1000) >= 0) {
			l_one = 1;
			buf = buf - 1000;
		}
		else
			l_one = 0;
		if ( (buf - 100) >= 0) {
			l_two = 1;
			buf = buf - 100;
		}
		else
			l_two = 0;
		if ( (buf -10) >= 0) {
			l_three = 1;
			buf = buf - 10;
		}
		else
			l_three = 0;
		if ( buf == 1)
			l_four = 1;
		else
			l_four = 0;
		while (shmctl(shmid, SHM_LOCK, &shmid_ds) == -1) {
		}
		struct_led_status_local -> led_one = l_one;
		struct_led_status_local -> led_two = l_two;
		struct_led_status_local -> led_three = l_three;
		struct_led_status_local -> led_four = l_four;
		shmctl(shmid, SHM_UNLOCK, &shmid_ds);
		sleeptime.tv_sec = 1;
		clock_nanosleep( CLOCK_MONOTONIC, 0, &sleeptime, NULL);
	}
	return 0;
}
