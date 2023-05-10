#include "my_queue.h"
#include <string.h>
#include "os_system.h"
void queue_init(uint8_t **head, uint8_t **tail, uint8_t *buff, uint8_t len)
{
    memset(buff, 0, len);
    *head = buff;
    *tail = buff;
}

void queue_in(uint8_t **head, uint8_t **tail, uint8_t *buff, uint8_t len, uint8_t *data, uint8_t data_len, FunctionalState is_interrupt)
{
    uint8_t pSta = 0;
    if (is_interrupt == ENABLE)
    {
        cpu_critical_control(CPU_ENTER_CRITICAL, &pSta);
    }

    for (uint8_t i = 0; i < data_len; i++)
    {
        **tail = data[i];
        (*tail)++;
        if (*tail == buff + len)
        {
            *tail = buff;
        }
        if (*tail == *head)
        {
            if (++(*head) == buff + len)
            {
                *head = buff;
            }
        }
        /* code */
    }

    if (is_interrupt == ENABLE)
    {
        cpu_critical_control(CPU_EXIT_CRITICAL, &pSta);
    }
}

uint8_t queue_out(uint8_t **head, uint8_t **tail, uint8_t *buff, uint8_t len, uint8_t *data, FunctionalState is_interrupt)
{
    uint8_t pSta = 0;

    if (is_interrupt == ENABLE)
    {
        uint8_t pSta = 0;
        cpu_critical_control(CPU_ENTER_CRITICAL, &pSta);
    }
    uint8_t status = 0;
    if (*head != *tail)
    {
        *data = **head;
        status = 1;
        if (++(*head) == buff + len)
        {
            *head = buff;
        }
    }
    if (is_interrupt == ENABLE)
    {
        cpu_critical_control(CPU_EXIT_CRITICAL, &pSta);
    }
    return status;
}

uint16_t queue_len(uint8_t **head, uint8_t **tail, uint8_t *buff, uint8_t len)
{
    if (*tail > *head)
    {
        return *tail - *head;
    }
    if (*tail < *head)
    {
        return *tail + len - *head;
    }
    return 0;
}

static unsigned char getprimask(void)
{
    return (!__get_PRIMASK());
}

static void cpu_critical_control(CPU_EA_CMD cmd, unsigned char *pSta)
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