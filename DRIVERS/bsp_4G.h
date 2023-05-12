// #ifndef  __BSP_4G_H
// #define	 __BSP_4G_H



// #include "stm32f10x.h"
// #include "common.h"
// #include <stdio.h>
// #include <stdbool.h>



// #if defined ( __CC_ARM   )
// #pragma anon_unions
// #endif

// /******************************** 4G 连接引脚定义 ***********************************/
// #define      RCC_4G_GPIO_ClockCmd                             RCC_APB2PeriphClockCmd
// #define      RCC_USART3_GPIO_Periph                               RCC_APB2Periph_GPIOA
// #define      PORT_USART3_TX                                       GPIOA
// #define      PIN_USART3_TX                                        GPIO_Pin_2
// #define      PORT_USART3_RX                                       GPIOA
// #define      PIN_USART3_RX                                        GPIO_Pin_3

// #define      RCC_USART3_ClockCmd                            RCC_APB1PeriphClockCmd
// #define      RCC_USART3_Periph                              RCC_APB1Periph_USART2  

// #define      USART_4G                                         USART2  
 

// #define      USART_BAUD_RATE                               115200

// #define      USART_3_IRQn                                    USART2_IRQn
// #define      USART_4G_IRQnHandler                             USART2_IRQHandler

// #define      WIFI_Usart( fmt, ... )                           USART_printf ( USART_4G, fmt, ##__VA_ARGS__ ) 






// //typedef struct                                   //串口数据帧的处理结构体
// //{
// //	int Pressure_Data;
// //	
// //   	int Height_Data;

// //	
// //} Struct_Barometric_Data;

// extern usart3_fram USART_4G_Fram_Instance;
// extern _Bool TCP_Is_Connect;
// void USART_4G_Init ( void );

// //void Start_4G_TCP(void);


// bool ME_GetIMEI (void);

// bool GPRS_Connect (void);
// bool Open_Call(void);

// bool TCP_Connect(void);
// void TCP_Send(char *message);
// void Start_ME(void);
// void ReceiveString (void);
// #endif
 

