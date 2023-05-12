#include "led.h"
#include "os_system__typedef.h"

void LED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE); // 使能A端口时钟
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

	//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	// 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50MHz
	// 	GPIO_Init(GPIOA, &GPIO_InitStructure);	  //初始化GPIOA

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  // 推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // 速度50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);			  // 初始化GPIOA

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB, GPIO_Pin_13);
	//	PAout(0) = 0;
	PAout(1) = 1;
}

void turn_led(void)
{
	while (1)
	{
		/* code */

		PAout(1) = !PAout(1);
		vTaskDelay(500);
	}
}
