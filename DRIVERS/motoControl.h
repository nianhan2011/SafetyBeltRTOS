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
    No_Request,
    Lock_Request,
    UnLock_Request,
} Lock_Request_Status;


typedef struct
{
    Device_Status device_status;
    Lock_Request_Status request_status;

    u8 attitude_error;
    u8 attitude_normal;
    u8 snap_error;
    u8 snap_normal;
    u8 fail_warn;
    u8 fatigure_warn;
    u8 sos;
    u8 emer_unlock;

} Drv_Moto_t, *Drv_Moto_pt;

extern Drv_Moto_pt drv_moto_pt;
void init_moto(void);

#endif


