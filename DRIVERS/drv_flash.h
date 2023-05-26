#ifndef DRV_FLASH_H
#define DRV_FLASH_H

#include "stm32f10x.h"

typedef struct
{
    u8 attitude_warn;
    u8 emergency_unclock;
    u8 height_check;
    u8 height_auto_lock;
} Permissions_t, *Permissions_pt;

typedef struct
{
    Permissions_t permissions_t ;
    void (*write_to_flash)(void);
    void (*read_from_flash)(void);

} Drv_Flash_t, *Drv_Flash_pt;
// extern Permissions_t permissions_t;
extern Drv_Flash_pt drv_flash_pt;
void init_flash(void);

#endif
