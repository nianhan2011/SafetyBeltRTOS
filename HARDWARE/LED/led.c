#include "led.h"
#include "FreeRTOS.h"
#include "task.h"
void LED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE); // ʹ��A�˿�ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE); // ʹ��A�˿�ʱ��

	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  // �������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // �ٶ�50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);			  // ��ʼ��GPIOA
	GPIO_SetBits(GPIOA, GPIO_Pin_1);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  // �������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // �ٶ�50MHz
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	// GPIO_SetBits(GPIOC, GPIO_Pin_4);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  // �������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // �ٶ�50MHz
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	// GPIO_SetBits(GPIOC, GPIO_Pin_5);

	// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	// GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  // �������
	// GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // �ٶ�50MHz
	// GPIO_Init(GPIOB, &GPIO_InitStructure);
	// // GPIO_SetBits(GPIOB, GPIO_Pin_0);

	// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	// GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  // �������
	// GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // �ٶ�50MHz
	// GPIO_Init(GPIOB, &GPIO_InitStructure);
	// GPIO_SetBits(GPIOB, GPIO_Pin_1);
}

void led_turn(void)  
{
	while (1)
	{
		/* code */

		PAout(1) = !PAout(1);
		vTaskDelay(500); // 丢弃剩余的时间片资源
	}
}
