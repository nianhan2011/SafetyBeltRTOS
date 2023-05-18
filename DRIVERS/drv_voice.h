#ifndef __DRV_VOICE_H
#define __DRV_VOICE_H

#include "stm32f10x.h"

typedef struct
{
    void (*yyhy)(void);

} Drv_Voice_t, *Drv_Voice_pt;

extern Drv_Voice_pt drv_voice_pt;
void init_voice(void);

#endif