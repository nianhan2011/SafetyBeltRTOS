
#ifndef DRV_OPT_H
#define DRV_OPT_H

#include "stm32f10x.h"
#include "os_system__typedef.h"

typedef struct
{
    u8 buff[40];
    u8 buff_length;
    float height;
    thread_cslock_t lock;
} Drv_Height_t, *Drv_Height_pt;

extern Drv_Height_pt drv_height_pt;

void init_drv_height(void);
void get_barometric(void);
#endif /* TIME_TEST_H */
