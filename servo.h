#ifndef _SERVO_H
#define _SERVO_H

void servo_init();

// pos must be a value from 0 to 180
void servo_set(float pos);

void servo_calibrate();

extern int pos0;
extern int pos180;

#endif
