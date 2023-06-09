#include "motoControl.h"
#include "Common.h"
#include "drv_me.h"
void moto_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); // 使能A端口时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; // D00
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // 速度50MHz
	GPIO_Init(GPIOC, &GPIO_InitStructure);			  // 初始化GPIOA

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15; // D01
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // 速度50MHz
	GPIO_Init(GPIOC, &GPIO_InitStructure);			  // 初始化GPIOA

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14; // D02
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // 速度50MHz
	GPIO_Init(GPIOC, &GPIO_InitStructure);			  // 初始化GPIOA

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13; // D03
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	// GPIO_SetBits(GPIOA,GPIO_Pin_8);
	//     GPIO_ResetBits(GPIOA,GPIO_Pin_11);
}

void get_status(void)
{
	while (1)
	{
		drv_moto_pt->pc0 = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_0);
		drv_moto_pt->pc15 = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_15);
		drv_moto_pt->pc14 = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_14);
		drv_moto_pt->pc13 = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13);

		// if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 0) // 读取电平

		// if (drv_moto_pt->pc0 == 0 && drv_moto_pt->pc15 == 1 && drv_moto_pt->pc14 == 1 && drv_moto_pt->pc13 == 0)
		// {
		// 	vTaskDelay(20);
		// 	if (drv_moto_pt->pc0 == 0 && drv_moto_pt->pc15 == 1 && drv_moto_pt->pc14 == 1 && drv_moto_pt->pc13 == 0)
		// 	{
		// 		drv_moto_pt->device_status = Device_Status_Lock;
		// 		drv_me_pt->set_send_cnt(0);
		// 	}

		// }

		// if (drv_moto_pt->pc0 == 0 &&drv_moto_pt->pc15 == 0 &&drv_moto_pt->pc14 == 1 &&drv_moto_pt->pc13 == 0 &&drv_moto_pt->device_status == Device_Status_Lock)
		// {
		// 	drv_moto_pt->device_status = Device_Status_UnLock;//chongfu
		// 	drv_me_pt->set_send_cnt(0);
		// }

		// 锁扣 + 水银状态
		// C1 W10
		if (drv_moto_pt->pc0 == 0 && drv_moto_pt->pc15 == 1 && drv_moto_pt->pc14 == 1 && drv_moto_pt->pc13 == 1)
		{ 
			if (drv_moto_pt->zitai_status != Zitai_Status_Safe &&drv_moto_pt->device_status != Device_Status_Lock)
			{
				drv_moto_pt->attitude_warn = 1;
				drv_moto_pt->zitai_status = Zitai_Status_Safe;
				drv_moto_pt->device_status = Device_Status_Lock;
				drv_me_pt->set_send_cnt(0);
			}
		}
 
		// 02 W11  0 1 1 0
		if (drv_moto_pt->pc0 == 0 && drv_moto_pt->pc15 == 1 && drv_moto_pt->pc14 == 1 && drv_moto_pt->pc13 == 0)
		{
			if (drv_moto_pt->zitai_status != Zitai_Status_UnSafe &&drv_moto_pt->device_status != Device_Status_UnLock)
			{
				drv_moto_pt->attitude_warn = 1;
				drv_moto_pt->zitai_status = Zitai_Status_UnSafe;
				drv_moto_pt->device_status = Device_Status_UnLock;
				drv_me_pt->set_send_cnt(0);
			}
		}

		// 02 W11   0 0 1 0

		if (drv_moto_pt->pc0 == 0 && drv_moto_pt->pc15 == 0 && drv_moto_pt->pc14 == 1 && drv_moto_pt->pc13 == 0)
		{
			if (drv_moto_pt->zitai_status != Zitai_Status_UnSafe && drv_moto_pt->device_status != Device_Status_UnLock)
			{
				drv_moto_pt->attitude_warn = 1;
				drv_moto_pt->zitai_status = Zitai_Status_UnSafe;
				drv_moto_pt->device_status = Device_Status_UnLock;
				drv_me_pt->set_send_cnt(0);
			}
		}

		// 02 W11   0 0 1 1

		if (drv_moto_pt->pc0 == 0 && drv_moto_pt->pc15 == 0 && drv_moto_pt->pc14 == 1 && drv_moto_pt->pc13 == 1)
		{
			if (drv_moto_pt->zitai_status != Zitai_Status_UnSafe && drv_moto_pt->device_status != Device_Status_UnLock)
			{
				drv_moto_pt->attitude_warn = 1;
				drv_moto_pt->zitai_status = Zitai_Status_UnSafe;
				drv_moto_pt->device_status = Device_Status_UnLock;
				drv_me_pt->set_send_cnt(0);
			}
		}

		// 腰扣两种状态

		if (drv_moto_pt->pc0 == 1 && drv_moto_pt->pc15 == 1 && drv_moto_pt->pc14 == 0 && drv_moto_pt->pc13 == 0)
		{
			if (drv_moto_pt->yaokou_status != Yaokou_Status_UnLock)
			{
				drv_moto_pt->snap_warn = 1;
				drv_moto_pt->yaokou_status = Yaokou_Status_UnLock;
				drv_me_pt->set_send_cnt(0);
			}
		}

		if (drv_moto_pt->pc0 == 1 && drv_moto_pt->pc15 == 0 && drv_moto_pt->pc14 == 0 && drv_moto_pt->pc13 == 0)
		{
			if (drv_moto_pt->yaokou_status != Yaokou_Status_Lock)
			{
				drv_moto_pt->snap_warn = 1;
				drv_moto_pt->yaokou_status = Yaokou_Status_Lock;
				drv_me_pt->set_send_cnt(0);
			}
		}
	}
}

Drv_Moto_t drv_moto_t = {
	.device_status = Device_Status_Lock,
	.yaokou_status = Yaokou_Status_Lock,
	.zitai_status = Zitai_Status_Safe,
	// .request_status = No_Request,
	// .attitude_error = 0,
	// .attitude_normal = 0,
	// .snap_error = 0,
	// .snap_normal = 0,

	.attitude_warn = 0,
	.snap_warn = 0,

	.fatigure_warn = 0,
	.fail_warn = 0,
	.sos = 0,
	.height_warn = 0,
	.get_status = get_status,

};

Drv_Moto_pt drv_moto_pt;

void init_moto(void)
{
	drv_moto_pt = &drv_moto_t;
	moto_init();
}

// void motoLock() {
// 	PBout(4) = 1;
// 	PBout(3) = 0;
// 	moto_start = 1;

// }

// void motoUnLock() {
// 	PBout(4) = 0;
// 	PBout(3) = 1;
// 	moto_start = 1;

// }

// void motoOFF() {
// 	PBout(4) = 0;
// 	PBout(3) = 0;
// }
