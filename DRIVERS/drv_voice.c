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

void height_zero_finish(void) 
{
    USART4_send_array(vol, sizeof(vol));
    // yy_val = 30;
    vTaskDelay(100);
    USART4_send_array(mode_single_stop, sizeof(mode_single_stop));
    vTaskDelay(100);
    USART4_send_array(set_04, sizeof(set_04));
    vTaskDelay(100);
}

void net_success(void) // 03： 网络连接成功语音
{
    USART4_send_array(vol, sizeof(vol));
    // yy_val = 30;
    vTaskDelay(100);
    USART4_send_array(mode_single_stop, sizeof(mode_single_stop));
    vTaskDelay(100);
    USART4_send_array(set_03, sizeof(set_03));
    vTaskDelay(100);
}

void yy_close(void) // 05： 关机语音
{
    USART4_send_array(vol, sizeof(vol));
    // yy_val = 30;
    vTaskDelay(100);
    USART4_send_array(mode_single_stop, sizeof(mode_single_stop));
    vTaskDelay(100);
    USART4_send_array(set_05, sizeof(set_05));
    vTaskDelay(100);
}

void pose_warn(void) // 01： 姿态报警语音
{
    USART4_send_array(vol, sizeof(vol));
    // yy_val = 10;
    vTaskDelay(100);
    USART4_send_array(mode_single_tune_circulation, sizeof(mode_single_tune_circulation));
    vTaskDelay(100);
    USART4_send_array(set_01, sizeof(set_01));
    vTaskDelay(100);
}

void low_battery(void) // 09 低电量报警
{
    USART4_send_array(vol, sizeof(vol));
    // yy_val = 10;
    vTaskDelay(100);
    USART4_send_array(mode_single_tune_circulation, sizeof(mode_single_tune_circulation));
    vTaskDelay(100);
    USART4_send_array(set_09, sizeof(set_09));
    vTaskDelay(100);
}

void unlock_finish(void) // 11 设备已解锁语音
{
    USART4_send_array(vol, sizeof(vol));
    // yy_val = 30;
    vTaskDelay(100);
    USART4_send_array(mode_single_stop, sizeof(mode_single_stop));
    vTaskDelay(100);
    USART4_send_array(set_11, sizeof(set_11));
    vTaskDelay(100);
}

/************新增3个语音模块*****************/
void lock_request(void) // 06 请求上锁语音
{
    USART4_send_array(vol, sizeof(vol));
    // yy_val = 10;
    vTaskDelay(100);
    USART4_send_array(mode_single_stop, sizeof(mode_single_stop));
    vTaskDelay(100);
    USART4_send_array(set_06, sizeof(set_06));
    vTaskDelay(100);
}

void unlock_request(void) // 07 请求解锁语音
{
    USART4_send_array(vol, sizeof(vol));
    // yy_val = 10;
    vTaskDelay(100);
    USART4_send_array(mode_single_stop, sizeof(mode_single_stop));
    vTaskDelay(100);
    USART4_send_array(set_07, sizeof(set_07));
    vTaskDelay(100);
}

void sos(void) // 08 紧急求救语音
{
    USART4_send_array(vol, sizeof(vol));
    // yy_val = 10;
    vTaskDelay(100);
    USART4_send_array(mode_single_stop, sizeof(mode_single_stop));
    vTaskDelay(100);
    USART4_send_array(set_08, sizeof(set_08));
    vTaskDelay(100);
}

void danger_height(void) // 12 危险高度语音
{
    USART4_send_array(vol, sizeof(vol));
    // yy_val = 10;
    vTaskDelay(100);
    USART4_send_array(mode_single_stop, sizeof(mode_single_stop));
    vTaskDelay(100);
    USART4_send_array(set_12, sizeof(set_12));
    vTaskDelay(100);
}
void fatigue(void) // 13 疲劳语音
{
    USART4_send_array(vol, sizeof(vol));
    // yy_val = 10;
    vTaskDelay(100);
    USART4_send_array(mode_single_tune_circulation, sizeof(mode_single_tune_circulation));
    vTaskDelay(100);
    USART4_send_array(set_13, sizeof(set_13));
    vTaskDelay(100);
}
void open_hooking(void) // 14 打开挂钩
{
    USART4_send_array(vol, sizeof(vol));
    // yy_val = 10;
    vTaskDelay(100);
    USART4_send_array(mode_single_tune_circulation, sizeof(mode_single_tune_circulation));
    vTaskDelay(100);
    USART4_send_array(set_14, sizeof(set_14));
    vTaskDelay(100);
}
void init_voice(void) 
{
    drv_voice_pt = &drv_voice_t;
    drv_voice_pt->yyhy = yyhy;
    drv_voice_pt->height_zero_finish = height_zero_finish;
    drv_voice_pt->net_success = net_success;
    drv_voice_pt->yy_close = yy_close;
    drv_voice_pt->pose_warn = pose_warn;
    drv_voice_pt->low_battery = low_battery;
    drv_voice_pt->unlock_finish = unlock_finish;
    drv_voice_pt->lock_request = lock_request;
    drv_voice_pt->unlock_request = unlock_request;
    drv_voice_pt->sos = sos;
    drv_voice_pt->danger_height = danger_height;
    drv_voice_pt->open_hooking = open_hooking;
    drv_voice_pt->lock = thread_cslock_init("drv_voice"),

    USART4_Init();
}


