#ifndef __TIMER_H
#define __TIMER_H
#include "common.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ���������ɣ��������������κ���;
//ALIENTEK Mini STM32������
//ͨ�ö�ʱ�� ��������			   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2010/12/03
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ����ԭ�� 2009-2019
//All rights reserved
////////////////////////////////////////////////////////////////////////////////// 	  
typedef struct                                             //��������֡�Ĵ����ṹ��
{
	__IO uint8_t Time_CNT;
	__IO uint8_t Finish_Flag;
} Time_Flag;
extern __IO Time_Flag ReceiveMSG_Flag;

void TIM3_Int_Init(u16 arr,u16 psc); 
 
#endif
