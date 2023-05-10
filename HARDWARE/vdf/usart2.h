#ifndef __BSP_USART2_H
#define __BSP_USART2_H

#include "stm32f10x.h"
#include "common.h"
#include <stdio.h>
#include <stdbool.h>
#include "FreeRTOS.h"
#include "delay.h"
#include "task.h"

#if defined(__CC_ARM)
#pragma anon_unions
#endif

// #define RCC_USART_GPIO_ClockCmd RCC_APB2PeriphClockCmd
// #define RCC_USART3_GPIO_Periph RCC_APB2Periph_GPIOB
// #define PORT_USART_TX GPIOB
// #define PIN_USART3_TX GPIO_Pin_10
// #define PORT_USART3_RX GPIOB
// #define PIN_USART3_RX GPIO_Pin_11

// #define RCC_USART3_ClockCmd RCC_APB1PeriphClockCmd
// #define RCC_USART3_Periph RCC_APB1Periph_USART3

// #define USART_3 USART3

// #define USART_BAUD_RATE 9600

// #define USART_3_IRQn USART3_IRQn
// #define USART_3_IRQnHandler USART3_IRQHandler

// #define USART_3_PRINT(fmt, ...) USART_printf(USART_3, fmt, ##__VA_ARGS__)

#define RCC_USART_GPIO_ClockCmd RCC_APB2PeriphClockCmd
#define RCC_USART2_GPIO_Periph RCC_APB2Periph_GPIOA
#define PORT_USART2_TX GPIOA
#define PIN_USART2_TX GPIO_Pin_2
#define PORT_USART2_RX GPIOA
#define PIN_USART2_RX GPIO_Pin_3

#define RCC_USART2_ClockCmd RCC_APB1PeriphClockCmd
#define RCC_USART2_Periph RCC_APB1Periph_USART2

#define USART_2 USART2

#define USART_BAUD_RATE 9600

#define USART_2_IRQn USART2_IRQn
#define USART_2_IRQnHandler USART2_IRQHandler

#define USART_2_PRINT(fmt, ...) USART_printf(USART_2, fmt, ##__VA_ARGS__)

extern USART_Fram usart2_fram;
void usart2_init(void);
void clear_usart2_frame(void);
void usart2_send_array(uint8_t *array, uint8_t num);

static void RS485_Delay(__IO uint32_t nCount) // ¼òµ¥µÄÑÓÊ±º¯Êý
{
    for (; nCount != 0; nCount--)
        ;
}

#define RS485_RX_ENABLE                \
    vTaskDelay(2);                     \
    GPIO_ResetBits(GPIOD, GPIO_Pin_2); \
    vTaskDelay(2);
#define RS485_TX_ENABLE              \
    GPIO_SetBits(GPIOD, GPIO_Pin_2); \
    vTaskDelay(2);
#endif
