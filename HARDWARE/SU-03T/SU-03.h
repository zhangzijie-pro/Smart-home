#ifndef __SU_03_H
#define __SU_03_H	 

#include "stdint.h"
#include <stdio.h>
#include <stdarg.h>
#include "stm32f4xx.h"


void SU_init(void);
void SU_sendbyte(uint8_t Byte);
void SU_sendArray(uint8_t *Array, uint16_t len);
void SU_sendString(char *String);
void SU_prinf(char *format,...);
void SU_sendCmd(char *String);


extern uint8_t SU_Serial_Buffer[3];
extern uint8_t SU_receive_ok_flag;
extern uint8_t SU_counter;
extern uint8_t SU_data;
#endif
