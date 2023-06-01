#include "led.h"
#include "os_system__typedef.h"
#include "drv_me.h"

void led_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC| RCC_APB2Periph_AFIO, ENABLE); // 使能A端口时钟
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

	//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	// 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50MHz
	// 	GPIO_Init(GPIOA, &GPIO_InitStructure);	  //初始化GPIOA

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  // 推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // 速度50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);			  // 初始化GPIOA

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  // 推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // 速度50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);			  // 初始化GPIOA

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  // 推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // 速度50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);			  // 初始化GPIOA

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  // 推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // 速度50MHz
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB, GPIO_Pin_13);
	//	PAout(0) = 0;
	PAout(1) = 1;
	PAout(8) = 1;
	PAout(11) = 1;

	PCout(8) = 0;

	// GPIO_SetBits(GPIOA,GPIO_Pin_8);
	//     GPIO_ResetBits(GPIOA,GPIO_Pin_11);

	
}

void turn_led(void)
{
	EventBits_t xEventGroupValue;
	const EventBits_t xBitsToWaitFor = TCP_TASK_BIT;
	while (1)
	{


			// /* 获取事件位 */
			// xEventGroupValue = xEventGroupWaitBits(drv_me_pt->tcp_group_handle,	   /* 事件组的句柄 */
			// 									   xBitsToWaitFor, /* 待测试的事件位 */
			// 									   pdFALSE,		   /* 满足添加时清除上面的事件位 */
			// 									   pdFALSE,		   /* 任意事件位被设置就会退出阻塞态 */
			// 									   portMAX_DELAY); /* 没有超时 */
			// /* 根据相应的事件位输出提示信息 */
			// if ((xEventGroupValue & TCP_TASK_BIT) == 0) continue;
			
			PAout(1) = !PAout(1);
			vTaskDelay(500);

			
			// PAout(1) = !PAout(1);
			// vTaskDelay(500);

			/* code */

	}
}
