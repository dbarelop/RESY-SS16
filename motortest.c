#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

int main( int argc, char **argv, char **envp )
{
  int letter, left=0, right=0;
  int fd_right, fd_left;
  struct timespec sleeptime;

  fd_left = open("/dev/motor-left", O_WRONLY);
  fd_right= open("/dev/motor-right", O_WRONLY);
  if (fd_left<0 || fd_right<0) {
    perror("open");
    return -1;
  }

  left = 1;
  right = 1;
  write( fd_left, &left, sizeof(left) );
  write( fd_right, &right, sizeof(right) );

  sleeptime.tv_sec = 1;
  sleeptime.tv_nsec = 500000000;
  clock_nanosleep( CLOCK_MONOTONIC,0,&sleeptime,NULL);

  left = -1;
  right= -1;
  write( fd_left, &left, sizeof(left) );
  write( fd_right, &right, sizeof(right) );

  sleeptime.tv_sec = 1;
  sleeptime.tv_nsec = 500000000;
  clock_nanosleep( CLOCK_MONOTONIC,0,&sleeptime,NULL);

  left = 0;
  right= 0;
  write( fd_left, &left, sizeof(left) );
  write( fd_right, &right, sizeof(right) );

  while (1)
  {
    sleeptime.tv_sec = 1000;
    sleeptime.tv_nsec = 500000000;
    clock_nanosleep( CLOCK_MONOTONIC,0,&sleeptime,NULL);
  }
return 0;

}
