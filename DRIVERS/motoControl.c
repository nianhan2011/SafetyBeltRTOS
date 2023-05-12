#include "motoControl.h"


uint8_t moto_start = 0;
uint8_t moto_start_cnt = 0;

void MOTO_Init(void)
{
	
	GPIO_InitTypeDef GPIO_InitStructure;

 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//ʹ��PORTB,PORTCʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4;//PB4
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //���ó��������
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_3;//PB3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //���ó��������
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_12;//PA12
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //���ó��������
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	PBout(3) = 1;  
	PBout(4) = 1;  
	
	PAout(12) = 0;


}

void motoLock() {
	PBout(4) = 1;
	PBout(3) = 0;
	moto_start = 1;

}

void motoUnLock() {
	PBout(4) = 0;
	PBout(3) = 1;
	moto_start = 1;

}

void motoOFF() {
	PBout(4) = 0;
	PBout(3) = 0;
}


