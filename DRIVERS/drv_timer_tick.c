#include "drv_opt.h"
#include "os_system__typedef.h"

/*-------------------------------------------------------------------------------------------------*/

// 中断优先级配置
static void GENERAL_TIM_NVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure; 
	// 设置中断组为0
	// NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);		
	// 设置中断来源
	NVIC_InitStructure.NVIC_IRQChannel = GENERAL_TIM_IRQ ;	
	/* 抢断优先级*/
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;
	/* 子优先级 */
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	/* 使能中断 */
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	/* 初始化配置NVIC */
	NVIC_Init(&NVIC_InitStructure);
}
static void GENERAL_TIM_Mode_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;		
	// 开启定时器时钟,即内部时钟CK_INT=72M
	GENERAL_TIM_APBxClock_FUN(GENERAL_TIM_CLK, ENABLE);	
	// 自动重装载寄存器的值，累计TIM_Period+1个频率后产生一个更新或者中断
	TIM_TimeBaseStructure.TIM_Period=GENERAL_TIM_Period;
	// 时钟预分频数
	TIM_TimeBaseStructure.TIM_Prescaler= GENERAL_TIM_Prescaler;	
	// 时钟分频因子 ，没用到不用管
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;		
	// 计数器计数模式，设置为向上计数
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 		
	// 重复计数器的值，没用到不用管
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;	
	// 初始化定时器
	TIM_TimeBaseInit(GENERAL_TIM, &TIM_TimeBaseStructure);

	// 清除计数器中断标志位
	TIM_ClearFlag(GENERAL_TIM, TIM_FLAG_Update);

	// 开启计数器中断
	TIM_ITConfig(GENERAL_TIM,TIM_IT_Update,ENABLE);

	// 使能计数器
	TIM_Cmd(GENERAL_TIM, ENABLE);
}

void GENERAL_TIM_Init(void)
{
	GENERAL_TIM_NVIC_Config();
	GENERAL_TIM_Mode_Config();		
}



/*-------------------------------------------------------------------------------------------------*/
static time_t gSystick = 0;
static thread_cslock_t cslock;

void add_systick(void)
{
	gSystick++;
}

void GENERAL_TIM_IRQHandler(void)
{
	if (TIM_GetITStatus(GENERAL_TIM, TIM_IT_Update) != RESET)
	{
		add_systick();
		TIM_ClearITPendingBit(GENERAL_TIM, TIM_FLAG_Update);
	}
}
//给内部drv组件使用的接口，无需再头文件作外部声明，应用程序不可见
time_t drv_get_systick(void)
{
	time_t tick=0;
//	thread_cslock_lock(cslock,MaxTick);
	tick = gSystick;
//	thread_cslock_free(cslock);
	return tick;
}

//给内部drv组件使用的接口，无需再头文件作外部声明，应用程序不可见
time_t drv_get_systick_isr(void)
{
	time_t tick=0;
	tick = gSystick;
	return tick;
}



static time_t get_systick(void)
{
	time_t tick=0;
	//32bit，原子操作，不需要上锁
//	thread_cslock_lock(cslock,MaxTick);
	tick = gSystick;
//	thread_cslock_free(cslock);
	return tick;
	
}


static void delay(time_t tick)
{
	time_t oldtick = get_systick();
	while(1)
	{
		if( tick <( get_systick() - oldtick ) )
			break;
	}

}



/*-------------------------------------------------------------------------------------------------*/
static systick_t do_timer_tick;
systick_pt timtick;
void init_drv_timer_tick(void)
{
	timtick = &do_timer_tick;
	timtick->get = get_systick;
	timtick->delay = delay;

	gSystick = 0;
	cslock = thread_cslock_init("timer_tick");
	
	GENERAL_TIM_Init();
	
	
}












