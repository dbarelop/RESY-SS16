#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "motorhandler.h"

// ------------ Variables
key_t key;
struct MotorControl *CMotor, *SMotor;
int fd_left, fd_right, shmid;
struct timespec sleeptime, pwmtime;
clock_t start, diff;
int timingBuffer[100], counter=0, speed;
char timingChar[100];

int main()
{
  // ---------- Initializing Variables
  signal(SIGINT, motorbreakhandler);
  fd_right = open("/dev/motor-left", O_WRONLY);
  fd_left = open("/dev/motor-right", O_WRONLY);
  if (fd_left<0 || fd_right<0)
  {
    perror("drivers not )");
    return -1;
  }
  CMotor=(struct MotorControl*)malloc(sizeof(struct MotorControl));
  int right, left;
  // ---------- Initializing Shared Memory
  key = 9001;
  if ((shmid = shmget(key, sizeof(struct MotorControl), IPC_CREAT | 0666)) < 0)
  {
    perror("shmget");
    exit(1);
  }
  if ((SMotor = shmat(shmid, NULL, 0))== (struct MotorControl*) -1)
  {
    perror("shmat");
    exit(1);
  }
  CMotor = SMotor;
  CMotor->changed = 1;
  CMotor->valueleft = 0;
  CMotor->valueright = 0;
  CMotor->speed = 100;
  CMotor->stop = 0;
  // ---------- Shared Memory Initialized

  // ---------- mainloop -> later to be done with ns sleepcycles
  sleeptime.tv_nsec = 10000000;
  while(1)
  {
    start = clock();
    if(CMotor->stop == 1)
    {
      raise(SIGINT);
    }
    if(CMotor->changed == 0)
    {
      if(speed < 100)
      {
        updatemotors(0,0);
        pwmtime.tv_nsec = (100 - CMotor->speed) * 100000;
        clock_nanosleep( CLOCK_MONOTONIC,0,&pwmtime,NULL );
        updatemotors(CMotor->valueleft, CMotor->valueright);
      }
      clock_nanosleep( CLOCK_MONOTONIC,0,&sleeptime,NULL);
      timingBuffer[counter] = clock() - start;
      timingChar[counter] = 'x';
    }
    else
    {
      left = CMotor->valueleft;
      right = CMotor->valueright;
      speed = CMotor->speed;
      start = clock();
      if(CMotor->stop == 1)
      {
        raise(SIGINT);
      }
      if(CMotor->changed == 0)
      {
        if(speed < 100)
        {
          updatemotors(0,0);
          pwmtime.tv_nsec = (100 - CMotor->speed) * 100000;
          clock_nanosleep( CLOCK_MONOTONIC,0,&pwmtime,NULL );
          updatemotors(CMotor->valueleft, CMotor->valueright);
        }
        clock_nanosleep( CLOCK_MONOTONIC,0,&sleeptime,NULL);
        timingBuffer[counter] = clock() - start;
        timingChar[counter] = 'x';
      }
      else
      {
        left = CMotor->valueleft;
        right = CMotor->valueright;
        speed = CMotor->speed;
        updatemotors(left, right);
        CMotor->changed = 0;
        timingBuffer[counter] = clock()-start;
        if(left == -1 && right == -1)
           timingChar[counter] = 'b';
        else if(left == 0 && right == 0)
           timingChar[counter] = 's';
        else if(left == 1 && right == 0)
           timingChar[counter] = 'l';
        else if(left == 0 && right == 1)
           timingChar[counter] = 'r';
        else if(left == 1 && right == 1)
           timingChar[counter] = 'f';
        else timingChar[counter] = 'x';
      }
      counter++;
      counter = counter % 100;
    }
  }

  return 0;
}
// ------------ Function that changes the values of the motors
void updatemotors(int left, int right)
{
  write( fd_left, &left, sizeof(left) );
  write( fd_right, &right, sizeof(right) );
}

// ------------ handler for the stop signal
void motorbreakhandler(int signum)
{
  FILE *fp;
  fp = fopen("/motorhandler.log", "w+");
  updatemotors(0, 0);
  timingBuffer[counter] = clock()-start;
  timingChar[counter] = 'R';
  for(counter = 0; counter < 100; counter++)
  {
    fprintf(fp, "%d\t%c\n", timingBuffer[counter], timingChar[counter]);
    //write to file
  }
  exit(1);
};
