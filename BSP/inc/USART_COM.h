#ifndef __USART_COM_H__
#define __USART_COM_H__

#include "bsp.h"
//The maximum length of RX buffer
#define UART_RX_MAX_LEN		100

#define RX_CPLE				0x8000
#define RX_ENTER			0X4000
#define RX_BUF_LEN_MASK		0X3FFF

typedef struct{
	uint8_t buffer[UART_RX_MAX_LEN];
	//bit15  finish status get 0x0A-\n
	//bit14  get 0x0D -\r
	//bit13-bit0  length of the
	uint16_t status;
}UART_RXD_TypeDef;

#define UART_COM USART1
#define UART_BAUD 115200
#define UART_RXD_BUF_LEN 256
#define UART_GPIO_AF GPIO_AF7_USART1
#define UART_RX_PIN GPIO_PIN_7
#define UART_TX_PIN GPIO_PIN_6
#define UART_PORT GPIOB
#define UART_PORTB_RCC_ENABLE()  __HAL_RCC_GPIOB_CLK_ENABLE()
#define UART_COM_RCC_ENABLE() __HAL_RCC_USART1_CLK_ENABLE()

#define UART_NVIC_ENABLE(){\
HAL_NVIC_EnableIRQ(USART1_IRQn);\
HAL_NVIC_SetPriority(USART1_IRQn,0,1);}

extern UART_HandleTypeDef uart_com_t;
extern UART_RXD_TypeDef uart_rx_t;


void uart_init(void);
void UART_COM_CPLT_Callback(UART_RXD_TypeDef *uart);

#endif 
