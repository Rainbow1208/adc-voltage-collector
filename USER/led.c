#include "led.h"
//�ú������ڳ�ʼ��LED���DS0
void LED_Init(void)
{    	 
	//��ʼ���ṹ�����
  GPIO_InitTypeDef  GPIO_InitStructure;
	//ʱ��ʹ��
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
  //GPIOF9,F10��ʼ������
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	//��ͨ���ģʽ
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	//�������
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	//GPIO�����Ƶ��100MHz
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	//��������
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	//��ʼ��
  GPIO_Init(GPIOF, &GPIO_InitStructure);
	//��������ʱ������������
	GPIO_SetBits(GPIOF,GPIO_Pin_9);
}
