#include "drv_height_port.h"
#include "os_system__typedef.h"
#include "drv_opt.h"
#include "string.h"
#include "stdlib.h"

static Drv_Height_t drv_height_t;
Drv_Height_pt drv_height_pt;

void get_barometric(void)
{
    char *temp;
    float orign_height;
    while (1)
    {

        if (xQueueReceive(drv_height_queue, &drv_height_pt->buff[drv_height_pt->buff_length++], portMAX_DELAY) == pdPASS)
        {
            while (xQueueReceive(drv_height_queue, &drv_height_pt->buff[drv_height_pt->buff_length++], portMAX_DELAY))
            {
                if (drv_height_pt->buff[0] != 'H')
                {
                    drv_height_pt->buff_length = 0;
                }
                if (drv_height_pt->buff_length >= 39)
                {
                    temp = strstr((const char *)drv_height_pt->buff, "Height = ");
                    if (temp)
                    {
                        orign_height = atof(temp + 9);
                        thread_cslock_lock(drv_height_pt->lock, MaxTick);
                        drv_height_pt->height = orign_height * 10;
                        thread_cslock_free(drv_height_pt->lock);
                    }

                    drv_height_pt->buff_length = 0;
                }
            }
        }
    }
}

void init_drv_height(void)
{
    USART5_Init();

    drv_height_pt = &drv_height_t;
    drv_height_pt->lock = thread_cslock_init("drv_height");
    memset(drv_height_pt->buff, 0, 20);
    drv_height_pt->buff_length = 0;
}
