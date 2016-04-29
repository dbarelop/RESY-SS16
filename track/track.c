#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

int main() {
	int led;
 	struct timespec sleeptime;

	led = open("/dev/track", O_RDONLY);
	printf("%d",led);
	
}
