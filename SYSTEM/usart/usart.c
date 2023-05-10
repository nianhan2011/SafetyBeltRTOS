#include "common.h"
#include "usart.h"
//////////////////////////////////////////////////////////////////////////////////
// 锟斤拷锟绞癸拷锟絬cos,锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷头锟侥硷拷锟斤拷锟斤拷.
#if SYSTEM_SUPPORT_UCOS
#include "includes.h" //ucos 使锟斤拷
#endif

#include <stdio.h>
#include <string.h>

//////////////////////////////////////////////////////////////////
// 锟斤拷锟斤拷锟斤拷锟铰达拷锟斤拷,支锟斤拷printf锟斤拷锟斤拷,锟斤拷锟斤拷锟斤拷要选锟斤拷use MicroLIB
#if 1
#pragma import(__use_no_semihosting)
// 锟斤拷准锟斤拷锟斤拷要锟斤拷支锟街猴拷锟斤拷
struct __FILE
{
	int handle;
};

FILE __stdout;
// 锟斤拷锟斤拷_sys_exit()锟皆憋拷锟斤拷使锟矫帮拷锟斤拷锟斤拷模式
void _sys_exit(int x)
{
	x = x;
}
// 锟截讹拷锟斤拷fputc锟斤拷锟斤拷
int fputc(int ch, FILE *f)
{
	while ((USART1->SR & 0X40) == 0)
		; // 循锟斤拷锟斤拷锟斤拷,直锟斤拷锟斤拷锟斤拷锟斤拷锟�
	USART1->DR = (u8)ch;
	return ch;
}
#endif

/*使锟斤拷microLib锟侥凤拷锟斤拷*/
/*
int fputc(int ch, FILE *f)
{
   USART_SendData(USART1, (uint8_t) ch);

   while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}

   return ch;
}
int GetKey (void)  {

   while (!(USART1->SR & USART_FLAG_RXNE));

   return ((int)(USART1->DR & 0x1FF));
}
*/

#if EN_USART1_RX // 锟斤拷锟绞癸拷锟斤拷私锟斤拷锟�
// 锟斤拷锟斤拷1锟叫断凤拷锟斤拷锟斤拷锟�
// 注锟斤拷,锟斤拷取USARTx->SR锟杰憋拷锟斤拷莫锟斤拷锟斤拷锟斤拷拇锟斤拷锟�
u8 USART_RX_BUF[USART_REC_LEN]; // 锟斤拷锟秸伙拷锟斤拷,锟斤拷锟経SART_REC_LEN锟斤拷锟街斤拷.
// 锟斤拷锟斤拷状态
// bit15锟斤拷	锟斤拷锟斤拷锟斤拷杀锟街�
// bit14锟斤拷	锟斤拷锟秸碉拷0x0d
// bit13~0锟斤拷	锟斤拷锟秸碉拷锟斤拷锟斤拷效锟街斤拷锟斤拷目
u16 USART_RX_STA = 0; // 锟斤拷锟斤拷状态锟斤拷锟�

void uart_init(u32 bound)
{
	// GPIO锟剿匡拷锟斤拷锟斤拷
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE); // 使锟斤拷USART1锟斤拷GPIOA时锟斤拷锟皆硷拷锟斤拷锟矫癸拷锟斤拷时锟斤拷
																										// USART1_TX   PA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; // PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // 锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟�
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// USART1_RX	  PA.10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 锟斤拷锟斤拷锟斤拷锟斤拷
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// Usart1 NVIC 锟斤拷锟斤拷

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; // 锟斤拷占锟斤拷锟饺硷拷3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		  // 锟斤拷锟斤拷锟饺硷拷3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  // IRQ通锟斤拷使锟斤拷
	NVIC_Init(&NVIC_InitStructure);							  // 锟斤拷锟斤拷指锟斤拷锟侥诧拷锟斤拷锟斤拷始锟斤拷VIC锟侥达拷锟斤拷

	// USART 锟斤拷始锟斤拷锟斤拷锟斤拷

	USART_InitStructure.USART_BaudRate = bound;										// 一锟斤拷锟斤拷锟斤拷为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						// 锟街筹拷为8位锟斤拷锟捷革拷式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							// 一锟斤拷停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;								// 锟斤拷锟斤拷偶校锟斤拷位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 锟斤拷硬锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					// 锟秸凤拷模式

	USART_Init(USART1, &USART_InitStructure);	   // 锟斤拷始锟斤拷锟斤拷锟斤拷
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); // 锟斤拷锟斤拷锟叫讹拷
	USART_Cmd(USART1, ENABLE);					   // 使锟杰达拷锟斤拷
}

void USART1_IRQHandler(void) // 锟斤拷锟斤拷1锟叫断凤拷锟斤拷锟斤拷锟�
{
	u8 Res;

	char str1[6] = "12345\0";
	char str2[6] = "12345\0";

	char *s3 = strcat(str1, str2);

#ifdef OS_TICKS_PER_SEC // 锟斤拷锟绞憋拷咏锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟�,说锟斤拷要使锟斤拷ucosII锟斤拷.
	OSIntEnter();
#endif
	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) // 锟斤拷锟斤拷锟叫讹拷(锟斤拷锟秸碉拷锟斤拷锟斤拷锟捷憋拷锟斤拷锟斤拷0x0d 0x0a锟斤拷尾)
	{
		Res = USART_ReceiveData(USART1); //(USART1->DR);	//锟斤拷取锟斤拷锟秸碉拷锟斤拷锟斤拷锟斤拷

		if ((USART_RX_STA & 0x8000) == 0) // 锟斤拷锟斤拷未锟斤拷锟�
		{
			if (USART_RX_STA & 0x4000) // 锟斤拷锟秸碉拷锟斤拷0x0d
			{
				if (Res != 0x0a)
					USART_RX_STA = 0; // 锟斤拷锟秸达拷锟斤拷,锟斤拷锟铰匡拷始
				else
					USART_RX_STA |= 0x8000; // 锟斤拷锟斤拷锟斤拷锟斤拷锟�
			}
			else // 锟斤拷没锟秸碉拷0X0D
			{
				if (Res == 0x0d)
					USART_RX_STA |= 0x4000;
				else
				{
					USART_RX_BUF[USART_RX_STA & 0X3FFF] = Res;
					USART_RX_STA++;
					if (USART_RX_STA > (USART_REC_LEN - 1))
						USART_RX_STA = 0; // 锟斤拷锟斤拷锟斤拷锟捷达拷锟斤拷,锟斤拷锟铰匡拷始锟斤拷锟斤拷
				}
			}
		}
	}
#ifdef OS_TICKS_PER_SEC // 锟斤拷锟绞憋拷咏锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟�,说锟斤拷要使锟斤拷ucosII锟斤拷.
	OSIntExit();
#endif
}
#endif
