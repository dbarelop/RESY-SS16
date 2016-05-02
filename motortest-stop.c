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
struct MotorControl *CMotor, *SMotor;

int main()
{
  int shmid;
  key_t key;
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

  CMotor->stop = 1;

  return 0;
}