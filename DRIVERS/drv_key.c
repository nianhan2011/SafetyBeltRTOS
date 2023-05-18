#include "drv_key.h"
#include "Common.h"
#include "os_system__typedef.h"
#include "drv_opt.h"
#include "drv_voice.h"

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
    }
}

void key_handle(KEY_NAME key_name, KEY_EVENT key_event)
{
    switch (key_event)
    {
    case KEY_CLICK:
        if (key_name == KEY_K1)
        {
        }
        else if (key_name == KEY_S1)
        {
            drv_voice_pt->yyhy();
            
        }

        break;
    case KEY_LONG_PRESS:
        if (key_name == KEY_K1)
        {
            // drv_height_pt->set_height_zero();

            // drv_height_pt->set_height_zero();
            thread_create(drv_height_pt->set_height_zero, "set_height_zero", 1024, NULL, 3, NULL);

            // u32 isSetZero = 1;
            // xTaskNotify(drv_height_pt->set_height_zero, isSetZero, eSetValueWithOverwrite);
        }
        else if (key_name == KEY_S1)
        {
            u8 tag = 4;
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

    while (1)
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
                        key = i + 1;
                        key_event = KEY_CLICK;
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
                    key_propertys[i].key_step = KEY_STEP_WAIT;

                    key = i + 1;
                    key_event = KEY_CLICK_RELEASE;
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

void register_callback(KEY_CALLBACK key_callback_t)
{
    if (key_callback == 0)
    {
        key_callback = key_callback_t;
    }
}
