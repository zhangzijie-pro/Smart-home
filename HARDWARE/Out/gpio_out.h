#ifndef __gpio_out_H
#define __gpio_out_H
#include "sys.h"

#define WET_OUT PEout(5)
#define FAN_OUT PEout(6)

#define TEMP_UP PFout(0)
#define TEMP_Down PFout(1)

#define LED1 PEout(0)
#define LED2 PEout(1)
#define LED3 PEout(2)
#define LED4 PEout(3)
#define LED5 PEout(4)

#define WC_FAN PFout(2)
#define ROLE_DOWN PCout(8)

void gpio_out_init(void);
#endif
