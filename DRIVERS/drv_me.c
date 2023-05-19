#include "drv_me.h"
#include "common.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "os_system__typedef.h"
static void USART_GPIO_Config(void);
static void USART_USART_Config(void);
static void USART_4G_NVIC_Configuration(void);

USART_4G_Fram USART_4G_Fram_Instance = {0};
_Bool TCP_Is_Connect = false;
/**
 * @brief  ��ʼ������
 * @param  ��
 * @retval ��
 */
void USART_4G_Init(void)
{
	USART_GPIO_Config();

	USART_USART_Config();
}

/**
 * @brief  ��ʼ��usart�õ���GPIO����
 * @param  ��
 * @retval ��
 */
static void USART_GPIO_Config(void)
{
	RCC_4G_GPIO_ClockCmd(RCC_4G_GPIO_Periph, ENABLE);

	RCC_4G_GPIO_ClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_SetBits(GPIOC, GPIO_Pin_9);

	/* USART GPIO config */
	/* Configure USART Tx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = PIN_4G_TX;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(PORT_4G_TX, &GPIO_InitStructure);

	/* Configure USART Rx as input floating */
	GPIO_InitStructure.GPIO_Pin = PIN_4G_RX;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(PORT_4G_RX, &GPIO_InitStructure);
}

static void USART_4G_NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Enable the USART_4G Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART_4G_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
/**
 * @brief  ��ʼ��ESP8266�õ��� USART
 * @param  ��
 * @retval ��
 */
static void USART_USART_Config(void)
{

	/* config USART clock */
	RCC_4G_USART_ClockCmd(RCC_4G_USART_Periph, ENABLE);

	USART_InitTypeDef USART_InitStructure;

	/* USART1 mode config */
	USART_InitStructure.USART_BaudRate = USART_BAUD_RATE_4G;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART_4G, &USART_InitStructure);

	/* �ж����� */
	USART_ITConfig(USART_4G, USART_IT_RXNE, ENABLE); // ʹ�ܴ��ڽ����ж�
	USART_ITConfig(USART_4G, USART_IT_IDLE, ENABLE); // ʹ�ܴ������߿����ж�

	USART_4G_NVIC_Configuration();

	USART_Cmd(USART_4G, ENABLE);
}

void USART_4G_IRQnHandler(void)
{
	uint8_t ucCh;

	if (USART_GetITStatus(USART_4G, USART_IT_RXNE) != RESET)
	{
		ucCh = USART_ReceiveData(USART_4G);

		if (USART_4G_Fram_Instance.InfBit.FramLength < (RX2_4G_BUF_MAX_LEN - 2)) // Ԥ��1���ֽ�д������
			USART_4G_Fram_Instance.Data_RX_BUF[USART_4G_Fram_Instance.InfBit.FramLength++] = ucCh;
	}

	if (USART_GetITStatus(USART_4G, USART_IT_IDLE) == SET) // ����֡�������
	{
		USART_4G_Fram_Instance.InfBit.FramFinishFlag = 1;

		ucCh = USART_ReceiveData(USART_4G);
	}
}

/*
 * ��������WiFi_Cmd
 * ����  ��ģ�鷢��ATָ��
 * ����  ��cmd�������͵�ָ��
 *         reply1��reply2���ڴ�����Ӧ��ΪNULL������Ӧ������Ϊ���߼���ϵ
 *         waittime���ȴ���Ӧ��ʱ��
 * ����  : 1��ָ��ͳɹ�
 *         0��ָ���ʧ��
 * ����  �����ⲿ����
 */
bool ME_Cmd(char *cmd, char *reply2, u32 waittime)
{
	bool isVerify;
	char *resultCMD = cmd;
	char *resultRP = reply2;
	USART_4G_Fram_Instance.InfBit.FramLength = 0; // ���¿�ʼ�����µ����ݰ�
	USART_4G_Fram_Instance.InfBit.FramFinishFlag = 0;
	memset((char *)USART_4G_Fram_Instance.Data_RX_BUF, 0, RX2_4G_BUF_MAX_LEN);
	//	USART_4G_Fram_Instance.Data_RX_BUF [USART_4G_Fram_Instance.InfBit.FramLength ]  = '\0';

	WIFI_Usart("%s\r\n", resultCMD);

	vTaskDelay(waittime); // ��ʱ

	//	char red[200];
	//	strcpy(red, USART_4G_Fram_Instance.Data_RX_BUF);
	// ������ձ�־
	//	if ( ( replay7 != 0 ) && ( reply2 != 0 ) )
	//		isVerify = ( ( bool ) strstr ( USART_4G_Fram_Instance.Data_RX_BUF, replay7 ) ||
	//						 ( bool ) strstr ( USART_4G_Fram_Instance.Data_RX_BUF, reply2 ) );
	//
	//	else if ( replay7 != 0 )
	//		isVerify = ( ( bool ) strstr ( USART_4G_Fram_Instance.Data_RX_BUF, replay7 ) );
	//
	//	else
	if (resultRP == 0)
	{
		isVerify = true;
	}
	else
	{
		isVerify = ((bool)strstr((char *)USART_4G_Fram_Instance.Data_RX_BUF, resultRP));

	} // ����Ҫ��������

	//	USART_4G_Fram_Instance.InfBit.FramLength = 0;               //���¿�ʼ�����µ����ݰ�
	//	USART_4G_Fram_Instance.InfBit.FramFinishFlag = 0;
	////	USART_4G_Fram_Instance.Data_RX_BUF [USART_4G_Fram_Instance.InfBit.FramLength ]  = '\0';
	//	memset(USART_4G_Fram_Instance.Data_RX_BUF, 0, RX2_4G_BUF_MAX_LEN);
	//
	return isVerify;
}

bool ME_CmdWithTemp(char *cmd, char *reply1, char *reply2, u32 waittime, char *tempReceive)
{
	bool isVerify;
	USART_4G_Fram_Instance.InfBit.FramLength = 0; // ���¿�ʼ�����µ����ݰ�
	USART_4G_Fram_Instance.InfBit.FramFinishFlag = 0;
	memset((char *)USART_4G_Fram_Instance.Data_RX_BUF, 0, strlen((char *)USART_4G_Fram_Instance.Data_RX_BUF));

	WIFI_Usart("%s\r\n", cmd);

	if ((reply1 == 0) && (reply2 == 0)) // ����Ҫ��������
		return true;

	vTaskDelay(waittime); // ��ʱ

	if ((reply1 != 0) && (reply2 != 0))
		isVerify = ((bool)strstr((char *)USART_4G_Fram_Instance.Data_RX_BUF, reply1) ||
					(bool)strstr((char *)USART_4G_Fram_Instance.Data_RX_BUF, reply2));

	else if (reply1 != 0)
		isVerify = ((bool)strstr((char *)USART_4G_Fram_Instance.Data_RX_BUF, reply1));

	else
		isVerify = ((bool)strstr((char *)USART_4G_Fram_Instance.Data_RX_BUF, reply2));

	memcpy(tempReceive, (char *)USART_4G_Fram_Instance.Data_RX_BUF, strlen((char *)USART_4G_Fram_Instance.Data_RX_BUF));
	//	tempReceive = strEsp8266_Fram_Record.Data_RX_BUF;
	USART_4G_Fram_Instance.InfBit.FramLength = 0; // ���¿�ʼ�����µ����ݰ�
	USART_4G_Fram_Instance.InfBit.FramFinishFlag = 0;
	memset((char *)USART_4G_Fram_Instance.Data_RX_BUF, 0, strlen((char *)USART_4G_Fram_Instance.Data_RX_BUF));

	return isVerify;
}

bool ME_GetIMEI()
{
	char cCmd[20];

	sprintf(cCmd, "AT+CGSN=1");
	if (ME_Cmd(cCmd, "OK", 500))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool ME_RSSI()
{
	char cCmd[20];

	sprintf(cCmd, "AT+CSQ");
	if (ME_Cmd(cCmd, "OK", 500))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool ME_115200()
{
	char cCmd[20];

	sprintf(cCmd, "AT+IPR=115200");
	if (ME_Cmd(cCmd, NULL, 500))
	{
		return true;
	}
	else
	{
		return false;
	}
}
bool ME_CPIN()
{
	char cCmd[20];

	sprintf(cCmd, "AT+CPIN?");
	if (ME_Cmd(cCmd, "READY", 500))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool ME_CEREG()
{
	char cCmd[20];

	sprintf(cCmd, "AT+CEREG?");
	if (ME_Cmd(cCmd, "CEREG: 0,1", 500))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool Open_Call()
{
	char cCmd[50];

	sprintf(cCmd, "AT+ZIPCALL=1");
	if (ME_Cmd(cCmd, "+ZIPCALL:", 500))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool Close_Call()
{
	char cCmd[50];

	sprintf(cCmd, "AT+ZIPCLOSE=1");
	if (ME_Cmd(cCmd, "+OK:", 500))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool TCP_Connect()
{
	char cCmd[50];

	sprintf(cCmd, "AT+ZIPOPEN=1,0,115.236.153.174,25773");
	if (ME_Cmd(cCmd, "+ZIPSTAT: 1,1", 500))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool TCP_Keep_ALive()
{
	char cCmd[50];

	sprintf(cCmd, "AT+ZIPALIVE=1,1,300,75,9");
	if (ME_Cmd(cCmd, "OK", 500))
	{
		return true;
	}
	else
	{
		return false;
	}
}
void TCP_Send(char *message)
{
	char cCmd[50];

	sprintf(cCmd, "AT+ZIPSEND=1,%s", message);
	if (ME_Cmd(cCmd, NULL, 200))
	{
	}
	else
	{
		//		TCP_Is_Connect = false;
		//		Close_Call();
		//		Start_4G_TCP();

		//		TCP_Send(message);
	}
}

void clearFrame(void)
{
	USART_4G_Fram_Instance.InfBit.FramLength = 0;
	USART_4G_Fram_Instance.InfBit.FramFinishFlag = 0;
	memset((char *)USART_4G_Fram_Instance.Data_RX_BUF, 0, strlen((char *)USART_4G_Fram_Instance.Data_RX_BUF));
}

static __IO uint32_t timeout = 50;

void Start_4G_TCP(void)
{

	//	while (!ME_RSSI());

	while (!ME_115200());
	// while (!ME_GetIMEI());
			

	while (!ME_CPIN())
			;
	while (!ME_CEREG())
		;
	while (!Open_Call())
		;
	while (!TCP_Connect())
		;
	// while (!TCP_Keep_ALive())
		;

	TCP_Is_Connect = true;
}
void Start_ME()
{

	GPIO_SetBits(GPIOC, GPIO_Pin_9);
	vTaskDelay(200);
	GPIO_ResetBits(GPIOC, GPIO_Pin_9);
	vTaskDelay(1000);

	Start_4G_TCP();
}

void ReceiveString(void)
{

	Start_ME();
	while (1)
	{
		char pRecStr[100] = {0};
		if (USART_4G_Fram_Instance.InfBit.FramFinishFlag == 0)
		{
			return;
		}

		strcpy(pRecStr, (char *)USART_4G_Fram_Instance.Data_RX_BUF);

		if (strstr(pRecStr, "S44"))
		{

			PAout(1) = 1;
			clearFrame();
		}
		else if (strstr(pRecStr, "S33"))
		{
			PAout(1) = 0;
			clearFrame();
		}
	}
}
