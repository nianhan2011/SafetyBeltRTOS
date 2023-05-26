#include "motoControl.h"

Drv_Moto_t drv_moto_t = {
	.device_status = Device_Status_Lock,
	.request_status = No_Request,

};

Drv_Moto_pt drv_moto_pt;

void init_moto(void)
{
	drv_moto_pt = &drv_moto_t;



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
