#ifndef __KEY_DRV_H
#define __KEY_DRV_H
#include <stdint.h>

typedef enum
{
    KEY_K1,
    KEY_S1,
    KEY_SUM
} KEY_NAME;

typedef enum
{
    KEY_STEP_WAIT,
    KEY_STEP_CLICK,
    KEY_STEP_LONG_PRESS,
    KEY_STEP_CONTINUOUS_PRESS,
} KEY_STEP;

typedef enum
{
    KEY_IDLE,
    KEY_CLICK,
    KEY_CLICK_RELEASE,
    KEY_LONG_PRESS,
    KEY_LONG_PRESS_CONTINUOUS,
    KEY_LONG_PRESS_RELEASE,
} KEY_EVENT;

typedef struct
{
    KEY_STEP key_step;
    uint16_t scan_time;
    uint16_t press_long_time;
    uint16_t press_continuous_long_time;
    uint8_t key_status;
} KEY_PROPERTY;

typedef void (*KEY_CALLBACK)(KEY_NAME key_name, KEY_EVENT key_event);

void key_init(void);
void key_proc(void);

#endif
