#ifndef __MOTO_H
#define __MOTO_H

#include "sys.h"

void MOTO_Init(void);
void motoLock(void);
void motoUnLock(void);
void motoOFF(void);

extern uint8_t moto_start;
extern uint8_t moto_start_cnt;



#endif


