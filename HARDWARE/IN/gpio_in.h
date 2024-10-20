#ifndef __gpio_in_H
#define __gpio_in_H
#include "sys.h"

// 人体感应
#define HUMAN_EXIST GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9)

void Human_Init(void);

// 雨滴检测  0有雨   1没有
#define RAIN_EXIST	GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_8)
void Rain_Init(void);

// 火焰检测
#define FIRE_EXIST	GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7)
void fire_Init(void);

#endif
