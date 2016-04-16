#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "motorhandler.h"

// ------------ Variables
key_t key;
struct MotorControl *CMotor, *SMotor;
int fd_left, fd_right,shmid;

int main()
{
  // ---------- Initializing Variables
  signal(SIGINT, motorbreakhandler);
  fd_right = open("/dev/motor-left", O_WRONLY);
  fd_right = open("/dev/motor-right", O_WRONLY);
  CMotor=(struct MotorControl*)malloc(sizeof(struct MotorControl));

  // ---------- Initializing Shared Memory
  key = 9001;
  if ((shmid = shmget(key, sizeof(struct MotorControl), IPC_CREAT | 0666)) < 0) {
    perror("shmget");
    exit(1);
  }
  if ((SMotor = shmat(shmid, NULL, 0))== (struct MotorControl*) -1) {
    perror("shmat");
    exit(1);
  }
  CMotor = SMotor;
  CMotor->changed = 1;
  CMotor->valueleft = 1;
  CMotor->valueright = 1;
  CMotor->priority = 99;
  CMotor->stop = 0;
  // ---------- Shared Memory Initialized

  // ---------- mainloop -> later to be done with ns sleepcycles
  while(1)
  {
    if(CMotor->stop == 1)
    {
      raise(SIGINT);
    }
    if(CMotor->changed == 0)
    {
      sleep(1);
    }
  }

  return 0;
}

// ------------ Function that changes the values of the motors
void updatemotors(int left, int right)
{
  write( fd_left, &CMotor->valueleft, sizeof(CMotor->valueleft) );
  write( fd_right, &CMotor->valueright, sizeof(CMotor->valueright) );
}

// ------------ handler for the stop signal
void motorbreakhandler(int signum)
{
  updatemotors(0, 0);
  exit(1);
}
