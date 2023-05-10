#ifndef __OS_SYSTEM_H
#define __OS_SYSTEM_H

typedef enum
{
    OS_TASK1,
    OS_TASK2,
    OS_TASK3,
    OS_TASK4,
    // OS_TASK5,
    // OS_TASK6,
    // OS_TASK7,

    OS_TASK_SUM
} OS_TaskID;

typedef enum
{
    OS_SLEEP,
    OS_RUN = !OS_SLEEP
} OS_Task_Status;

typedef enum
{
    CPU_ENTER_CRITICAL,
    CPU_EXIT_CRITICAL
} CPU_EA_CMD;

typedef void (*Task_Handle)(void);

typedef void (*CPU_Interrupt_CallBack_t)(CPU_EA_CMD cmd, unsigned char *pSta);

typedef struct
{
    Task_Handle task_handle;
    OS_Task_Status os_task_status;
    unsigned short run_period;
    unsigned short run_cnt;
} OS_Task;

void os_task_init(void);

void os_create_task(unsigned char id, Task_Handle task_handle, unsigned short period, OS_Task_Status status);

void os_clock_interrupt_handle(void);

void os_start(void);

void os_cpu_interrupt_register(CPU_Interrupt_CallBack_t _cpu_interrupt_callBack_t);

void cpu_critical_control(CPU_EA_CMD cmd, unsigned char *pSta);

#endif
