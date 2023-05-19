#ifndef __DRV_VOICE_H
#define __DRV_VOICE_H

#include "stm32f10x.h"

typedef struct
{
    void (*yyhy)(void);
    void (*height_zero_finish)(void);
    void (*net_success)(void);
    void (*yy_close)(void);
    void (*pose_warn)(void);
    void (*low_battery)(void);
    void (*unlock_finish)(void);
    void (*lock_request)(void);
    void (*unlock_request)(void);
    void (*sos)(void);
    void (*danger_height)(void);
    void (*open_hooking)(void);
} Drv_Voice_t, *Drv_Voice_pt;

extern Drv_Voice_pt drv_voice_pt;
void init_voice(void);

#endif