#include "drv_voice.h"
#include "common.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "drv_gps.h"
#include "os_system__typedef.h"
#include <string.h>

static void USART1_GPIO_Config(void);
static void USART1_USART_Config(void);
static void USART1_NVIC_Configuration(void);

static QueueHandle_t drv_gps_queue;

/**
 * @brief  初始化函数
 * @param  无
 * @retval 无
 */
static void USART1_Init(void)
{
    //	USART1_GPIO_Config ();
    drv_gps_queue = xQueueCreate(100, sizeof(u8));

    USART1_USART_Config();
}

/**
 * @brief  初始化usart用到的GPIO引脚
 * @param  无
 * @retval 无
 */
static void USART1_GPIO_Config(void)
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
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
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
static void USART1_USART_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    /* config USART clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    /* USART GPIO config */
    /* Configure USART Tx as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Configure USART Rx as input floating */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* USART1 mode config */
    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);

    /* 中断配置 */
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); // 使能串口接收中断
    USART_ITConfig(USART1, USART_IT_IDLE, ENABLE); // 使能串口总线空闲中断

    USART_NVIC_Configuration();

    USART_Cmd(USART1, ENABLE);
}

void USART1_IRQHandler(void)
{
    uint8_t ucCh;

    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        ucCh = USART_ReceiveData(USART1);

        xQueueSendFromISR(drv_gps_queue, &ucCh, NULL);
        // if (ucCh == 0x0A)

        // {
        // 	Baro_Height_Fram.InfBit.FramLength = 0;
        // 	Baro_Height_Fram.Data_RX_BUF[Baro_Height_Fram.InfBit.FramLength] = '\0';
        // }

        // Baro_Height_Fram.Data_RX_BUF[Baro_Height_Fram.InfBit.FramLength++] = ucCh;
    }

    if (USART_GetITStatus(USART1, USART_IT_IDLE) == SET) // 数据帧接收完毕
    {
        // Baro_Height_Fram.InfBit.FramFinishFlag = 1;

        ucCh = USART_ReceiveData(USART1);
    }
}

static void usart_send_byte(USART_TypeDef *pUSARTx, uint8_t data)
{
    USART_SendData(pUSARTx, data);
    while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET)
        ;
}

void USART1_send_array(uint8_t *array, uint8_t num)
{
    uint8_t i;
    for (i = 0; i < num; i++)
    {
        usart_send_byte(USART1, array[i]);
    }
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
        ;
}

static u8 itoa_gps(u8 i)
{
    if (i <= 9)
    {
        return i + '0';
    }
    else if (i <= 0x0f)
    {
        return i - 0x0a + 'A';
    }
    return 0;
}
/*
0x01: GPS
0x02: BDS
0x04: GLONASS
*/
static void set_gnss()
{
    u8 value = 3;
    char str_tmp[25] = "$PCAS04,2*1B\r\n";
    str_tmp[8] = value + 0x30;
    str_tmp[10] = 0;
    str_tmp[11] = 0;
    for (u8 i = 1; i < 9; i++)
    {
        str_tmp[10] ^= (str_tmp[i] >> 4);
        str_tmp[11] ^= (str_tmp[i] & 0x0f);
    }
    str_tmp[10] = itoa_gps(str_tmp[10]);
    str_tmp[11] = itoa_gps(str_tmp[11]);

    /* code */
    USART1_send_array((u8 *)str_tmp, strlen(str_tmp));
}

void get_gps(void)
{
    char *temp1;
    char *temp2;

    char *temp3;
    char *temp4;

    while (1)
    {

        while (xQueueReceive(drv_gps_queue, &drv_gps_pt->buff[drv_gps_pt->buff_length++], portMAX_DELAY))
        {
            if (drv_gps_pt->buff[drv_gps_pt->buff_length - 1] == '$')
            {
                drv_gps_pt->buff_length = 0;
            }

            if (drv_gps_pt->buff[0] == 'G' && drv_gps_pt->buff[4] == 'C')
            {
                temp1 = strstr((const char *)drv_gps_pt->buff, "A,");
                temp2 = strstr((const char *)drv_gps_pt->buff, ",N,");

                temp3 = strstr((const char *)drv_gps_pt->buff, "N,");
                temp4 = strstr((const char *)drv_gps_pt->buff, ",E,");
                if (temp1 && temp2)
                {
                    strncpy(drv_gps_pt->latitude, temp1+ 2, temp2 - temp1 -2);

                    // thread_cslock_lock(drv_gps_pt->lock, MaxTick);
                    // thread_cslock_free(drv_gps_pt->lock);
                }
                if (temp3 && temp4)
                {
                    strncpy(drv_gps_pt->longitude, temp3 + 2, temp4 - temp3 -2);

                    // thread_cslock_lock(drv_gps_pt->lock, MaxTick);
                    // thread_cslock_free(drv_gps_pt->lock);
                    drv_gps_pt->buff_length = 0;
                }
            }

            if (drv_gps_pt->buff_length >= 100)
            {

                drv_gps_pt->buff_length = 0;
            }
        }
    }
}
static Drv_Gps_t drv_gps_t;
Drv_Gps_pt drv_gps_pt;

void init_gps(void)
{
    drv_gps_pt = &drv_gps_t;
    drv_gps_pt->set_gnss = set_gnss;
    drv_gps_pt->lock = thread_cslock_init("drv_gps");
    memset(drv_gps_pt->buff, 0, 101);
    memset(drv_gps_pt->latitude, 0, 20);
    memset(drv_gps_pt->longitude, 0, 20);

    drv_gps_pt->buff_length = 0;
    drv_gps_pt->get_gps = get_gps;


    USART1_Init();

    // drv_gps_pt->set_gnss();
}
