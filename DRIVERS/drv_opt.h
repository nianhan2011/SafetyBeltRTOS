
#ifndef DRV_OPT_H
#define DRV_OPT_H

#include "stm32f10x.h"
#include "os_system__typedef.h"

// drv_timer_tick
/**************通用定时器TIM参数定义，只限TIM2、3、4、5************/
#define GENERAL_TIM TIM2
#define GENERAL_TIM_APBxClock_FUN RCC_APB1PeriphClockCmd
#define GENERAL_TIM_CLK RCC_APB1Periph_TIM2
#define GENERAL_TIM_Period (10000 - 1) //10ms
#define GENERAL_TIM_Prescaler 71
#define GENERAL_TIM_IRQ TIM2_IRQn
#define GENERAL_TIM_IRQHandler TIM2_IRQHandler

typedef struct
{
    time_t (*get)(void);
    void (*delay)(time_t tick);
} systick_t, *systick_pt;

extern systick_pt timtick;
extern void init_drv_timer_tick(void);

// drv_height
typedef struct
{
    u8 buff[40];
    u8 buff_length;
    float height;
    thread_cslock_t lock;
    void (*get_barometric)(void);
    void (*set_height_zero)(void);

} Drv_Height_t, *Drv_Height_pt;

extern Drv_Height_pt drv_height_pt;


#endif /* TIME_TEST_H */
