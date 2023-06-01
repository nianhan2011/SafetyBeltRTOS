#include "motoControl.h"
#include "Common.h"
void moto_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC , ENABLE); // 使能A端口时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;//D00
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // 速度50MHz
	GPIO_Init(GPIOC, &GPIO_InitStructure);			  // 初始化GPIOA

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;//D01
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // 速度50MHz
	GPIO_Init(GPIOC, &GPIO_InitStructure);			  // 初始化GPIOA

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;//D02
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // 速度50MHz
	GPIO_Init(GPIOC, &GPIO_InitStructure);			  // 初始化GPIOA

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;//D03
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);


	// GPIO_SetBits(GPIOA,GPIO_Pin_8);
	//     GPIO_ResetBits(GPIOA,GPIO_Pin_11);
}

Drv_Moto_t drv_moto_t = {
	.device_status = Device_Status_Lock,
	.request_status = No_Request,
	.attitude_error = 0,
	.attitude_normal = 0,
	.snap_error = 0,
	.snap_normal = 0,
	.fatigure_warn = 0,
	.fail_warn = 0,
	.sos = 0,

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
