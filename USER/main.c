
#include "delay.h"
#include "common.h"
#include "led.h"
#include <stdio.h>
#include "timer.h"
#include "os_system.h"
#include "os_system__typedef.h"
#include "drv_opt.h"
#include "drv_key.h"
#include "drv_adc.h"
#include "drv_voice.h"
#include "drv_gps.h"
#include "drv_me.h"
#include "drv_flash.h"
#include "motoControl.h"
void main_task(void);

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); // 设置NVIC中断分组2:2位抢占优先级，2位响应优先级

    thread_create(main_task, "main_task", 256, NULL, 1, NULL);
    vTaskStartScheduler();
}

void IWDG_Config(uint8_t prv, uint16_t rlv)
{
    // 使能 预分频寄存器PR和重装载寄存器RLR可写
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    // 设置预分频器值
    IWDG_SetPrescaler(prv);
    // 设置重装载寄存器值
    IWDG_SetReload(rlv);
    // 把重装载寄存器的值放到计数器中
    IWDG_ReloadCounter();
    // 使能 IWDG
    IWDG_Enable();
}
// 喂狗
void IWDG_Feed(void)
{
    // 把重装载寄存器的值放到计数器中，喂狗，防止IWDG复位
    // 当计数器的值减到0的时候会产生系统复位
    IWDG_ReloadCounter();
}

void Iwdg_task(void *pvParameters)
{
    EventBits_t uxBits;
    const TickType_t xTicksToWait = 2000 / portTICK_PERIOD_MS; /* 最大延迟2000ms */
    /*
      开始执行启动任务主函数前使能独立看门狗。
      设置LSI是64分频，下面函数参数范围0-0xFFF，分别代表最小值2ms和最大值6552ms

       设置LSI是128分频，下面函数参数范围0-0xFFF，分别代表最小值4ms和最大值13104ms
       Tout = prv/40 * rlv (s)
      下面设置的是5s，如果5s内没有喂狗，系统复位。
    */
    IWDG_Config(IWDG_Prescaler_64, 625 * 5);
    while (1)
    {
        /* 等待所有任务发来事件标志 */
        // uxBits = xEventGroupWaitBits(xCreatedEventGroup, /* 事件标志组句柄 */
        //                              TASK_BIT_ALL,       /* 等待TASK_BIT_ALL被设置 */
        //                              pdTRUE,             /* 退出前TASK_BIT_ALL被清除，这里是TASK_BIT_ALL都被设置才表示“退出”*/
        //                              pdTRUE,             /* 设置为pdTRUE表示等待TASK_BIT_ALL都被设置*/
        //                              xTicksToWait);      /* 等待延迟时间 */
        // if ((uxBits & TASK_BIT_ALL) == TASK_BIT_ALL)     // 判断各个任务是否执行
        // {

            
            IWDG_Feed();

            vTaskDelay(2000);
        // }
    }
}

void main_task(void)
{

    init_drv_timer_tick();
    led_init();
    key_init();
    init_adc();
    init_drv_height();
    init_voice();
    init_gps();
    init_me();
    init_flash();
    init_moto();
    vTaskDelay(500);

    taskENTER_CRITICAL(); // 进入临界区

    thread_create(drv_adc_pt->startListen, "startListen", 64, NULL, 4, NULL);
    thread_create(drv_adc_pt->shutDownListen, "shutDownListen", 128, NULL, 2, NULL);

    thread_create(drv_me_pt->tcp_task, "tcp_task", 128, NULL, 3, NULL);

    thread_create(drv_adc_pt->get_adc, "get_adc", 64, NULL, 2, NULL);
    // thread_create(drv_adc_pt->shutDownListen, "shutDownListen", 32, NULL, 2, NULL);

    thread_create(turn_led, "turn_led", 64, NULL, 2, NULL);
    thread_create(key_proc, "key_proc", 256, NULL, 2, NULL);

    thread_create(drv_height_pt->get_barometric, "get_barometric", 164, NULL, 2, NULL);
    thread_create(drv_height_pt->fall_check, "fall_check", 164, NULL, 2, NULL);
    thread_create(drv_height_pt->height_check, "height_check", 98, NULL, 2, NULL);

    thread_create(drv_gps_pt->get_gps, "get_gps", 128, NULL, 2, NULL);
 
    thread_create(drv_me_pt->me_proc, "me_proc", 128, NULL, 2, NULL);
    thread_create(drv_me_pt->me3630_send, "me3630_send", 128, NULL, 2, NULL);

    thread_create(drv_moto_pt->get_status, "get_status", 128, NULL, 2, NULL);
 
    thread_create(Iwdg_task, "Iwdg_task", 128, NULL, 4, NULL);

    vTaskDelete(NULL);   // 删除AppTaskCreate任务
    taskEXIT_CRITICAL(); // 退出临界区
}
