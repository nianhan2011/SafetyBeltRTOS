#include "usart3.h"
#include "common.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "delay.h"

static void USART_GPIO_Config(void);
static void USART_USART_Config(void);
static void USART_4G_NVIC_Configuration(void);

// Queqe256 usart3_fram;
USART_Fram usart3_fram;

/**
 * @brief  初始化函数
 * @param  无
 * @retval 无
 */
u8 test333[10] = {0x01, 0x01, 0, 0, 0, 0x18, 0x3C, 0x00};

void clearFrame(void)
{
    usart3_fram.InfBit.FramLength = 0;
    // usart3_fram.InfBit.ReadLength = 0;
    // usart3_fram.InfBit.FramFinishFlag = 0;
//    memset((char *)usart3_fram.Data_RX_BUF, 0, strlen((char *)usart3_fram.Data_RX_BUF));

    // memcpy(usart3_fram.Data_RX_BUF, test333, 8);
    // usart3_fram.InfBit.FramLength = 8;
}

void usart3_init(void)
{
    // usart3_fram.is_interrupt = ENABLE;
    // QUEUE_INIT(usart3_fram);
    clearFrame();
    USART_GPIO_Config();
    USART_USART_Config();
}

/**
 * @brief  初始化usart用到的GPIO引脚
 * @param  无
 * @retval 无
 */
static void USART_GPIO_Config(void)
{
    RCC_USART_GPIO_ClockCmd(RCC_USART3_GPIO_Periph, ENABLE);

    RCC_USART_GPIO_ClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;

    /* USART GPIO config */
    /* Configure USART Tx as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = PIN_USART3_TX;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(PORT_USART3_TX, &GPIO_InitStructure);

    /* Configure USART Rx as input floating */
    GPIO_InitStructure.GPIO_Pin = PIN_USART3_RX;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(PORT_USART3_RX, &GPIO_InitStructure);
}

static void USART_4G_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Enable the USART_3 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART_3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
/**
 * @brief  初始化ESP8266用到的 USART
 * @param  无
 * @retval 无
 */
static void USART_USART_Config(void)
{

    /* config USART clock */
    RCC_USART3_ClockCmd(RCC_USART3_Periph, ENABLE);

    USART_InitTypeDef USART_InitStructure;

    /* USART1 mode config */
    USART_InitStructure.USART_BaudRate = USART_BAUD_RATE;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART_3, &USART_InitStructure);

    /* 中断配置 */
    USART_ITConfig(USART_3, USART_IT_RXNE, ENABLE); // 使能串口接收中断
    USART_ITConfig(USART_3, USART_IT_IDLE, ENABLE); // 使能串口总线空闲中断

    USART_4G_NVIC_Configuration();

    USART_Cmd(USART_3, ENABLE);
}

void USART_3_IRQnHandler(void)
{
    uint8_t ucCh;

    if (USART_GetITStatus(USART_3, USART_IT_RXNE) != RESET)
    {
        ucCh = USART_ReceiveData(USART_3);

        // QUEUE_IN(usart3_fram, &ucCh, 1);

        if (usart3_fram.InfBit.FramLength < (RX_BUF_MAX_LEN - 2)) // 预留1个字节写结束符
            usart3_fram.Data_RX_BUF[usart3_fram.InfBit.FramLength++] = ucCh;
    }

    if (USART_GetITStatus(USART_3, USART_IT_IDLE) == SET) // 数据帧接收完毕
    {
        usart3_fram.InfBit.FramFinishFlag = 1;

        ucCh = USART_ReceiveData(USART_3);
    }
}
static void usart_send_byte(USART_TypeDef *pUSARTx, uint8_t data)
{
    USART_SendData(pUSARTx, data);
    while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET)
        ;
}

void usart3_send_array(uint8_t *array, uint8_t num)
{
    uint8_t i;
    for (i = 0; i < num; i++)
    {
        usart_send_byte(USART_3, array[i]);
    }
    while (USART_GetFlagStatus(USART_3, USART_FLAG_TC) == RESET)
        ;
}
