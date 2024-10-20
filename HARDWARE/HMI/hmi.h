#ifndef __hmi_h__
#define __hmi_h__

#include "stdint.h"
#include <stdio.h>
#include <stdarg.h>
#include "stm32f4xx.h"


extern uint8_t HMI_Serial_Buffer[2];
extern uint8_t HMI_receive_ok_flag;
extern uint8_t HMI_counter;
//extern uint8_t HMI_data;

void HMI_init(void);
void HMI_sendbyte(uint8_t Byte);
void HMI_sendArray(const uint8_t *Array, uint16_t len);
void HMI_sendString(char *String);
void HMI_prinf(char *format,...);

#endif
