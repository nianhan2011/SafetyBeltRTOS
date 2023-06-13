#ifndef __DRV_GPS_H
#define __DRV_GPS_H

#include "stm32f10x.h"

typedef struct
{
    u8 buff[101];
    u8 buff_length;
    char latitude[50];  // 纬度
    char longitude[50]; // 经度
    thread_cslock_t lock;
    void (*get_gps)(void);
    void (*set_gnss)(void);
} Drv_Gps_t, *Drv_Gps_pt;

extern Drv_Gps_pt drv_gps_pt;
void init_gps(void);

#endif