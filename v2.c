/*
 * v2.c
 * 
 * source: Moderne Realzeitsysteme kompakt
 */


#include <stdio.h>
#include <sys/times.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>

#define NANOSECONDS_PER_SECOND 1000000000

struct timespec * diff_time(struct timespec before, struct timespec after, struct timespec *result)
{
	if (result == NULL)
		return NULL;

	if ((after.tv_sec<before.tv_sec) || 
		((after.tv_sec==before.tv_sec) && 
		(after.tv_nsec<=before.tv_nsec)))
	{
		result->tv_sec = result->tv_nsec = 0;
	}

	result->tv_sec = after.tv_sec - before.tv_sec;
	result->tv_nsec = after.tv_nsec - before.tv_nsec;
	
	if (result->tv_nsec<0)
	{
		result->tv_sec--;
		result->tv_nsec = NANOSECONDS_PER_SECOND+result->tv_nsec;
	}

	return result;
}

int
main (int argc, char *argv[])
{

	struct timespec req, rem, start, end, diff;
	int error;
	long max, delay;

	req.tv_sec = 0;
	req.tv_nsec = 10000*1000;
	max = 0;

	printf("Starting measurment with period[us]: 10000 f0r 100 loops:\n");


	int i;
	for (i=0; i<100; i++) {
		clock_gettime(CLOCK_MONOTONIC, &start);
		if ((error=clock_nanosleep(CLOCK_MONOTONIC, 0, &req, NULL))!=0)
		{
			printf("clock_nanosleep reporting error %d\n",error);
		}
		clock_gettime(CLOCK_MONOTONIC, &end);
		diff_time(start, end, &diff);
		delay = (diff.tv_nsec-req.tv_nsec)/1000;
		printf("value:%ld usec, delay: %ld usec, raw_data: %ld\n", diff.tv_nsec/1000, delay, diff.tv_nsec);
		if (delay > max)
		{
			max = diff.tv_nsec;
		}
	}

	printf("Max: %ld usec\n", max/1000);
    

}

