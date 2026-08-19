#include "stm32f10x.h"                  // Device header
#include "delay.h"

void AD_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 |GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	ADC_RegularChannelConfig(ADC1,ADC_Channel_0,1,ADC_SampleTime_55Cycles5);//在序列1的位置写入通道0
	
	ADC_InitTypeDef ADC_InitStructure;
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;//单通道，非连续转换
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;//非扫描
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADC1,&ADC_InitStructure);
	
	ADC_Cmd(ADC1,ENABLE);//给ADC上电
	
	ADC_ResetCalibration(ADC1);//复位校准
	while(ADC_GetResetCalibrationStatus(ADC1)==SET);//返回复位校准的状态，一旦标志位被硬件清零，空循环自动跳出
	ADC_StartCalibration(ADC1);//开始校准
	while(ADC_GetCalibrationStatus(ADC1)==SET);//等待校准是否完成
}

uint16_t AD_GetValue(uint8_t ADC_Channel)
{
	ADC_RegularChannelConfig(ADC1,ADC_Channel,1,ADC_SampleTime_55Cycles5);//在序列1的位置写入通道0
	ADC_SoftwareStartConvCmd(ADC1,ENABLE);//软件触发转换     启动
	while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC)==RESET);//获取标志位状态,转换未完成    等待
	uint16_t adc_val = ADC_GetConversionValue(ADC1);//读取
	
	return (float)adc_val*3.3f/4096.0f;
}

float AD_Average(uint8_t channel,uint8_t samples)
{
	uint32_t sum=0;
	for(uint8_t i=0;i<samples;i++)
	{
		ADC_RegularChannelConfig(ADC1,channel,1,ADC_SampleTime_55Cycles5);
		ADC_SoftwareStartConvCmd(ADC1,ENABLE);
		while(!ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC));
		sum+=ADC_GetConversionValue(ADC1);
		Delay_ms(5);
	}
	float avg=(float)sum/samples;
	return avg*3.3f/4096.0f;
}

//中值滤波
float AD_Median(uint8_t channel,uint8_t samples)
{
	float buf[10];
	for(int i=0;i<samples;i++)
	{
		buf[i]=AD_GetValue(channel);
	}
	//冒泡排序取中间值
	for(int i=0;i<samples;i++)
	{
		for(int j=0;j<samples-i-1;j++)
		{
			if(buf[j]>buf[j+1])
			{
				float t=buf[j];
				buf[j]=buf[j+1];
				buf[j+1]=t;
			}
		}
	}
	return buf[samples/2];
}
