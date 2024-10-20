#ifndef __LED_H
#define __LED_H
#include "sys.h"

#define LED1 PFout(9)	  // D1
#define LED2 PFout(10)	// D2

void LED_Init(void);	 				    
#endif
