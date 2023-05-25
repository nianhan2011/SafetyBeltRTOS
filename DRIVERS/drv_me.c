#include "drv_me.h"
#include <stdio.h>
#include <string.h>
#include "drv_voice.h"
#include "drv_gps.h"
#include "drv_opt.h"

static void USART_GPIO_Config(void);
static void USART_USART_Config(void);
static void USART_4G_NVIC_Configuration(void);
static void clearFrame(void);

USART_4G_Fram USART_4G_Fram_Instance = {0};
/**
 * @brief  初始化函数
 * @param  无
 * @retval 无
 */
void USART_4G_Init(void)
{
	USART_GPIO_Config();

	USART_USART_Config();
}

/**
 * @brief  初始化usart用到的GPIO引脚
 * @param  无
 * @retval 无
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
 * @brief  初始化ESP8266用到的 USART
 * @param  无
 * @retval 无
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

	/* 中断配置 */
	USART_ITConfig(USART_4G, USART_IT_RXNE, ENABLE); // 使能串口接收中断
	USART_ITConfig(USART_4G, USART_IT_IDLE, ENABLE); // 使能串口总线空闲中断

	USART_4G_NVIC_Configuration();

	USART_Cmd(USART_4G, ENABLE);
}

void USART_4G_IRQnHandler(void)
{
	uint8_t ucCh;

	if (USART_GetITStatus(USART_4G, USART_IT_RXNE) != RESET)
	{
		ucCh = USART_ReceiveData(USART_4G);

		if (USART_4G_Fram_Instance.InfBit.FramLength >= (RX2_4G_BUF_MAX_LEN - 2))
		{
			USART_4G_Fram_Instance.InfBit.FramLength = 0;
		} // 预留1个字节写结束符
		USART_4G_Fram_Instance.Data_RX_BUF[USART_4G_Fram_Instance.InfBit.FramLength++] = ucCh;
	}

	if (USART_GetITStatus(USART_4G, USART_IT_IDLE) == SET) // 数据帧接收完毕
	{
		USART_4G_Fram_Instance.InfBit.FramFinishFlag = 1;

		ucCh = USART_ReceiveData(USART_4G);
	}
}

/*
 * 函数名：WiFi_Cmd
 * 描述  ：模块发送AT指令
 * 输入  ：cmd，待发送的指令
 *         reply1，reply2，期待的响应，为NULL表不需响应，两者为或逻辑关系
 *         waittime，等待响应的时间
 * 返回  : 1，指令发送成功
 *         0，指令发送失败
 * 调用  ：被外部调用
 */
bool ME_Cmd(char *cmd, char *reply, u32 waittime, u8 isClearFrame)
{
	bool isVerify;

	WIFI_Usart("%s\r\n", cmd);

	vTaskDelay(waittime); // 延时

	if (reply == 0)
	{
		isVerify = true;
	}
	else
	{
		isVerify = ((bool)strstr((char *)USART_4G_Fram_Instance.Data_RX_BUF, reply));

	} // 不需要接收数据

	if (isClearFrame)
	{
		clearFrame();
	}
	// memset(USART_4G_Fram_Instance.Data_RX_BUF, 0, RX2_4G_BUF_MAX_LEN);

	return isVerify;
}

bool ME_GetIMEI()
{
	char cCmd[20];

	sprintf(cCmd, "AT+CGSN");
	if (ME_Cmd(cCmd, "OK", 500, false))
	{
		for (u8 i = 0, j = 0; i < strlen(USART_4G_Fram_Instance.Data_RX_BUF); i++)
		{
			/* code */
			if (USART_4G_Fram_Instance.Data_RX_BUF[i] >= '0' && USART_4G_Fram_Instance.Data_RX_BUF[i] <= '9')
			{
				drv_me_pt->imei_id[j++] = USART_4G_Fram_Instance.Data_RX_BUF[i];
			}
		}
		clearFrame();
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
	if (ME_Cmd(cCmd, "OK", 500, true))
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
	if (ME_Cmd(cCmd, NULL, 500, true))
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
	if (ME_Cmd(cCmd, "READY", 500, true))
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
	if (ME_Cmd(cCmd, "CEREG: 0,1", 500, true))
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
	if (ME_Cmd(cCmd, "+ZIPCALL:", 500, true))
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

	sprintf(cCmd, "AT+ZIPCALL=0");
	if (ME_Cmd(cCmd, "+OK:", 500, true))
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
	if (ME_Cmd(cCmd, "+ZIPSTAT: 1,1", 500, true))
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
	if (ME_Cmd(cCmd, "OK", 500, true))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool transmission_mode()
{
	char cCmd[50];

	sprintf(cCmd, "AT+ZIPCREATE=1");
	if (ME_Cmd(cCmd, NULL, 500, true))
	{
		return true;
	}
	else
	{
		return false;
	}
}
void clearFrame(void)
{
	// thread_cslock_lock(drv_me_pt->lock, MaxTick);

	// USART_4G_Fram_Instance.InfBit.FramLength = 0; // 从新开始接收新的数据包
	// USART_4G_Fram_Instance.InfBit.FramFinishFlag = 0;
	// USART_4G_Fram_Instance.Data_RX_BUF[USART_4G_Fram_Instance.InfBit.FramLength] = '\0';
	taskENTER_CRITICAL(); // 进入临界区

	USART_4G_Fram_Instance.InfBit.FramLength = 0;
	USART_4G_Fram_Instance.InfBit.FramFinishFlag = 0;
	memset((char *)USART_4G_Fram_Instance.Data_RX_BUF, 0, strlen((char *)USART_4G_Fram_Instance.Data_RX_BUF));
	// thread_cslock_free(drv_me_pt->lock);
	taskEXIT_CRITICAL(); // 退出临界区
}

static __IO uint32_t timeout = 50;

void start_me()
{

restart:

	drv_voice_pt->yyhy();
	GPIO_SetBits(GPIOC, GPIO_Pin_9);
	vTaskDelay(200);
	GPIO_ResetBits(GPIOC, GPIO_Pin_9);
	vTaskDelay(1000);

	timeout = 50;
	while (!ME_115200())
	{
		if ((timeout--) == 0)
			goto restart;
	};

	timeout = 50;
	while (!ME_GetIMEI())
	{
		if ((timeout--) == 0)
			goto restart;
	}

	timeout = 50;
	while (!ME_CPIN())
	{
		if ((timeout--) == 0)
			goto restart;
	};

	timeout = 50;
	while (!ME_RSSI())
	{
		if ((timeout--) == 0)
			goto restart;
	}

	timeout = 50;

	while (!ME_CEREG())
	{
		if ((timeout--) == 0)
			goto restart;
	};
	timeout = 50;

	while (!Open_Call())
	{
		if ((timeout--) == 0)
			goto restart;
	};
	timeout = 50;

	while (!TCP_Connect())
	{
		if ((timeout--) == 0)
			goto restart;
	};
	timeout = 50;
	while (!TCP_Keep_ALive())
	{
		if ((timeout--) == 0)
			goto restart;
	}

	timeout = 50;
	while (!transmission_mode())
	{
		if ((timeout--) == 0)
			goto restart;
	}

	drv_me_pt->tcp_connection_status = true;
	drv_voice_pt->net_success();
}

static void pre_send_data(void)
{
	memset(drv_me_pt->send_data,0,1024);

	strcat(drv_me_pt->send_data, drv_me_pt->imei_id);//imei
	strcat(drv_me_pt->send_data, " ");

	strcat(drv_me_pt->send_data, "101.1");//气压
	strcat(drv_me_pt->send_data, " ");

	strcat(drv_me_pt->send_data, "O2");//设备状态
	strcat(drv_me_pt->send_data, " ");

	strcat(drv_me_pt->send_data, "100");//电量
	strcat(drv_me_pt->send_data, " ");

	// strcat(drv_me_pt->send_data, "C11"); // 请求上锁/解锁
	// strcat(drv_me_pt->send_data, " ");

	// strcat(drv_me_pt->send_data, "W11"); // 工人请求/报警/高度
	// strcat(drv_me_pt->send_data, " ");

    char height_str[10];
	sprintf(height_str, "T%d",drv_height_pt->height);
	strcat(drv_me_pt->send_data, height_str); // 工人请求/报警/高度
	strcat(drv_me_pt->send_data, " ");

	strcat(drv_me_pt->send_data, drv_gps_pt->latitude); // 维度
	strcat(drv_me_pt->send_data, " ");

	strcat(drv_me_pt->send_data, drv_gps_pt->longitude  ); // 经度
	strcat(drv_me_pt->send_data, " ");
}

void me3630_send(void)
{

	while (1)
	{
		/* code */
		switch (drv_me_pt->send_step)
		{
		case ME_SEND_DEFAULT:
			if (drv_me_pt->tcp_connection_status == true)
			{
				pre_send_data();
				thread_cslock_lock(drv_me_pt->lock, MaxTick);

				WIFI_Usart("%s\r\n", drv_me_pt->send_data);
				vTaskDelay(50);
				thread_cslock_free(drv_me_pt->lock);
			}
			/* code */
			break;

		default:
			break;
		}

		vTaskDelay(5000);
	}
}

void me3630_receive(void)
{

	char pRecStr[100] = {0};

	strcpy(pRecStr, (char *)USART_4G_Fram_Instance.Data_RX_BUF);

	if (strstr(pRecStr, "ERROR"))
	{
		thread_cslock_lock(drv_me_pt->lock, MaxTick);
		Close_Call();
		drv_me_pt->tcp_connection_status = false;
		thread_cslock_free(drv_me_pt->lock);

		clearFrame();
		// WIFI_Usart("%s\r\n", "AT+");
	}
	if (strstr(pRecStr, "S44"))
	{

		drv_voice_pt->unlock_finish();
		clearFrame();
	}
	if (strstr(pRecStr, "S33"))
	{
		drv_voice_pt->open_hooking();
		clearFrame();
	}

	if (strstr(pRecStr, "+ZIPSEND"))
	{
		clearFrame();
	}
}

void me_proc(void)
{
	while (1)
	{
		/* code */

		if (drv_me_pt->tcp_connection_status != true)
		{
			start_me();
		}
		else
		{
			me3630_receive();
		}
	}
}
static Drv_Me_t drv_me_t = {
	.tcp_connection_status = 0,
	.me_proc = me_proc,
	.me3630_send = me3630_send,
	.imei_id = {0},
	.send_step = ME_SEND_DEFAULT,
	.send_data = {0},
};
Drv_Me_pt drv_me_pt;

void init_me(void)
{
	drv_me_pt = &drv_me_t;
	drv_me_pt->lock = thread_cslock_init("drv_me"),

	USART_4G_Init();
}
