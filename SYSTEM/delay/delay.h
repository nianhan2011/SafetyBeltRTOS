#ifndef __DELAY_H
#define __DELAY_H
#include "stdio.h"	
#include "common.h" 
//当使用ucos的时候,此函数会初始化ucos的时钟节拍
//SYSTICK的时钟固定为HCLK时钟的1/8
//SYSCLK:系统时钟
void delay_init(void);						    

//延时nus
//nus为要延时的us数.		    								   
// void delay_us(u32 nus);
//延时nms
//nms:要延时的ms数
// void delay_ms(u16 nms);

#endif













