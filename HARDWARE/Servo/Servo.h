#ifndef __SERVO_H
#define __SERVO_H	

#include "pwm.h"

void Servo_Init(void);
void Servo_Angle_up(float Angle);
void Servo_Angle_down(float Angle);
void Servo_Angle_door(float Angle);
#endif
