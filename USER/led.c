#include "led.h"
//该函数用于初始化LED红灯DS0
void LED_Init(void)
{    	 
	//初始化结构体变量
  GPIO_InitTypeDef  GPIO_InitStructure;
	//时钟使能
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
  //GPIOF9,F10初始化设置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	//普通输出模式
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	//推挽输出
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	//GPIO口输出频率100MHz
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	//上拉电阻
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	//初始化
  GPIO_Init(GPIOF, &GPIO_InitStructure);
	//程序启动时先让两个灯灭
	GPIO_SetBits(GPIOF,GPIO_Pin_9);
}
