
#include "delay.h"
#include "common.h"
#include "led.h"
#include "usart.h"
#include <stdio.h>
#include "timer.h"
#include "os_system.h"
#include "os_system__typedef.h"
#include "drv_opt.h"
#include "drv_key.h"
#include "drv_adc.h"
#include "drv_voice.h"

void main_task(void);

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); // 设置NVIC中断分组2:2位抢占优先级，2位响应优先级

    thread_create(main_task, "main_task", 1024, NULL, 1, NULL);
    vTaskStartScheduler();
}

void main_task(void)
{
    //    delay_init();

    init_drv_timer_tick();
    led_init();
    key_init();
    init_adc();
    init_drv_height();
    init_voice();

    vTaskDelay(500);

    taskENTER_CRITICAL(); // 进入临界区

    thread_create(turn_led, "turn_led", 128, NULL, 2, NULL);
    thread_create(drv_height_pt->get_barometric, "get_barometric", 1024, NULL, 2, NULL);
    thread_create(key_proc, "key_proc", 1024, NULL, 2, NULL);
    // thread_create(drv_adc_pt->get_adc, "get_adc", 1024, NULL, 2, NULL);

    vTaskDelete(NULL);   // 删除AppTaskCreate任务
    taskEXIT_CRITICAL(); // 退出临界区
}