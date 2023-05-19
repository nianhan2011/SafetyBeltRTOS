#ifndef __BSP_ME_H
#define __BSP_ME_H

#include "stm32f10x.h"
#include "common.h"
#include <stdio.h>
#include <stdbool.h>

#if defined(__CC_ARM)
#pragma anon_unions
#endif

/******************************** 4G �������Ŷ��� ***********************************/
#define RCC_4G_GPIO_ClockCmd RCC_APB2PeriphClockCmd
#define RCC_4G_GPIO_Periph RCC_APB2Periph_GPIOA
#define PORT_4G_TX GPIOA
#define PIN_4G_TX GPIO_Pin_2
#define PORT_4G_RX GPIOA
#define PIN_4G_RX GPIO_Pin_3

#define RCC_4G_USART_ClockCmd RCC_APB1PeriphClockCmd
#define RCC_4G_USART_Periph RCC_APB1Periph_USART2

#define USART_4G USART2

#define USART_BAUD_RATE_4G 115200

#define USART_4G_IRQn USART2_IRQn
#define USART_4G_IRQnHandler USART2_IRQHandler

#define WIFI_Usart(fmt, ...) USART_printf(USART_4G, fmt, ##__VA_ARGS__)

#define RX2_4G_BUF_MAX_LEN 100 // �����ջ����ֽ���

typedef struct // ��������֡�Ĵ����ṹ��
{
	char Data_RX_BUF[RX2_4G_BUF_MAX_LEN];

	struct
	{
		__IO uint16_t FramLength : 15;	  // 14:0
		__IO uint16_t FramFinishFlag : 1; // 15

	} InfBit;

} USART_4G_Fram;

// typedef struct                                   //��������֡�Ĵ����ṹ��
//{
//	int Pressure_Data;
//
//    	int Height_Data;

//
//} Struct_Barometric_Data;

extern USART_4G_Fram USART_4G_Fram_Instance;
extern _Bool TCP_Is_Connect;
void USART_4G_Init(void);

void Start_4G_TCP(void);

bool ME_GetIMEI(void);

bool GPRS_Connect(void);
bool Open_Call(void);

bool TCP_Connect(void);
void TCP_Send(char *message);
void Start_ME(void);
void ReceiveString(void);
#endif
