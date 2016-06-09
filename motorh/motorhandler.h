#ifndef MOTORHANDLER_H_
#define MOTORHANDLER_H_

// ------------ handler for the stop function
void motorbreakhandler(int);

// ------------ Motor Control Structure
// ------------ valueleft & valueright can be set to -1, 0 and 1 (starts with 1)
// ------------ priority is for your process priority (starts with 99)
// ------------ stop if stop is set, motors stop and program exits
struct MotorControl {
  int changed;
  int valueleft;
  int valueright;
  int speed;
  int stop;
} motorcontrol;

#endif
