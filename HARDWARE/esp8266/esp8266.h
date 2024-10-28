#ifndef __esp8266__H
#define __esp8266__H

#include "stm32f4xx.h"
#include "stdint.h"
#include <stdio.h>
#include <stdarg.h>
#include "delay.h"

#define MAX_BUFFER_LEN 3

extern uint8_t esp8266_Serial_Buffer[MAX_BUFFER_LEN];
extern uint8_t esp8266_receive_ok_flag;
extern uint8_t esp8266_counter;
extern uint8_t esp8266_data;


void esp8266_init(void);
void esp8266_sendbyte(uint8_t Byte);
void esp8266_sendArray(uint8_t *Array, uint16_t len);
void esp8266_sendString(char *String);
void esp8266_prinf(char *format,...);
void esp_sendcmd(uint8_t Byte);
void esp_uploaddata(uint8_t *Array, uint16_t len);

#endif
