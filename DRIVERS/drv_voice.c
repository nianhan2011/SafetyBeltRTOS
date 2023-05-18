#include "drv_voice.h"
#include "common.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "os_system__typedef.h"
#include "drv_voice.h"
static void USART4_GPIO_Config(void);
static void USART4_USART_Config(void);
static void USART4_NVIC_Configuration(void);
u8 vol[5] = {0xAA, 0x13, 0x01, 0x1E, 0xDC};
u8 vol_0[5] = {0xAA, 0x13, 0x01, 0x01, 0xBf};
u8 mode_single_stop[6] = {0xAA, 0x18, 0x01, 0x02, 0xC5};
u8 set_14[6] = {0xAA, 0x07, 0x02, 0x00, 0x0E, 0xC1};
u8 set_13[6] = {0xAA, 0x07, 0x02, 0x00, 0x0d, 0xC0};
u8 set_12[6] = {0xAA, 0x07, 0x02, 0x00, 0x0c, 0xBF};
u8 set_11[6] = {0xAA, 0x07, 0x02, 0x00, 0x0b, 0xBe};
u8 set_10[6] = {0xAA, 0x07, 0x02, 0x00, 0x0A, 0xBD};
u8 set_09[6] = {0xAA, 0x07, 0x02, 0x00, 0x09, 0xBC};
u8 set_08[6] = {0xAA, 0x07, 0x02, 0x00, 0x08, 0xBB};
u8 set_07[6] = {0xAA, 0x07, 0x02, 0x00, 0x07, 0xBA};
u8 set_06[6] = {0xAA, 0x07, 0x02, 0x00, 0x06, 0xB9};
u8 set_05[6] = {0xAA, 0x07, 0x02, 0x00, 0x05, 0xB8};
u8 set_04[6] = {0xAA, 0x07, 0x02, 0x00, 0x04, 0xB7};
u8 set_03[6] = {0xAA, 0x07, 0x02, 0x00, 0x03, 0xB6};
u8 set_02[6] = {0xAA, 0x07, 0x02, 0x00, 0x02, 0xB5};
u8 mode_single_tune_circulation[5] = {0xAA, 0x18, 0x01, 0x01, 0xC4};
u8 stop[4] = {0xAA, 0x04, 0x00, 0xAE};
u8 set_01[6] = {0xAA, 0x07, 0x02, 0x00, 0x01, 0xB4};

static QueueHandle_t drv_height_queue;

/**
 * @brief  初始化函数
 * @param  无
 * @retval 无
 */
static void USART4_Init(void)
{
    //	USART4_GPIO_Config ();
    drv_height_queue = xQueueCreate(40, sizeof(u8));

    USART4_USART_Config();
}

/**
 * @brief  初始化usart用到的GPIO引脚
 * @param  无
 * @retval 无
 */
static void USART4_GPIO_Config(void)
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
    NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
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
static void USART4_USART_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    /* config USART clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    /* USART GPIO config */
    /* Configure USART Tx as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    /* Configure USART Rx as input floating */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    /* USART1 mode config */
    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(UART4, &USART_InitStructure);

    /* 中断配置 */
    USART_ITConfig(UART4, USART_IT_RXNE, ENABLE); // 使能串口接收中断
    USART_ITConfig(UART4, USART_IT_IDLE, ENABLE); // 使能串口总线空闲中断

    USART_NVIC_Configuration();

    USART_Cmd(UART4, ENABLE);
}

void UART4_IRQHandler(void)
{
    uint8_t ucCh;

    if (USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)
    {
        ucCh = USART_ReceiveData(UART4);

        xQueueSendFromISR(drv_height_queue, &ucCh, NULL);
        // if (ucCh == 0x0A)

        // {
        // 	Baro_Height_Fram.InfBit.FramLength = 0;
        // 	Baro_Height_Fram.Data_RX_BUF[Baro_Height_Fram.InfBit.FramLength] = '\0';
        // }

        // Baro_Height_Fram.Data_RX_BUF[Baro_Height_Fram.InfBit.FramLength++] = ucCh;
    }

    if (USART_GetITStatus(UART4, USART_IT_IDLE) == SET) // 数据帧接收完毕
    {
        // Baro_Height_Fram.InfBit.FramFinishFlag = 1;

        ucCh = USART_ReceiveData(UART4);
    }
}

static void usart_send_byte(USART_TypeDef *pUSARTx, uint8_t data)
{
    USART_SendData(pUSARTx, data);
    while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET)
        ;
}

void USART4_send_array(uint8_t *array, uint8_t num)
{
    uint8_t i;
    for (i = 0; i < num; i++)
    {
        usart_send_byte(UART4, array[i]);
    }
    while (USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET)
        ;
}

static Drv_Voice_t drv_voice_t;
Drv_Voice_pt drv_voice_pt;

void yyhy(void)
{
    USART4_send_array(vol, sizeof(vol));
    // yy_val = 30;
    vTaskDelay(100);
    USART4_send_array(mode_single_stop, sizeof(mode_single_stop));
    vTaskDelay(100);
    USART4_send_array(set_02, sizeof(set_02));
    // yy_h_flag = 1;
    vTaskDelay(100);
}

void init_voice(void) 
{
    drv_voice_pt = &drv_voice_t;
    drv_voice_pt->yyhy = yyhy;

    USART4_Init();
}


