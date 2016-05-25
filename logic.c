#include <signal.h>
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>
#include "motorh/motorhandler.h"
#include "track/track.h"

struct MotorControl *CMotor, *SMotor;
struct led_status *c_track, *s_track;
struct shmid_ds shmid_ds_track;
int shmid, shmid_led;
key_t key;
clock_t start;
int buffer[1000];
int timingBuffer[100], counter=0;
char timingChar[100];

void close_handler();

void change_direction();

int main(int argc, char *argv[])
{
  char a;
  int i;
  signal(SIGINT,close_handler);
  CMotor = (struct MotorControl*)malloc(sizeof(struct MotorControl));
  c_track = (struct led_status*) malloc(sizeof(struct led_status));

  /* ---------- Initializing Shared Memory for MOTOR */
  key = 9001;
  i = 0;
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
        struct timespec sleep;
        if( i == 1000) {
                i = 1;
                buffer[0] = 9999;
        }
        buffer[i] = c_track -> led_one + (c_track -> led_two * 10) + (c_track -> led_three * 100) + (c_track ->$
        i++;
        while (shmctl(shmid_led, SHM_LOCK, &shmid_ds_track) == -1) {
        }
        l_one = c_track -> led_one;
        l_two = c_track -> led_two;
        l_three = c_track -> led_three;
        l_four = c_track -> led_four;
        shmctl(shmid_led, SHM_UNLOCK, &shmid_ds_track);
        sleep.tv_sec = 0;
        sleep.tv_nsec = 15000000;
        CMotor->speed = 20;


        if(l_one && !l_two && !l_three && !l_four) {
                change_direction(1, 0, &sleep);
        }
        else if(l_one && l_two && !l_three && !l_four) {
                change_direction(1, 0, &sleep);
        }
        else if(!l_one && l_two && l_three && !l_four) {
                change_direction(1, 1, &sleep);
        }
        else if(!l_one && !l_two && l_three && !l_four) {
                change_direction(1, 1, &sleep);
        }
        else if(!l_one && l_two && !l_three && !l_four) {
                change_direction(1, 1, &sleep);
        }
        else if(!l_one && !l_two && l_three && l_four) {
                change_direction(0, 1, &sleep);
        }
        else if(!l_one && !l_two && !l_three && l_four) {
                change_direction(0, 1, &sleep);
        }
        else {
                CMotor->changed = 1;
                CMotor->valueleft = 1;
                CMotor->valueright = 1;
                nanosleep(&sleep, NULL);
                CMotor->changed = 1;
                CMotor->valueleft = 0;
                CMotor->valueright = 0;
                nanosleep(&sleep, NULL);

        }
  }
  while(1)
  {
          start = clock();
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
          if(a == 'z')
          {
                  CMotor->changed = 1;
                  CMotor->speed = (CMotor->speed - 10) % 110;
          }
          if(a == 't')
          {
                  CMotor->changed = 1;
                  CMotor->speed = CMotor->speed + 10;
          }
          if(a == 'u')
          {
                  CMotor->changed = 1;
                  CMotor->speed = 100;
          }

          timingBuffer[counter] = clock() - start;
          timingChar[counter] = a;
          counter++;
          counter = counter % 100;
  }
}

void close_handler(int sugnum) {
        FILE *fp;
        fp = fopen("./logic-buffer.log", "w+");
        CMotor->changed = 1;
        CMotor->valueleft=0;
        CMotor->valueright=0;
        fprintf(fp, "TimingLogic - buffer\n");
        for(counter=0; counter < 1000; counter++)
        {
                fprintf(fp, "%d.\t%d\n", counter, buffer[counter]);
        }
        fp = fopen("./logic-timingBuffer.log", "w+");
        fprintf(fp, "TimingLogic - timingBuffer & timingChar\n");
        for(counter=0; counter < 100; counter++)
        {
                fprintf(fp, "%d.\t%d\t%c\n", counter, timingBuffer[counter], timingChar[counter]);
        }
        exit(1);
}

void change_direction(int left, int right, struct timespec *sleep) {
        CMotor->changed = 1;
        CMotor->valueleft = left;
        CMotor->valueright = right;
        nanosleep(sleep, NULL);
}

