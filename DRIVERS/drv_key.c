#include "drv_key.h"
#include "Common.h"
#include "os_system__typedef.h"
#include "drv_opt.h"
#include "drv_voice.h"
#include "drv_me.h"
#include "motoControl.h"
#include "drv_flash.h"

static uint8_t get_key1_state(void)
{
    // u8 status;
    // status = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14);
    // return !status;
    return !PBin(14);
}
static uint8_t get_key2_state(void)
{
    // u8 status;
    // status = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15);
    // return !status;
    return !PBin(15);
}

static void key_gpio_init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // 速度50MHz
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

uint8_t (*get_key_state[KEY_SUM])() = {get_key1_state, get_key2_state};
KEY_PROPERTY key_propertys[KEY_SUM];

KEY_CALLBACK key_callback;

void key_init(void)
{
    key_gpio_init();
    key_callback = 0;
    for (uint8_t i = 0; i < KEY_SUM; i++)
    {
        key_propertys[i].key_step = KEY_STEP_WAIT;
        key_propertys[i].scan_time = 2;
        key_propertys[i].press_long_time = 300;
        key_propertys[i].press_continuous_long_time = 15;
        key_propertys[i].key_status = 0;
        key_propertys[i].key_double_click_time = 20;
    }
}

void key_handle(KEY_NAME key_name, KEY_EVENT key_event)
{
    switch (key_event)
    {
    case KEY_CLICK:
        if (key_name == KEY_K1)
        {
            if (drv_me_pt->tcp_connection_status == 1)
            {
                thread_cslock_lock(drv_me_pt->lock, MaxTick);
                char mytaskstatebuffer[500];
                vTaskList((char *)&mytaskstatebuffer);
                WIFI_Usart("%s\r\n", mytaskstatebuffer);
                vTaskDelay(50);
                thread_cslock_free(drv_me_pt->lock);
            }
        }
        else if (key_name == KEY_S1)
        {
            drv_moto_pt->request_status = Lock_Request;
            drv_me_pt->set_send_cnt(0);

            drv_voice_pt->lock_request();
        }

        break;
    case KEY_LONG_PRESS:
        if (key_name == KEY_K1)
        {
            drv_moto_pt->request_status = UnLock_Request;
            // thread_cslock_lock(drv_me_pt->lock, MaxTick);
            drv_me_pt->set_send_cnt(0);
            // thread_cslock_free(drv_me_pt->lock);
            drv_voice_pt->unlock_request();
            // drv_height_pt->set_height_zero();

            // drv_height_pt->set_height_zero();

            // u32 isSetZero = 1;
            // xTaskNotify(drv_height_pt->set_height_zero, isSetZero, eSetValueWithOverwrite);
        }
        else if (key_name == KEY_S1)
        {
            drv_flash_pt->read_from_flash();

            if (drv_flash_pt->permissions_t.height_check)
            {
                thread_create(drv_height_pt->set_height_zero, "set_height_zero", 128, NULL, 3, NULL);
            }
        }

        break;
    case KEY_DOUBLE_CLICK:

        if (key_name == KEY_K1)
        {
            drv_moto_pt->sos = 1;
            drv_me_pt->set_send_cnt(0);
            drv_voice_pt->sos();
        }
        else if (key_name == KEY_S1)
        {
            drv_flash_pt->read_from_flash();

            if (drv_flash_pt->permissions_t.emergency_unclock)
            {
                drv_moto_pt->emer_unlock = 1;
                drv_me_pt->set_send_cnt(0);

                drv_voice_pt->unlock_finish();
            }
        }

        break;

    default:
        break;
    }
}

void key_proc(void)
{
    uint8_t key;
    KEY_EVENT key_event;

    const EventBits_t xBitsToWaitFor = TCP_TASK_BIT;

    while (1)
    {
        /* code */
        /* 获取事件位 */
        EventBits_t xEventGroupValue = xEventGroupWaitBits(drv_me_pt->tcp_group_handle, /* 事件组的句柄 */
                                                           xBitsToWaitFor,              /* 待测试的事件位 */
                                                           pdFALSE,                     /* 满足添加时清除上面的事件位 */
                                                           pdFALSE,                     /* 任意事件位被设置就会退出阻塞态 */
                                                           portMAX_DELAY);              /* 没有超时 */
        /* 根据相应的事件位输出提示信息 */
        if ((xEventGroupValue & TCP_TASK_BIT) != 0)
        {
            /* code */

            for (uint8_t i = 0; i < KEY_SUM; i++)
            {
                key = 0;
                key_event = 0;
                key_propertys[i].key_status = get_key_state[i]();

                switch (key_propertys[i].key_step)
                {
                case KEY_STEP_WAIT:
                    if (key_propertys[i].key_status)
                    {
                        key_propertys[i].key_step = KEY_STEP_CLICK;
                    }
                    break;
                case KEY_STEP_CLICK:
                    if (key_propertys[i].key_status)
                    {
                        if (!(--(key_propertys[i].scan_time)))
                        {
                            key_propertys[i].scan_time = 2;
                            key_propertys[i].key_step = KEY_STEP_LONG_PRESS;
                            // key = i + 1;
                            // key_event = KEY_CLICK;
                        }
                    }
                    else
                    {
                        key_propertys[i].scan_time = 2;
                        key_propertys[i].key_step = KEY_STEP_WAIT;
                    }
                    break;

                case KEY_STEP_LONG_PRESS:

                    if (key_propertys[i].key_status)
                    {
                        if (!(--(key_propertys[i].press_long_time)))
                        {
                            key_propertys[i].press_long_time = 300;
                            key_propertys[i].key_step = KEY_STEP_CONTINUOUS_PRESS;

                            key = i + 1;
                            key_event = KEY_LONG_PRESS;
                        }
                    }
                    else
                    {
                        key_propertys[i].press_long_time = 300;
                        key_propertys[i].key_step = KEY_STEP_CLICK_RELEASE;

                        // key = i + 1;
                        // key_event = KEY_CLICK_RELEASE;
                        // key_event = KEY_CLICK;
                    }
                    break;
                case KEY_STEP_CLICK_RELEASE:

                    if (key_propertys[i].key_status)
                    {
                        if ((--(key_propertys[i].key_double_click_time)))
                        {
                            key_propertys[i].key_double_click_time = 20;
                            key_propertys[i].key_step = KEY_STEP_WAIT;

                            key = i + 1;
                            key_event = KEY_DOUBLE_CLICK;
                        }
                    }
                    else
                    {
                        if (--(key_propertys[i].key_double_click_time) == 0)
                        {
                            key_propertys[i].key_double_click_time = 20;
                            key_propertys[i].key_step = KEY_STEP_WAIT;
                            key = i + 1;
                            key_event = KEY_CLICK;
                        }
                    }

                    break;

                case KEY_STEP_CONTINUOUS_PRESS:
                    if (key_propertys[i].key_status)
                    {
                        if (!(--(key_propertys[i].press_continuous_long_time)))
                        {
                            key_propertys[i].press_continuous_long_time = 15;

                            key = i + 1;
                            key_event = KEY_LONG_PRESS_CONTINUOUS;
                        }
                    }
                    else
                    {
                        key_propertys[i].key_step = KEY_STEP_WAIT;
                        key_propertys[i].press_continuous_long_time = 15;

                        key = i + 1;
                        key_event = KEY_LONG_PRESS_RELEASE;
                    }

                    break;

                default:
                    break;
                }

                if (key)
                {
                    //                if (key_callback)
                    //                {
                    // key_callback(key - 1, key_event);
                    key_handle(key - 1, key_event);
                    //                }
                }
            }

            vTaskDelay(10);
        }
    }
}

void register_callback(KEY_CALLBACK key_callback_t)
{
    if (key_callback == 0)
    {
        key_callback = key_callback_t;
    }
}
