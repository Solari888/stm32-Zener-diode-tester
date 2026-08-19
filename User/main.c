#include "stm32f10x.h"                  // Device header
#include "delay.h"                      //使用延时函数
#include "OLED.h"
#include "Serial.h"
#include "AD.h"
#include <string.h>
#define ZENER_CAL 2.04f

typedef enum{
	ZENER_OK,              //稳压正常
	ZENER_FORWARD,         //正向导通（接反/损坏）
	ZENER_BROKEN,          //短路损坏
	ZENER_OPEN             //开路或未接
}Zener_Status;

int main(void)
{
	//STM32的PB3/PB4/PA15用作普通IO使用
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
    DBGMCU->CR  &= ~((uint32_t)1<<5);
    
    //PC13、PC14、PC15引脚用作普通IO口配置方法
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO,ENABLE );
    PWR_BackupAccessCmd( ENABLE );/* 允许修改RTC和后备寄存器*/
    RCC_LSEConfig( RCC_LSE_OFF ); /* 关闭外部低速时钟,PC14+PC15可以用作普通IO*/
    BKP_TamperPinCmd(DISABLE);  /* 关闭入侵检测功能,PC13可以用作普通IO*/
    PWR_BackupAccessCmd(DISABLE);/* 禁止修改RTC和后备寄存器*/
	
	float value;
	Zener_Status status;
	char Serial_buf[32];
	uint8_t int_part,frac_part;
	
	AD_Init();
	OLED_Init();
	Serial_Init();
	
	OLED_ShowString(1,1,"Zener Senser:");
	
	Delay_ms(1000);
	OLED_Clear();
	
	float Vin;
	
	while (1)
	{
//		float adc0;
//		float adc1;

//		adc0 = AD_Average(ADC_Channel_0,20);
//		adc1 = AD_Average(ADC_Channel_1,20);

//		OLED_ShowString(1,1,"A0:");
//		OLED_ShowNum(1,4,(uint16_t)adc0,1);
//		OLED_ShowChar(1,5,'.');
//		OLED_ShowNum(1,6,(uint16_t)(adc0*100)%100,2);

//		OLED_ShowString(2,1,"A1:");
//		OLED_ShowNum(2,4,(uint16_t)adc1,1);
//		OLED_ShowChar(2,5,'.');
//		OLED_ShowNum(2,6,(uint16_t)(adc1*100)%100,2);

//		Delay_ms(200);
		
		Delay_ms(200);
		
		value=(AD_Average(ADC_Channel_1,20)*11.0f);       //采集稳压管两端电压
//		if(value > 1.0f)
//		{
//			value *= ZENER_CAL;
//		}
		
		if(value<0.3f)
		{
			status=ZENER_BROKEN;                //电压过低，可能短路
		}
		else if(value<1.0f)
		{
			status=ZENER_FORWARD;               //正向导通，稳压管接反或击穿
		}
		else if(value>8.0f)
		{
			status=ZENER_OPEN;                  //开路或没接
		}
		else
		{
			status=ZENER_OK;                    //正常稳压
		}
		
		sprintf(Serial_buf,"value:%.2fV | Status: ",value);
		Serial_SendString(Serial_buf);
		if(status==ZENER_OK)
		{
			Serial_SendString("OK\r\n");
		}
		else if(status==ZENER_FORWARD)
		{
			Serial_SendString("FORWORD\r\n");
		}
		else if(status==ZENER_BROKEN)
		{
			Serial_SendString("BROKEN\r\n");
		}
		else
		{
			Serial_SendString("OPEN\r\n");
		}
		
		int_part=(int)value;
		frac_part=(int)((value-int_part)*100);
		OLED_ShowString(1,1,"V:");
		OLED_ShowNum(1,4,int_part,2);
		OLED_ShowChar(1,6,'.');
		OLED_ShowNum(1,7,frac_part,2);
		OLED_ShowString(1,9,"V");
		
		Vin =AD_Average(ADC_Channel_0,20) * 11.0f;

		OLED_ShowString(2,1,"VIN:");
		OLED_ShowNum(2,5,(uint16_t)Vin,2);
		OLED_ShowChar(2,7,'.');
		OLED_ShowNum(2,8,(uint16_t)(Vin*100)%100,2);
		OLED_ShowString(2,10,"V");
		Delay_ms(200);
		
		OLED_ShowString(3,1, "Status:");
		if(status==ZENER_OK)
		{
			OLED_ShowString(4,1,"OK     ");
		}
		else if(status==ZENER_FORWARD)
		{
			OLED_ShowString(4,1,"FORWARD");
		}
		else if(status==ZENER_BROKEN)
		{
			OLED_ShowString(4,1,"BROKEN ");
		}
		else
		{
			OLED_ShowString(4,1,"OPEN   ");
		}
		Delay_ms(500);
	}
}
