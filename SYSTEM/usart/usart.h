#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "common.h" 

#define USART_REC_LEN  			200  	//锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟街斤拷锟斤拷 200
#define EN_USART1_RX 			1		//使锟杰ｏ拷1锟斤拷/锟斤拷止锟斤拷0锟斤拷锟斤拷锟斤拷1锟斤拷锟斤拷
	  	
extern u8  USART_RX_BUF[USART_REC_LEN]; //锟斤拷锟秸伙拷锟斤拷,锟斤拷锟経SART_REC_LEN锟斤拷锟街斤拷.末锟街斤拷为锟斤拷锟叫凤拷 
extern u16 USART_RX_STA;         		//锟斤拷锟斤拷状态锟斤拷锟�	
//锟斤拷锟斤拷氪拷锟斤拷卸辖锟斤拷眨锟斤拷氩灰拷锟斤拷锟斤拷潞甓拷锟�
void uart_init(u32 bound);
#endif


