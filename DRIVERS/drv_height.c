#include "drv_height_port.h"
#include "os_system__typedef.h"
#include "drv_opt.h"
#include "string.h"
#include "stdlib.h"
#include "Common.h"
#include "drv_voice.h"
u8 cmd1[30] = "https://www.wit-motion.cn/\n";
u8 cmd2[20] = {0xFF, 0xAA, 0x0d, 0x03, 0x00, 0x0a};
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

void set_height_zero(void)
{

    // u32 isOpen;

    // xTaskNotifyWait(0x0, 0xFFFFFFFF, &isOpen, portMAX_DELAY);

    // if (isOpen) {
    while ((drv_height_pt->height > 1.0) || (drv_height_pt->height < -1.0))
    {
        USART_printf(UART5, cmd1);
        //        timtick->delay(10);
        vTaskDelay(100);
        usart5_send_array(cmd2, 6);
        //        taskEXIT_CRITICAL();

        vTaskDelay(100);
    }

    drv_voice_pt->height_zero_finish();

    vTaskDelete(NULL);

}

void init_drv_height(void)
{
    drv_height_pt = &drv_height_t;
    drv_height_pt->lock = thread_cslock_init("drv_height");
    memset(drv_height_pt->buff, 0, 40);
    drv_height_pt->buff_length = 0;
    drv_height_pt->set_height_zero = set_height_zero;
    drv_height_pt->get_barometric = get_barometric;

    USART5_Init();
}
