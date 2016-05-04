#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

int main() {
	int led;
	int n = 4;
	unsigned int buf;
 	struct timespec sleeptime;

	led = open("/dev/track", O_RDONLY);
	printf("%d\n", led);
	read(led, &buf, n);
	printf("%d\n", buf);
	


}
