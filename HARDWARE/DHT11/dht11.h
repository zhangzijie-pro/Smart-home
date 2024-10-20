#ifndef __DTH11_H
#define __DTH11_H
#include "stm32f4xx.h"
#include "delay.h"


int32_t dht11_read(uint8_t *pbuf);
void dht11_init(void);
#endif /* __DS18B20_H */
