#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>
#include "motorhandler.h"

// ------------ Variables
struct MotorControl *CMotor, *SMotor;

int main()
{
  int shmid;
  key_t key;
  struct timespec sleeptime;
  key = 9001;
  if ((shmid = shmget(key, sizeof(struct MotorControl), 0666)) < 0) {
    perror("shmget");
    exit(1);
  }
  if ((SMotor = shmat(shmid, NULL, 0))== (struct MotorControl *) -1) {
    perror("shmat");
    exit(1);
  }
  CMotor = SMotor;

  CMotor->valueleft = 1;
  CMotor->changed = 1;
  sleeptime.tv_sec = 1;
  sleeptime.tv_nsec = 500000000;
  clock_nanosleep( CLOCK_MONOTONIC,0,&sleeptime,NULL);
  
  CMotor->valueleft = -1;
  CMotor->changed = 1;
  sleeptime.tv_sec = 1;
  sleeptime.tv_nsec = 500000000;
  clock_nanosleep( CLOCK_MONOTONIC,0,&sleeptime,NULL);
  
  CMotor->valueleft = 0;
  CMotor->changed = 1;
  sleeptime.tv_sec = 1;
  sleeptime.tv_nsec = 500000000;
  clock_nanosleep( CLOCK_MONOTONIC,0,&sleeptime,NULL);

  return 0;
}