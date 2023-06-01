#ifndef __BSP_ME_H
#define __BSP_ME_H

#include "stm32f10x.h"
#include "common.h"
#include <stdio.h>
#include <stdbool.h>
#include "os_system__typedef.h"
#include "event_groups.h"

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

/* 定义事件位的意义 */
#define TCP_TASK_BIT (1UL << 1UL)
// #define mainSECOND_TASK_BIT (1UL << 1UL)
// #define mainISR_BIT (1UL << 2UL)

typedef struct // ��������֡�Ĵ����ṹ��
{
	char Data_RX_BUF[RX2_4G_BUF_MAX_LEN];

	struct
	{
		__IO uint16_t FramLength : 15;	  // 14:0
		__IO uint16_t FramFinishFlag : 1; // 15

	} InfBit;

} USART_4G_Fram;

typedef enum
{
	ME_SEND_DEFAULT,
	ME_CLOSE_CALL,
	ME_SEND_DEBUG,
} ME_STEP;

typedef struct
{
	__IO u8 tcp_connection_status;
	thread_cslock_t lock;
	thread_cslock_t cnt_lock;
	EventGroupHandle_t tcp_group_handle;
	u8 imei_id[20];
	u8 send_data[1024];
	u16 send_cnt;
	ME_STEP send_step;
	void (*me_proc)(void);
	void (*me3630_send)(void);
	void (*set_send_cnt)(u16);
	void (*tcp_task)(void);

} Drv_Me_t, *Drv_Me_pt;

extern Drv_Me_pt drv_me_pt;
void init_me(void);
#endif
