#include "led.h"
#include "os_system__typedef.h"
#include "drv_me.h"

void led_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC| RCC_APB2Periph_AFIO, ENABLE); // ʹ��A�˿�ʱ��
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

	//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	// 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//�ٶ�50MHz
	// 	GPIO_Init(GPIOA, &GPIO_InitStructure);	  //��ʼ��GPIOA

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  // �������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // �ٶ�50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);			  // ��ʼ��GPIOA

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  // �������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // �ٶ�50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);			  // ��ʼ��GPIOA

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  // �������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // �ٶ�50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);			  // ��ʼ��GPIOA

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  // �������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // �ٶ�50MHz
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


			// /* ��ȡ�¼�λ */
			// xEventGroupValue = xEventGroupWaitBits(drv_me_pt->tcp_group_handle,	   /* �¼���ľ�� */
			// 									   xBitsToWaitFor, /* �����Ե��¼�λ */
			// 									   pdFALSE,		   /* �������ʱ���������¼�λ */
			// 									   pdFALSE,		   /* �����¼�λ�����þͻ��˳�����̬ */
			// 									   portMAX_DELAY); /* û�г�ʱ */
			// /* ������Ӧ���¼�λ�����ʾ��Ϣ */
			// if ((xEventGroupValue & TCP_TASK_BIT) == 0) continue;
			
			PAout(1) = !PAout(1);
			vTaskDelay(500);

			
			// PAout(1) = !PAout(1);
			// vTaskDelay(500);

			/* code */

	}
}
