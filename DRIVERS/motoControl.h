#ifndef __MOTO_H
#define __MOTO_H
#include "os_system__typedef.h"
typedef enum
{
    Device_Status_Lock,
    Device_Status_UnLock,
    Device_Status_Off,

} Device_Status;

typedef enum
{
    Yaokou_Status_Lock,
    Yaokou_Status_UnLock,
} Yaokou_Status;

typedef enum
{
    Zitai_Status_Safe,
    Zitai_Status_UnSafe,
} Zitai_Status;

typedef enum
{
    No_Request,
    Lock_Request,
    UnLock_Request,
} Lock_Request_Status;

typedef struct
{
    Device_Status device_status;
    // Lock_Request_Status request_status;
    Yaokou_Status yaokou_status;
    Zitai_Status zitai_status;

    u8 attitude_warn;
    u8 snap_warn;

    u8 fail_warn;
    u8 fatigure_warn;
    u8 sos;
    u8 height_warn;

    // u8 emer_unlock;

    u8 pc0;
    u8 pc15;
    u8 pc14;
    u8 pc13;

    // u8 old_pc0;
    // u8 old_pc15;
    // u8 old_pc14;
    // u8 old_pc13;

    void (*get_status)(void);

} Drv_Moto_t, *Drv_Moto_pt;

extern Drv_Moto_pt drv_moto_pt;
void init_moto(void);

#endif
