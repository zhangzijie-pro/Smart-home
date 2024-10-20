#ifndef __wt558d_H
#define __wt558d_H

#include "stm32f4xx.h"
#include "delay.h"

#define WT588D_RCC RCC_AHB1Periph_GPIOC
#define WT588D_PORT GPIOC
#define WT588D_CS_Pin 	GPIO_Pin_5
#define WT588D_SCL_Pin	GPIO_Pin_6
#define WT588D_SDA_Pin	GPIO_Pin_7

#define CS_BEGNI GPIO_ResetBits(WT588D_PORT,WT588D_CS_Pin)
#define CS_END GPIO_SetBits(WT588D_PORT,WT588D_CS_Pin)

#define SCL_BEGNI GPIO_ResetBits(WT588D_PORT,WT588D_SCL_Pin)
#define SCL_END GPIO_SetBits(WT588D_PORT,WT588D_SCL_Pin)

#define SDA_BEGNI GPIO_ResetBits(WT588D_PORT,WT588D_SDA_Pin)
#define SDA_END GPIO_SetBits(WT588D_PORT,WT588D_SDA_Pin)

// 音量调节
#define VOICE_0 E0H
#define VOICE_1 E1H
#define VOICE_2 E2H
#define VOICE_3 E3H
#define VOICE_4 E4H
#define VOICE_5 E5H
#define VOICE_6 E6H
#define VOICE_7 E7H
// 循环播放
#define CIRCULA_PLAY F2H

#define STOP_PLAY FEH


#endif
