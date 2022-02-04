#include "LED.h"

void led_init(void)
{
	GPIO_InitTypeDef gpio_t;
	 
	__GPIOI_CLK_ENABLE();
	gpio_t.Pin = LED_G_PIN;
	gpio_t.Mode = GPIO_MODE_OUTPUT_PP;
	gpio_t.Pull = GPIO_NOPULL;
	gpio_t.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(LED_G_GPIO_PORT,&gpio_t);
	LED_G_OFF();
}

