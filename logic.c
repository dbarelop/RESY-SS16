#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include "motorhandler.h"
#include "track.h"

struct MotorControl *CMotor, *SMotor;
struct led_status *c_track, *s_track;
struct shmid_ds shmid_ds_track;
int shmid, shmid_led;
key_t key;

int main(int argc, char *argv[])
{
  char a;
  CMotor = (struct MotorControl*)malloc(sizeof(struct MotorControl));
  c_track = (struct led_status*) malloc(sizeof(struct led_status));


  /* ---------- Initializing Shared Memory for MOTOR */
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

  /* ---------- Initializing Shared Memory for TRACKING LEDS */
  key = 9002;
  if ((shmid_led = shmget(key, sizeof(struct led_status), IPC_CREAT | 0666)) < 0) {
    perror("shmget");
    exit(1);
  }
  if ((s_track = shmat(shmid_led, NULL, 0)) == (struct led_status*) -1) {
    perror("shmat");
    exit(1);
  }
  c_track = s_track;

  while(argc > 1) {
	int l_one, l_two, l_three, l_four;
	printf("%d  %d  %d  %d\n",
	 c_track -> led_one, c_track -> led_two, c_track -> led_three, c_track -> led_four);
	while (shmctl(shmid_led, SHM_LOCK, &shmid_ds_track) == -1) {
	}
	l_one = c_track -> led_one;
	l_two = c_track -> led_two;
	l_three = c_track -> led_three;
	l_four = c_track -> led_four;
	shmctl(shmid_led, SHM_UNLOCK, &shmid_ds_track);
	
	if(l_one && !l_two && !l_three && !l_four) {
		CMotor->changed = 1;
		CMotor->valueleft = 1;
		CMotor->valueright = 0;
		usleep(50);
		CMotor->changed = 1;
		CMotor->valueleft = 0;
		CMotor->valueright = 0;
	}
	else if(l_one && l_two && !l_three && !l_four) {
		CMotor->changed = 1;
		CMotor->valueleft = 1;
		CMotor->valueright = 0;
		usleep(50);
		CMotor->changed = 1;
		CMotor->valueleft = 0;
		CMotor->valueright = 0;
	}
	else if(!l_one && l_two && l_three && !l_four) {
		CMotor->changed = 1;
		CMotor->valueleft = 1;
		CMotor->valueright = 1;
		usleep(50);
		CMotor->changed = 1;
		CMotor->valueleft = 0;
		CMotor->valueright = 0;
	}
	else if(!l_one && !l_two && l_three && l_four) {
		CMotor->changed = 1;
		CMotor->valueleft = 0;
		CMotor->valueright = 1;
		usleep(50);
		CMotor->changed = 1;
		CMotor->valueleft = 0;
		CMotor->valueright = 0;
	}
	else if(!l_one && !l_two && !l_three && l_four) {
		CMotor->changed = 1;
		CMotor->valueleft = 0;
		CMotor->valueright = 1;
		usleep(50);
		CMotor->changed = 1;
		CMotor->valueleft = 0;
		CMotor->valueright = 0;
	}
  }
  while(1)
  {
	  scanf("%c", &a);
	  printf("%c\n", a);
	  if(a == 'l')
	  {
		  CMotor->changed = 1;
		  CMotor->valueleft=1;
		  CMotor->valueright=0;
	  }
	  if(a == 'r')
	  {
		  CMotor->changed = 1;
		  CMotor->valueleft=0;
		  CMotor->valueright=1;
	  }
	  if(a == 'f')
	  {
		  CMotor->changed = 1;
		  CMotor->valueleft=1;
		  CMotor->valueright=1;
	  }
	  if(a == 'b')
	  {
		  CMotor->changed = 1;
		  CMotor->valueleft=-1;
		  CMotor->valueright=-1;
	  }
	  if(a == 's')
	  {
		  CMotor->changed = 1;
		  CMotor->valueleft=0;
		  CMotor->valueright=0;
	  }
  }
}

