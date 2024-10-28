#ifndef __gpio_out_H
#define __gpio_out_H
#include "sys.h"

#define WET_OUT PEout(6)
#define FAN_OUT PEout(5)

#define TEMP_UP PEout(4)
#define TEMP_Down PFout(0)

#define LED1 PEout(0)
#define LED2 PEout(1)
#define LED3 PEout(2)
#define LED4 PEout(3)
#define LED5 PEout(7)

#define Light_road PGout(5)
#define WC_FAN PGout(4)

#define ROLE_DOWN PCout(11)
#define FIRE_SIGNAL PCout(10)


#define RED PCout(1)
#define GREEN PCout(2)
#define BLUE PCout(3)
void gpio_out_init(void);
#endif
