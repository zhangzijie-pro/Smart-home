#ifndef __pwm_h__
#define __pwm_h__
#include "sys.h"
#include "stm32f4xx.h"

int PWM_init(void);
void PWM_set_compare(int temp);

void PWM_init_servo(void);
void PWM_set_up(int value);
void PWM_set_down(int value);
void PWM_set_door(int value);
#endif
