//����DMA�ʼ�
/**
*@brief : ʹ��DMA���д��ڽ���
1.���ڳ�ʼ������
2.DMA��ʼ�����ã�ÿ��DMA��ʼ������ǰ��Ҫ���з���ʼ��Deinit��
3.__HAL_LINKDMA(&USART_Handler,hdmarx,USARTRxDMA_Handler);
4.HAL_UART_Receive_DMA(&UART1_Handler,RX_BUF,Uart_size);�򿪴���DMA����
5.HAL_DMA_IRQHandler(&USARTRxDMA_Handler);	//����HAL���жϴ����ú���
6.void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)

__HAL_DMA_CLEAR_FLAG(&UART1TxDMA_Handler,DMA_FLAG_TCIF3_7);//���DMA2_Steam7������ɱ�־
HAL_UART_DMAStop(&UART1_Handler);      //��������Ժ�رմ���DMA

**/
#include "USART_COM.h"
#include "ringbuffer.h"

//length of HAL_UART buffer
#define HAL_RX_BUF_SIZE 1
#define max_delay 1000

uint8_t HAL_RX_BUF[HAL_RX_BUF_SIZE];

UART_HandleTypeDef uart_com_t;
UART_RXD_TypeDef uart_rx_t;



void uart_init(void)
{
	uart_com_t.Instance = UART_COM;
	uart_com_t.Init.BaudRate = UART_BAUD;
	uart_com_t.Init.WordLength = UART_WORDLENGTH_8B;
	uart_com_t.Init.StopBits = UART_STOPBITS_1;
	uart_com_t.Init.Parity = UART_PARITY_NONE;
	uart_com_t.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	uart_com_t.Init.Mode = UART_MODE_TX_RX;
	HAL_UART_Init(&uart_com_t);
	HAL_UART_Receive_IT(&uart_com_t,(uint8_t*)HAL_RX_BUF,HAL_RX_BUF_SIZE);

}

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
	GPIO_InitTypeDef gpio_t;
	if(huart->Instance == UART_COM)
	{
		UART_PORTB_RCC_ENABLE();
		UART_COM_RCC_ENABLE();

		gpio_t.Pin = UART_RX_PIN|UART_TX_PIN;
		gpio_t.Mode = GPIO_MODE_AF_PP;
		gpio_t.Pull = GPIO_PULLUP;
		gpio_t.Speed = GPIO_SPEED_FAST;
		gpio_t.Alternate = UART_GPIO_AF;
		HAL_GPIO_Init(UART_PORT,&gpio_t);

		HAL_NVIC_EnableIRQ(USART1_IRQn);
		HAL_NVIC_SetPriority(USART1_IRQn,0,1);
	}
}

DECLARE_RB(rb_usart,UART_RXD_BUF_LEN);

int __io_putchar(int ch)
{
	UART_COM->DR = (u8) ch;
	while((UART_COM->SR & USART_FLAG_TXE) == RESET);
	return ch;
}

int __io_getchar(void)
{
	signed char ch;
	while(rbempty(&rb_usart)) continue;
	ch=rbget(&rb_usart);
	switch(ch)
	{
		case '\r':ch='\n';return __io_putchar(ch);
		case 0x1b:ch=' '; return __io_putchar(ch);
		case 0x08:/* backspace in minicom or putty */
		case 0x7F:
			/* earse char in screen(replace with space
			 and return the backspace code */
			__io_putchar(ch); __io_putchar(' ');__io_putchar(ch);
			return ch;
		default: return __io_putchar(ch);
	}

	/* echo it */
	return __io_putchar(ch);
}

/* USART interrupt handler */
//remember the UART_COM receive effective when input have a ending with "\r\n"
uint8_t temp;
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == UART_COM)
	{
		temp = HAL_RX_BUF[0];
		if((uart_rx_t.status&RX_CPLE) == 0)
		{
			if(uart_rx_t.status&RX_ENTER)
			{
				if(temp != 0x0A) uart_rx_t.status = 0;//receive error, restart receive
				else uart_rx_t.status |= RX_CPLE;	//receive complete
			}
			else
			{
				if(temp == 0x0D) uart_rx_t.status |= RX_ENTER;//wait new line ACSII 0x0
				else //get each char into buffer
				{
					uart_rx_t.buffer[uart_rx_t.status&RX_BUF_LEN_MASK] = temp;
					uart_rx_t.status++;
					// if the receive buffer overflow, receive complete
					if((uart_rx_t.status&RX_BUF_LEN_MASK) > (UART_RX_MAX_LEN-1))
						uart_rx_t.status |= RX_CPLE;
				}
			}
		}
	}
}

void USART1_IRQHandler(void)
{

	HAL_UART_IRQHandler(&uart_com_t);

	uint8_t timeout=0;
	while (HAL_UART_GetState(&uart_com_t) != HAL_UART_STATE_READY)
	{
		timeout++;
		if(timeout>max_delay)break;
	}

	timeout=0;
	while(HAL_UART_Receive_IT(&uart_com_t,(uint8_t*)HAL_RX_BUF,HAL_RX_BUF_SIZE) != HAL_OK)
	{
		timeout++;
		if(timeout>max_delay) break;
	}

	if(uart_rx_t.status & RX_CPLE)//whole char is complete receive
	{
		UART_COM_CPLT_Callback(&uart_rx_t);
		uart_rx_t.status = 0;
	}
}
__weak void UART_COM_CPLT_Callback(UART_RXD_TypeDef *uart)
{

}


