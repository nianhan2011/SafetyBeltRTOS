
#include "delay.h"
#include "common.h"
#include "led.h"
#include "usart.h"
#include <stdio.h>
#include "timer.h"
#include "os_system.h"
#include "os_system__typedef.h"
#include "drv_opt.h"

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

    LED_Init();

    init_drv_height();
    // ADCx_Init();
    // // uart_init(9600);
    // sk_init();
    // vdf_init();
    // TIM3_Int_Init(100 - 1, 7200 - 1);
    vTaskDelay(500);

    // modbus_open();

    //    vTaskDelay(1000);
    //    modbus_open();
    //    vTaskDelay(1000);

    // os_task_init();

    // os_create_task(OS_TASK1, sk_proc, 10, OS_SLEEP);
    // os_create_task(OS_TASK2, led_turn, 10, OS_SLEEP);
    // os_create_task(OS_TASK3, vdf_receive_proc, 1, OS_SLEEP);
    // os_create_task(OS_TASK4, vdf_send_proc, 10, OS_SLEEP);

    // os_start();
    taskENTER_CRITICAL(); // 进入临界区

    // thread_create(sk_proc, "sk_proc", 1024, NULL, 2, NULL);
    // thread_create(led_turn, "led_turn", 1024, NULL, 2, NULL);
    // thread_create(vdf_receive_proc, "vdf_receive_proc", 1024, NULL, 2, NULL);
    // thread_create(vdf_send_proc, "vdf_send_proc", 1024, NULL, 2, NULL);
    // thread_create(GET_ADC, "GET_ADC", 512, NULL, 2, NULL);

    thread_create(turn_led, "turn_led", 125, NULL, 2, NULL);
    thread_create(get_barometric, "get_barometric", 512, NULL, 2, NULL);

    vTaskDelete(NULL);   // 删除AppTaskCreate任务
    taskEXIT_CRITICAL(); // 退出临界区
}