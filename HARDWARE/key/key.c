#include "key.h"
#include "Common.h"

static uint8_t get_key1_state(void);
static uint8_t get_key2_state(void);
static uint8_t get_key3_state(void);

static void key_gpio_init(void);

uint8_t (*get_key_state[KEY_SUM])() = {get_key1_state, get_key2_state, get_key3_state};
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

void key_proc(void)
{
    uint8_t key;
    KEY_EVENT key_event;

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
                else
                {
                    key_propertys[i].scan_time = 2;
                    key_propertys[i].key_step = KEY_STEP_WAIT;
                }
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
            if (key_callback)
            {
                key_callback(key - 1, key_event);
            }
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
uint8_t get_key1_state(void)
{
}
uint8_t get_key2_state(void)
{
}
uint8_t get_key3_state(void)
{
}

void key_gpio_init(void)
{
}
