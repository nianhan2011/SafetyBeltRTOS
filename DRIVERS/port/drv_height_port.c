#include "drv_height_port.h"
#include "common.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

static void USART5_GPIO_Config(void);
static void USART5_USART_Config(void);
static void USART5_NVIC_Configuration(void);

QueueHandle_t drv_height_queue;

/**
 * @brief  初始化函数
 * @param  无
 * @retval 无
 */
void USART5_Init(void)
{
	//	USART5_GPIO_Config ();
	drv_height_queue = xQueueCreate(40, sizeof(u8));

	USART5_USART_Config();
}

/**
 * @brief  初始化usart用到的GPIO引脚
 * @param  无
 * @retval 无
 */
static void USART5_GPIO_Config(void)
{

	//	GPIO_InitTypeDef GPIO_InitStructure;

	//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	//	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//	GPIO_SetBits(GPIOA,GPIO_Pin_7);
}

static void USART_NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Enable the USART2 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
/**
 * @brief  初始化ESP8266用到的 USART
 * @param  无
 * @retval 无
 */
static void USART5_USART_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	/* config USART clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	/* USART GPIO config */
	/* Configure USART Tx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* Configure USART Rx as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	/* USART1 mode config */
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(UART5, &USART_InitStructure);

	/* 中断配置 */
	USART_ITConfig(UART5, USART_IT_RXNE, ENABLE); // 使能串口接收中断
	USART_ITConfig(UART5, USART_IT_IDLE, ENABLE); // 使能串口总线空闲中断

	USART_NVIC_Configuration();

	USART_Cmd(UART5, ENABLE);
}

void UART5_IRQHandler(void)
{
	uint8_t ucCh;

	if (USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)
	{
		ucCh = USART_ReceiveData(UART5);

		xQueueSendFromISR(drv_height_queue, &ucCh, NULL);
		// if (ucCh == 0x0A)

		// {
		// 	Baro_Height_Fram.InfBit.FramLength = 0;
		// 	Baro_Height_Fram.Data_RX_BUF[Baro_Height_Fram.InfBit.FramLength] = '\0';
		// }

		// Baro_Height_Fram.Data_RX_BUF[Baro_Height_Fram.InfBit.FramLength++] = ucCh;
	}

	if (USART_GetITStatus(UART5, USART_IT_IDLE) == SET) // 数据帧接收完毕
	{
		// Baro_Height_Fram.InfBit.FramFinishFlag = 1;

		ucCh = USART_ReceiveData(UART5);
	}
}

static void usart_send_byte(USART_TypeDef *pUSARTx, uint8_t data)
{
	USART_SendData(pUSARTx, data);
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET)
		;
}

void usart5_send_array(uint8_t *array, uint8_t num)
{
	uint8_t i;
	for (i = 0; i < num; i++)
	{
		usart_send_byte(UART5, array[i]);
	}
	while (USART_GetFlagStatus(UART5, USART_FLAG_TC) == RESET)
		;
}
