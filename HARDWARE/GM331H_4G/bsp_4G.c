// #include "bsp_4G.h"
// #include "common.h"
// #include <stdio.h>  
// #include <string.h>  
// #include <stdbool.h>
// #include "delay.h"

// static void                   USART_GPIO_Config                 ( void );
// static void                   USART_USART_Config                ( void );
// static void                   USART_4G_NVIC_Configuration          ( void );

// usart3_fram USART_Fram_Instance = {0};
// _Bool TCP_Is_Connect = false;
// /**
//   * @brief  初始化函数
//   * @param  无
//   * @retval 无
//   */
// void USART_4G_Init ( void )
// {
// 	USART_GPIO_Config (); 
	
// 	USART_USART_Config (); 
	
// }


// /**
//   * @brief  初始化usart用到的GPIO引脚
//   * @param  无
//   * @retval 无
//   */
// static void USART_GPIO_Config ( void )
// {
// 	RCC_4G_GPIO_ClockCmd ( RCC_USART3_GPIO_Periph, ENABLE );
	
// 	RCC_4G_GPIO_ClockCmd ( RCC_APB2Periph_GPIOC, ENABLE );
	 
// 	GPIO_InitTypeDef GPIO_InitStructure;

//   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
// 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
// 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
// 	GPIO_Init(GPIOC, &GPIO_InitStructure);	
// //	GPIO_SetBits(GPIOC,GPIO_Pin_9);
// 	GPIO_ResetBits(GPIOC, GPIO_Pin_9);



// 	/* USART GPIO config */
// 	/* Configure USART Tx as alternate function push-pull */
// 	GPIO_InitStructure.GPIO_Pin =  PIN_USART3_TX;
// 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
// 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
// 	GPIO_Init(PORT_USART3_TX, &GPIO_InitStructure);  
  
// 	/* Configure USART Rx as input floating */
// 	GPIO_InitStructure.GPIO_Pin = PIN_USART3_RX;
// 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
// 	GPIO_Init(PORT_USART3_RX, &GPIO_InitStructure);
// }

// static void USART_4G_NVIC_Configuration ( void )
// {
// 	NVIC_InitTypeDef NVIC_InitStructure; 
	
	
// 	/* Enable the USART_3 Interrupt */
// 	NVIC_InitStructure.NVIC_IRQChannel = USART_3_IRQn;	 
// 	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
// 	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
// 	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
// 	NVIC_Init(&NVIC_InitStructure);

// }
// /**
//   * @brief  初始化ESP8266用到的 USART
//   * @param  无
//   * @retval 无
//   */
// static void USART_USART_Config ( void )
// {
		
// 	/* config USART clock */
// 	RCC_USART3_ClockCmd ( RCC_USART3_Periph, ENABLE );
	
// 	USART_InitTypeDef USART_InitStructure;
	
	
// 	/* USART1 mode config */
// 	USART_InitStructure.USART_BaudRate = USART_BAUD_RATE;
// 	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
// 	USART_InitStructure.USART_StopBits = USART_StopBits_1;
// 	USART_InitStructure.USART_Parity = USART_Parity_No ;
// 	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
// 	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
// 	USART_Init(USART_3, &USART_InitStructure);
	
	
// 	/* 中断配置 */
// 	USART_ITConfig ( USART_3, USART_IT_RXNE, ENABLE ); //使能串口接收中断 
// 	USART_ITConfig ( USART_3, USART_IT_IDLE, ENABLE ); //使能串口总线空闲中断 	

// 	USART_4G_NVIC_Configuration ();
	
	
// 	USART_Cmd(USART_3, ENABLE);
	
	
// }

// void USART_4G_IRQnHandler ( void )
// {	
// 	uint8_t ucCh;
	
// 	if ( USART_GetITStatus ( USART_3, USART_IT_RXNE ) != RESET )
// 	{
// 		ucCh  = USART_ReceiveData( USART_3 );
		
// 	 if ( USART_4G_Fram_Instance.InfBit.FramLength < ( RX2_4G_BUF_MAX_LEN - 2 ) )    		 //预留1个字节写结束符
// 		USART_4G_Fram_Instance.Data_RX_BUF [ USART_4G_Fram_Instance.InfBit.FramLength ++ ]  = ucCh;
		
// 	}
	 	 
// 	if ( USART_GetITStatus( USART_3, USART_IT_IDLE ) == SET )                                         //数据帧接收完毕
// 	{
//        USART_4G_Fram_Instance.InfBit.FramFinishFlag = 1;
		
// 	     ucCh = USART_ReceiveData( USART_3 );                                                             
	
		
//   }	

// }


// /*
//  * 函数名：WiFi_Cmd
//  * 描述  ：模块发送AT指令
//  * 输入  ：cmd，待发送的指令
//  *         reply1，reply2，期待的响应，为NULL表不需响应，两者为或逻辑关系
//  *         waittime，等待响应的时间
//  * 返回  : 1，指令发送成功
//  *         0，指令发送失败
//  * 调用  ：被外部调用
//  */
//  bool ME_Cmd ( char * cmd, char * reply2, u32 waittime)
// {    
// 	bool isVerify;
// 	char *resultCMD = cmd;
// 	char *resultRP = reply2;
// 	USART_4G_Fram_Instance.InfBit.FramLength = 0;               //从新开始接收新的数据包
// 	USART_4G_Fram_Instance.InfBit.FramFinishFlag = 0;                             
// 	memset((char *)USART_4G_Fram_Instance.Data_RX_BUF, 0, RX2_4G_BUF_MAX_LEN);
// //	USART_4G_Fram_Instance.Data_RX_BUF [USART_4G_Fram_Instance.InfBit.FramLength ]  = '\0';
  
	
// 	WIFI_Usart ( "%s\r\n", resultCMD );


	
// 	delay_ms ( waittime );                 //延时
	
	
// //	char red[200];
// //	strcpy(red, USART_4G_Fram_Instance.Data_RX_BUF);
//                              //清除接收标志
// //	if ( ( replay7 != 0 ) && ( reply2 != 0 ) )
// //		isVerify = ( ( bool ) strstr ( USART_4G_Fram_Instance.Data_RX_BUF, replay7 ) || 
// //						 ( bool ) strstr ( USART_4G_Fram_Instance.Data_RX_BUF, reply2 ) ); 
// // 	
// //	else if ( replay7 != 0 )
// //		isVerify = ( ( bool ) strstr ( USART_4G_Fram_Instance.Data_RX_BUF, replay7 ) );
// //	
// //	else
// 	if (  resultRP == 0 ) {
// 			isVerify = true;

// 	} else {
// 			isVerify = ( ( bool ) strstr ( (char *)USART_4G_Fram_Instance.Data_RX_BUF, resultRP ) );

// 	}                      //不需要接收数据
	
// //	USART_4G_Fram_Instance.InfBit.FramLength = 0;               //从新开始接收新的数据包
// //	USART_4G_Fram_Instance.InfBit.FramFinishFlag = 0;                             
// ////	USART_4G_Fram_Instance.Data_RX_BUF [USART_4G_Fram_Instance.InfBit.FramLength ]  = '\0';
// //	memset(USART_4G_Fram_Instance.Data_RX_BUF, 0, RX2_4G_BUF_MAX_LEN);
// //	
// 	return isVerify;
	
// }


// bool ME_CmdWithTemp( char * cmd, char * reply1, char * reply2, u32 waittime, char *tempReceive)
// {    
// 	bool isVerify;
// 	USART_4G_Fram_Instance.InfBit.FramLength = 0;               //从新开始接收新的数据包
// 	USART_4G_Fram_Instance.InfBit.FramFinishFlag = 0;                             
// 	memset((char *)USART_4G_Fram_Instance.Data_RX_BUF, 0, strlen((char *)USART_4G_Fram_Instance.Data_RX_BUF));

	
// 	WIFI_Usart ( "%s\r\n", cmd );

// 	if ( ( reply1 == 0 ) && ( reply2 == 0 ) )                      //不需要接收数据
// 		return true;
	
// 	delay_ms ( waittime );                 //延时
	
// 	if ( ( reply1 != 0 ) && ( reply2 != 0 ) )
// 		isVerify = ( ( bool ) strstr ( (char *)USART_4G_Fram_Instance.Data_RX_BUF, reply1 ) || 
// 						 ( bool ) strstr ( (char *)USART_4G_Fram_Instance.Data_RX_BUF, reply2 ) ); 
 	
// 	else if ( reply1 != 0 )
// 		isVerify = ( ( bool ) strstr ( (char *)USART_4G_Fram_Instance.Data_RX_BUF, reply1 ) );
	
// 	else
// 		isVerify = ( ( bool ) strstr ( (char *)USART_4G_Fram_Instance.Data_RX_BUF, reply2 ) );
	
// 	memcpy(tempReceive, (char *)USART_4G_Fram_Instance.Data_RX_BUF, strlen((char *)USART_4G_Fram_Instance.Data_RX_BUF));
// //	tempReceive = strEsp8266_Fram_Record.Data_RX_BUF;
// 	USART_4G_Fram_Instance.InfBit.FramLength = 0;               //从新开始接收新的数据包
// 	USART_4G_Fram_Instance.InfBit.FramFinishFlag = 0;                             
// 	memset((char *)USART_4G_Fram_Instance.Data_RX_BUF, 0, strlen((char *)USART_4G_Fram_Instance.Data_RX_BUF));
	
// 	return isVerify;
	
// }

// bool ME_GetIMEI ( )
// {
// 	char cCmd [20];

// 	sprintf ( cCmd, "AT+CGSN");
// 	if (ME_Cmd ( cCmd, "OK", 500 )) {
// 		return true;
// 	} else {
// 		return false;
// 	}
	
// }

// bool ME_RSSI( )
// {
// 	char cCmd [20];

// 	sprintf ( cCmd, "AT+CSQ");
// 	if (ME_Cmd ( cCmd, "OK", 500 )) {
// 		return true;
// 	} else {
// 		return false;
// 	}
	
// }

// bool ME_CPIN ( )
// {
// 	char cCmd [20];

// 	sprintf ( cCmd, "AT+CPIN?");
// 	if (ME_Cmd ( cCmd, "READY", 500 )) {
// 		return true;
// 	} else {
// 		return false;
// 	}
	
// }

// bool ME_CEREG ( )
// {
// 	char cCmd [20];

// 	sprintf ( cCmd, "AT+CEREG?");
// 	if (ME_Cmd ( cCmd, "CEREG: 0,1", 500 )) {
// 		return true;
// 	} else {
// 		return false;
// 	}
	
// }

// bool GPRS_Connect ( )
// {
// 	char cCmd [50];

// 	sprintf ( cCmd, "AT+ZIPCFG=cmiot,none,none");
// 	if (ME_Cmd ( cCmd, "OK", 500 )) {
// 		return true;
// 	} else {
// 		return false;
// 	}
	
// }

// //bool GPRS_Connect ( )
// //{
// //	char cCmd [50];

// //	sprintf ( cCmd, "AT+ZIPCFG=cmnet,none,none");
// //	if (ME_Cmd ( cCmd, "OK", 500 )) {
// //		return true;
// //	} else {
// //		return false;
// //	}
// //	
// //}

// //bool Open_Call( )
// //{
// //	char cCmd [50];

// //	sprintf ( cCmd, "AT+ZIPCALL=1");
// //	if (ME_Cmd ( cCmd, "+ZIPCALL:", 500 )) {
// //		return true;
// //	} else {
// //		return false;
// //	}
// //	
// //}

// bool Open_Call( )
// {
// 	char cCmd [50];

// 	sprintf ( cCmd, "AT+ZIPCALL=1");
// 	if (ME_Cmd ( cCmd, NULL, 500 )) {
// 		return true;
// 	} else {
// 		return false;
// 	}
	
// }

// bool Close_Call( )
// {
// 	char cCmd [50];

// 	sprintf ( cCmd, "AT+ZIPCLOSE=1");
// 	if (ME_Cmd ( cCmd, "+OK:", 500 )) {
// 		return true;
// 	} else {
// 		return false;
// 	}
	
// }


// //bool TCP_Connect( )
// //{
// //	char cCmd [50];

// //	sprintf ( cCmd, "AT+ZIPOPEN=1,0,115.236.153.174,25773");
// //	if (ME_Cmd ( cCmd, "+ZIPSTAT: 1,1", 500 )) {
// //		return true;
// //	} else {
// //		return false;
// //	}
// //	
// //}

// bool TCP_Connect( )
// {
// 	char cCmd [50];

// 	sprintf ( cCmd, "AT+ZIPOPEN=1,0,115.236.153.174,25773");
// 	if (ME_Cmd ( cCmd, NULL, 500 )) {
// 		return true;
// 	} else {
// 		return false;
// 	}
	
// }

// bool TCP_Keep_ALive( )
// {
// 	char cCmd [50];

// 	sprintf ( cCmd, "AT+ZIPALIVE=1,1,300,75,9");
// 	if (ME_Cmd ( cCmd, "OK", 500 )) {
// 		return true;
// 	} else {
// 		return false;
// 	}
	
// }
// void TCP_Send(char *message)
// {
// 	char cCmd [50];

// 	sprintf ( cCmd, "AT+ZIPSEND=1,%s",message);
// 	if (ME_Cmd ( cCmd, "OK", 200 )) {
// 	} else {
// 		TCP_Is_Connect = false; 

// 		Start_ME();
// //		Close_Call();

// //		TCP_Send(message);
// 	}
	
// }

// void clearFrame(void) {
// 		USART_4G_Fram_Instance.InfBit.FramLength = 0;
// 	  USART_4G_Fram_Instance.InfBit.FramFinishFlag = 0;
// 	  memset((char *)USART_4G_Fram_Instance.Data_RX_BUF, 0, strlen((char *)USART_4G_Fram_Instance.Data_RX_BUF));
// }
// void ReceiveString (void)
// {

// 	char pRecStr[100] = { 0 };
// 	if (USART_4G_Fram_Instance.InfBit.FramFinishFlag == 0) {
// 		return;
// 	}
	
// 	 strcpy(pRecStr, (char *)USART_4G_Fram_Instance.Data_RX_BUF);
	
 

// 	if (strstr(pRecStr, "S44"))
//   {
		
// 		PAout(1) = 1; 
// 		clearFrame();
// 	} 
// 	else if (strstr(pRecStr, "S33"))
// 	{
// 		PAout(1) = 0;
// 		clearFrame();

// 	}
	
	
	
	
// }

// static __IO uint32_t  timeout = 50;     

// void Start_ME() 
// {
	
// 	restart:
// 	GPIO_SetBits(GPIOC,GPIO_Pin_9);
// 	delay_ms(2000);
// 	GPIO_ResetBits(GPIOC, GPIO_Pin_9);
// 	delay_ms(5000);
// //	delay_ms(1000);
// //	delay_ms(1000);


// //	while (!ME_RSSI());
// 	timeout = 50;
// 	while (!ME_CPIN()) {
// 		if((timeout--) == 0) goto restart;
// 	};
// 	timeout = 50;
// 	while (!ME_CEREG()) {
// 		if((timeout--) == 0) goto restart;
// 	}
// 	timeout = 50;
// 	while (!Open_Call()) {
// 		if((timeout--) == 0) goto restart;
// 	}
// 	timeout = 50;
// 	while (!TCP_Connect()) {
// 		if((timeout--) == 0) goto restart;
// 	}
// 	TCP_Keep_ALive();

// 	TCP_Is_Connect = true;
	
	
	
	
	
// }






