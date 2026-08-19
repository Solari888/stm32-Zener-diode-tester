//存放驱动程序可以对外提供的函数或变量的声明
#ifndef __LED_H
#define __LED_H

void LED_Init(void);//对模块外部声明，说明该函数可被外部调用
void LED1_ON(void);
void LED1_OFF(void);
void LED1_Turn(void);
void LED2_ON(void);
void LED2_OFF(void);
void LED2_Turn(void);

#endif
