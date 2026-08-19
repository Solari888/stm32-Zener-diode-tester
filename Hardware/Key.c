#include "stm32f10x.h"                  // Device header
#include "Delay.h"

void Key_Init(void)            //初始化按键
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//读取按键，上拉输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
}

uint8_t Key_GetNum(void)
{
	uint8_t KeyNum =0;//unsigned char 默认为0，若无按键按下，即返回0
	//GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1)     //读取PB1端口的值，返回值为输入寄存器某一位的值，0为低电平，1为高电平
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1)==0)   //按键按下
	{
		Delay_ms(20);//消除刚按下按键时的抖动
		while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1)==0);//如果按键一直按下，就卡在这里，直到松手？等待按键释放，死循环
		Delay_ms(20);
		KeyNum =1;
	}
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11)==0)   //按键按下
	{
		Delay_ms(20);//消除刚按下按键时的抖动
		while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11)==0);//如果按键一直按下，就卡在这里，直到松手？
		Delay_ms(20);
		KeyNum =2;
	}
	
	return KeyNum;
}
