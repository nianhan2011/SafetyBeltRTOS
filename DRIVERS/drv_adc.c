#include "drv_adc.h"
#include <string.h>
#include "os_system__typedef.h"
#include "Common.h"
#include "drv_opt.h"
#include "drv_voice.h"
#include "motoControl.h"
#include "drv_me.h"
static Drv_ADC_t drv_adc_t;
Drv_ADC_pt drv_adc_pt;

static void ADCx_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	// 打开 ADC IO端口时钟
	ADC_GPIO_APBxClock_FUN(RCC_APB2Periph_GPIOA, ENABLE);
	ADC_GPIO_APBxClock_FUN(RCC_APB2Periph_GPIOC, ENABLE);

	// 配置 ADC IO 引脚模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	// 初始化 ADC IO
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;

	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

// ANSI C标准，C89 标准
// C99
// C11 2011

static void ADCx_Mode_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	RCC_AHBPeriphClockCmd(ADC_DMA_CLK, ENABLE);

	DMA_DeInit(ADC_DMA_CHANNEL);
	// 配置 DMA 初始化结构体
	// 外设基址为：ADC 数据寄存器地址
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(ADC1->DR));
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)drv_adc_pt->ADC_ConvertedValue;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;				 // 数据源来自外设
	DMA_InitStructure.DMA_BufferSize = NOFCHANEL;					 // 改
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; // 外设寄存器只有一个，地址不用递增
	// 存储器地址固定
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; // 改
	// 外设数据大小为半字，即两个字节
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	// 内存数据大小也为半字，跟外设数据大小相同
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	// 循环传输模式
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	// 禁止存储器到存储器模式，因为是从外设到存储器
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(ADC_DMA_CHANNEL, &DMA_InitStructure);
	DMA_Cmd(ADC_DMA_CHANNEL, ENABLE);
	/*----------------------------------------------------------------------*/

	ADC_InitTypeDef ADC_InitStruct;
	ADC_APBxClock_FUN(ADC_CLK, ENABLE);
	// 单模式
	ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;
	// 连续扫描模式
	ADC_InitStruct.ADC_ScanConvMode = ENABLE; // 改
	// 连续转换模式
	ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;
	// 不用外部触发，软件开启
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	// 右对齐
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
	// 转换通道
	ADC_InitStruct.ADC_NbrOfChannel = NOFCHANEL;
	ADC_Init(ADC_x, &ADC_InitStruct);
	// 8分频 72/8 = 9
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);

	ADC_RegularChannelConfig(ADC_x, ADC_Channel_14, 1, ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC_x, ADC_Channel_15, 2, ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC_x, ADC_Channel_5, 3, ADC_SampleTime_239Cycles5);

	// 使能ADC DMA 请求
	ADC_DMACmd(ADC_x, ENABLE);

	// 初始化ADC 校准寄存器
	//	ADC_ResetCalibration(ADC_x);
	//	// 等待校准寄存器初始化完成
	//	while(ADC_GetResetCalibrationStatus(ADC_x));
	ADC_Cmd(ADC_x, ENABLE);
	// ADC开始校准
	ADC_StartCalibration(ADC_x);
	// 等待校准完成
	while (ADC_GetCalibrationStatus(ADC_x))
		;

	ADC_SoftwareStartConvCmd(ADC_x, ENABLE);
}

void ADCx_Init(void)
{
	ADCx_GPIO_Config();
	ADCx_Mode_Config();
}

void get_adc()
{

	while (1)
	{

		drv_adc_pt->ADC_ConvertedValueLocal[0] = (float)drv_adc_pt->ADC_ConvertedValue[0] / 4096 * 3.3;
		drv_adc_pt->ADC_ConvertedValueLocal[1] = (float)drv_adc_pt->ADC_ConvertedValue[1] / 4096 * 3.3;

		// drv_adc_pt->bat_percent = (drv_adc_pt->ADC_ConvertedValueLocal[1] * 2 - 3.5) *100/(4.2-3.5);
		drv_adc_pt->bat_percent = (drv_adc_pt->ADC_ConvertedValueLocal[1] * 2) / 4.2 * 100;
		if (drv_adc_pt->bat_percent > 100)
		{
			drv_adc_pt->bat_percent = 100;
		}

		if (drv_adc_pt->bat_percent < 10)
		{
			PAout(11) = 0;
			PAout(8) = 1;

			if (drv_adc_pt->bat_low_voice_cnt == 0)
			{
				drv_adc_pt->bat_low_voice_cnt = 3000;
				drv_voice_pt->low_battery();
			}
			else
			{
				drv_adc_pt->bat_low_voice_cnt--;
			}
		}
		else
		{
			PAout(11) = 1;
			PAout(8) = 0;
			drv_adc_pt->bat_low_voice_cnt = 3000;
		}

		// if (drv_adc_pt->ADC_ConvertedValueLocal[0] >= 0 && drv_adc_pt->ADC_ConvertedValueLocal[0] <= 1.6)
		// {
		// 	// Attitude_Alarm = 1;
		// }
		// else
		// {

		// 	// Attitude_Alarm = 0;
		// }

		vTaskDelay(10);
	}
}

void startListen(void)
{

	taskENTER_CRITICAL(); // 进入临界区
	u16 start_time = 100;
	while (start_time)
	{
		drv_adc_pt->ADC_ConvertedValueLocal[0] = (float)drv_adc_pt->ADC_ConvertedValue[0] / 4096 * 3.3;
		if (drv_adc_pt->ADC_ConvertedValueLocal[0] < 0.3)
		{
			PBout(13) = 0;
			while (1)
				;
		}
		timtick->delay(1);
		start_time--;
	}

	drv_voice_pt->yyhy();

	// vTaskDelay(1000);
	// drv_adc_pt->isStartTiming = 1;

	taskEXIT_CRITICAL();

	vTaskDelete(NULL);
}

void shutDownListen(void)
{
	while (1)
	{

		// drv_adc_pt->ADC_ConvertedValueLocal[0] = (float)drv_adc_pt->ADC_ConvertedValue[0] / 4096 * 3.3;

		if (drv_adc_pt->ADC_ConvertedValueLocal[0] >= 0.3)
		{
			u16 end_time = 300;

			u8 shouldShutDown = 1;
			while (end_time)
			{
				drv_adc_pt->ADC_ConvertedValueLocal[0] = (float)drv_adc_pt->ADC_ConvertedValue[0] / 4096 * 3.3;
				if (drv_adc_pt->ADC_ConvertedValueLocal[0] < 0.3)
				{
					shouldShutDown = 0;
					break;
				}

				vTaskDelay(10);
				end_time--;
			}

			if (shouldShutDown)
			{
				drv_moto_pt->device_status = Device_Status_Off;
				drv_me_pt->set_send_cnt(0);
				drv_voice_pt->yy_close();
				vTaskDelay(2000);
				PBout(13) = 0;
			}
		}
		else
		{
		}
	}
}

void init_adc(void)
{
	drv_adc_pt = &drv_adc_t;
	memset(drv_adc_pt->ADC_ConvertedValueLocal, 0, NOFCHANEL);
	memset(drv_adc_pt->ADC_ConvertedValue, 0, NOFCHANEL);
	drv_adc_pt->get_adc = get_adc;
	drv_adc_pt->startListen = startListen;
	drv_adc_pt->shutDownListen = shutDownListen;
	drv_adc_pt->bat_low_voice_cnt = 3000;
	ADCx_Init();
}
