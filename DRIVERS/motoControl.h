#ifndef __MOTO_H
#define __MOTO_H

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

} Drv_Moto_t, *Drv_Moto_pt;

extern Drv_Moto_pt drv_moto_pt;
void init_moto(void);

#endif


