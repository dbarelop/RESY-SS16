/*
 * v2.c
 * 
 * source: Moderne Realzeitsysteme kompakt
 */


#include <stdio.h>
#include <stdlib.h>
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

typedef struct params {
	long min, max, step;
	char * filename;
} params_t;


void sleep_function(void * arg)
{
	params_t params = *((params_t *) arg);
	long min = params.min;
	long max = params.max;
	long step = params.step;
	int i,j,error;
	char * filename = params.filename;
	FILE * fp;
	struct timespec req, start, end, diff;
	long max_delay, delay;


	req.tv_sec = 0;
	max_delay = 0;
	int fors = ((max-min) / step)+1;
	
	fp = fopen(filename, "w+");
	for (j=0; j < fors; j++)
	{
		req.tv_nsec = (min+j*step)*1000;

		//printf("Starting measurment with period[us]: %d f0r 100 loops:\n", req.tv_nsec/1000);
		
		for (i=0; i<100; i++)
		{
			clock_gettime(CLOCK_MONOTONIC, &start);
			if ((error=clock_nanosleep(CLOCK_MONOTONIC, 0, &req, NULL))!=0)
			{
				printf("clock_nanosleep reporting error %d\n",error);
			}
			clock_gettime(CLOCK_MONOTONIC, &end);
			diff_time(start, end, &diff);
			delay = (diff.tv_nsec-req.tv_nsec)/1000;
			//printf("value:%ld usec, delay: %ld usec, raw_data: %ld\n", diff.tv_nsec/1000, delay, diff.tv_nsec);
			if (delay > max_delay)
			{
				max_delay = delay;
			}
		}
		//printf("Max: %ld usec\n", max_delay);
		//printf("-------------------\n");
		fprintf(fp, "%lu %lu\n", req.tv_nsec/1000, max_delay);
		fprintf(stdout, "%lu %lu\n", req.tv_nsec/1000, max_delay);
		max_delay = 0;
	}

	fclose(fp);
	exit(1);










}



int
main (int argc, char *argv[])
{

	int rtflag = 0;
	int i,j;
	params_t params;
	pthread_t tid;	


	if (argc < 9)
	{
		printf("please give all arguments (-min -max -step -out)\n");		
		return 0;
	}


	for(i = 1; i < argc; i++)
	{	

		if (strcmp("-min",argv[i]) == 0)
		{
			params.min = atol(argv[i+1]);
			
		}
		else if (strcmp("-max",argv[i]) == 0)
		{
			params.max = atol(argv[i+1]);
		}
		else if (strcmp("-step",argv[i]) == 0)
		{
			params.step = atol(argv[i+1]);
		}
		else if (strcmp("-out",argv[i]) == 0)
		{
			params.filename = argv[i+1];
		}
		else if (strcmp("-out",argv[i]) == 0)
		{
			rtflag = 1;
		}
		
	}


	if (rtflag == 0)
	{
		if (pthread_create(&tid, NULL, &sleep_function, &params))
		{
		    fprintf(stderr, "Error creating thread\n");
		    return 1;
		}
		printf("Thread started\n

    }


		
	//printf("%ld %ld %ld %s \n", min, max, step, filename);


	pthread_join(tid, NULL);
	return 0;
}
