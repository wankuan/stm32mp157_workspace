#ifndef __LED_H
#define __LED_H
#include "bsp.h"

#define LED_G      	PIout(1) 

#define LED_G_PIN                                GPIO_PIN_1
#define LED_G_GPIO_PORT                          GPIOI
#define LED_G_GPIO_CLK_ENABLE()                  __GPIOI_CLK_ENABLE()  
#define LED_G_GPIO_CLK_DISABLE()                 __GPIOI_CLK_DISABLE()  
#define LED_G_ON() { LED_G = 0;}
#define LED_G_OFF() {LED_G = 1;}

void led_init(void);

#endif

