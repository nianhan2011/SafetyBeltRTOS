
#define __IO volatile /*!< defines 'read / write' permissions   */

#include "os_system.h"
#include "stm32f10x.h"

__IO OS_Task os_task_list[OS_TASK_SUM];
CPU_Interrupt_CallBack_t cpu_interrupt_callBack_t;

void os_task_init(void)
{
    unsigned char i;
    for (i = 0; i < OS_TASK_SUM; i++)
    {
        os_task_list[i].task_handle = 0;
        os_task_list[i].os_task_status = OS_SLEEP;
        os_task_list[i].run_period = 0;
        os_task_list[i].run_cnt = 0;
    }
}

void os_create_task(unsigned char id, Task_Handle task_handle, unsigned short period, OS_Task_Status status)
{
    if (!os_task_list[id].task_handle)
    {
        os_task_list[id].task_handle = task_handle;
        os_task_list[id].os_task_status = OS_SLEEP;
        os_task_list[id].run_period = period;
        os_task_list[id].run_cnt = 0;
    }
}

void os_clock_interrupt_handle(void)
{
    unsigned char i;
    for (i = 0; i < OS_TASK_SUM; i++)
    {
        if (os_task_list[i].task_handle)
        {
            os_task_list[i].run_cnt++;
            if (os_task_list[i].run_cnt >= os_task_list[i].run_period)
            {
                os_task_list[i].run_cnt = 0;
                os_task_list[i].os_task_status = OS_RUN;
            }
        }
    }
}

void os_start(void)
{
    unsigned char i;
    while (1)
    {
        for (i = 0; i < OS_TASK_SUM; i++)
        {
            if (os_task_list[i].os_task_status == OS_RUN)
            {
                os_task_list[i].os_task_status = OS_SLEEP;
                os_task_list[i].task_handle();
            }
        }
    }
}

void os_cpu_interrupt_register(CPU_Interrupt_CallBack_t _cpu_interrupt_callBack_t)
{
    if (cpu_interrupt_callBack_t == 0)
    {
        cpu_interrupt_callBack_t = _cpu_interrupt_callBack_t;
    }
}

static unsigned char getprimask(void)
{
    return (!__get_PRIMASK());
}

void cpu_critical_control(CPU_EA_CMD cmd, unsigned char *pSta)
{

    if (cmd == CPU_ENTER_CRITICAL)
    {
        *pSta = getprimask();
        __disable_irq();
    }
    else
    {
        if (*pSta)
        {
            __enable_irq();
        }
        else
        {
            __disable_irq();
        }
    }
}