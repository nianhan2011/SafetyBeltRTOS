#include "drv_height_port.h"
#include "os_system__typedef.h"
#include "drv_opt.h"
#include "string.h"
#include "stdlib.h"
#include "Common.h"
#include "drv_voice.h"
#include "drv_me.h"
#include "drv_voice.h"
#include "drv_flash.h"
#include "motoControl.h"

u8 cmd1[30] = "https://www.wit-motion.cn/\n";
u8 cmd2[20] = {0xFF, 0xAA, 0x0d, 0x03, 0x00, 0x0a};
static Drv_Height_t drv_height_t;
Drv_Height_pt drv_height_pt;

void get_barometric(void)
{
    char *temp;
    float orign_height;
    float height_update;
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

                        height_update = orign_height - drv_height_pt->orign_height;
                        height_update = __fabsf(height_update);
                        drv_height_pt->orign_height = orign_height;
                        drv_height_pt->height = (u32)(orign_height * 10);

                        if (height_update >= 0.5 && drv_height_pt->set_zero_success)
                        {
                            drv_me_pt->set_send_cnt(0);
                        }
                    }

                    drv_height_pt->buff_length = 0;
                }
            }
        }
    }
}

void height_check(void)
{
    while (1)
    {
        if (drv_height_pt->orign_height >= 2.0 && drv_height_pt->set_zero_success)
        {

            if (drv_height_pt->danger_height_lock_timer)
            {
                if (drv_height_pt->danger_height_lock_cnt == 0)
                {
                    drv_height_pt->danger_height_lock_cnt = 300;
                    drv_height_pt->danger_height_lock_timer = 0;
                    // drv_voice_pt->lock_request();
                    // 上锁
                }
                else
                {
                    drv_height_pt->danger_height_lock_cnt--;
                }
                continue;
            }
            if (drv_height_pt->danger_height_voice_cnt == 0)
            {
                drv_height_pt->danger_height_voice_cnt = 1500;
                drv_voice_pt->danger_height();
                drv_flash_pt->read_from_flash();
                if (drv_flash_pt->permissions_t.height_auto_lock)
                {
                    drv_height_pt->danger_height_lock_timer = 1;
                }
            }
            else
            {
                drv_height_pt->danger_height_voice_cnt--;
            }
        }
        else if (drv_height_pt->orign_height < 2.0 && drv_height_pt->set_zero_success)
        {
            // 解锁
            drv_height_pt->danger_height_voice_cnt = 0;
            drv_height_pt->danger_height_lock_cnt = 300;
            drv_height_pt->danger_height_lock_timer = 0;
        }
        else
        {
            drv_height_pt->danger_height_voice_cnt = 0;
            drv_height_pt->danger_height_lock_cnt = 300;
            drv_height_pt->danger_height_lock_timer = 0;
        }

        vTaskDelay(10);
    }
}

void fall_check(void)
{
    float height_update;

    while (1)
    {
        if (drv_height_pt->pre_height)
        {
            height_update = drv_height_pt->pre_height - drv_height_pt->orign_height;
            if (height_update >= 10.0 && drv_height_pt->set_zero_success)
            {
                drv_moto_pt->fail_warn = 1;
                drv_me_pt->set_send_cnt(0);
            }
        }

        drv_height_pt->pre_height = drv_height_pt->orign_height;

        vTaskDelay(1000);
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
    drv_height_pt->set_zero_success = 1;
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
    drv_height_pt->set_zero_success = 0;
    drv_height_pt->danger_height_voice_cnt = 0;
    drv_height_pt->danger_height_lock_cnt = 300;
    drv_height_pt->danger_height_lock_timer = 0;
    drv_height_pt->height_check = height_check;
    drv_height_pt->fall_check = fall_check;

    USART5_Init();
}
