#ifndef __DRV_HEIGHT_PORT_H
#define __DRV_HEIGHT_PORT_H
#include "os_system__typedef.h"

extern QueueHandle_t drv_height_queue;

void USART5_Init(void);
void usart5_send_array(uint8_t *array, uint8_t num);

#endif
