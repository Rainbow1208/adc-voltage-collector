//������Ҫ��ͷ�ļ�
#include "sys.h"			//ϵͳ�ļ�
#include "delay.h"		//��ʱ��������
#include "usart.h"		//��������
#include "led.h"			//LED������
#include "lcd.h"			//LCD��Ļ����
#include "key.h"			//��������
#include "adc.h"			//ADCͨ������
#include "rtc.h"			//RTCʱ������
#include "stmflash.h"	//FLASH����
#include "beep.h"			//����������
//����FLASHʹ�õ�������ַ����ʹ�������������ֱ𱣴��ѹ��ʱ�������
#define ADDR1  0X08040000		//��ѹ�����ַ
#define ADDR2  0X08060000		//ʱ�䱣���ַ
#define ADDR3  0X08080000 	//���ڱ����ַ
//����Ҫд��FLASH�����ݳ���
//ʱ��
#define T_LENTH sizeof(t)	 		  	
#define TSIZE T_LENTH/4+((T_LENTH%4)?1:0)
//����
#define D_LENTH sizeof(d)	 		  
#define DSIZE D_LENTH/4+((D_LENTH%4)?1:0)
//��ѹ�Լ����һ�α������ݵ�ָ��
#define OP_LENTH sizeof(op)	 		  
#define OPSIZE OP_LENTH/4+((OP_LENTH%4)?1:0)

//������
int main(void)
{
	//����ϵͳ�ж����ȼ�����2
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	u8 x=0;																	//x���������ж��Ƿ���Ʒ������к͵Ƶ���˸
	u16 k=0;																//k�����һ�α������ݵ�ָ�루�������ָ�룬ָ����0-9�ڵ�һ�����������жϱ��浽�˵ڼ�������
	u8 tbuf[40];														//tbuf�����ڱ���ʱ�����ڵ�����
	RTC_TimeTypeDef RTC_TimeStruct;					//RTCʱ��ṹ�壬�������ڻ�ȡʱ��
	RTC_DateTypeDef RTC_DateStruct;					//RTC���ڽṹ�壬�������ڻ�ȡ����
	u16 op[11]={0};													//op�������ڱ���ʮ����ѹֵ�����һ��ֵ���ڱ�������ָ��k
	STMFLASH_Read(ADDR1,(u32*)op,OPSIZE);		//��ADDR1������ȡ֮ǰ�����op����
	if(op[10]>9)														//�ж�op���һ������������һ�α����k��ֵ�Ƿ񳬳��˷�Χ�����������ʼ����δ������ֱ�Ӹ�ֵ
	{k=0;}
	else
	{k=op[10];}
	RTC_TimeTypeDef t[10]={0};							//����RTCʱ��ṹ�����t,���������FLASHд��ʱ�䣬�궨�����t�������ȼ��������t����
	RTC_DateTypeDef d[10]={0};							//����RTC���ڽṹ�����t,���������FLASHд�����ڣ��궨�����d�������ȼ��������d����
	u16 adcx;																//adcx���ڱ���ADC�ɼ����ĵ�ѹֵ
	float temp;															//temp���ڵ�ѹֵת������ʱ���м����
	delay_init(168);      									//��ʼ����ʱ������ϵͳʱ��168
	uart_init(115200);											//��ʼ������,������Ϊ115200
	Adc_Init();															//��ʼ��ADC
	KEY_Init();															//��ʼ������
	LED_Init();					  									//��ʼ��LED��
 	LCD_Init();           									//��ʼ��LCD��Ļ
	My_RTC_Init();													//��ʼ��RTCʱ��
	BEEP_Init();													  //��ʼ��������
	POINT_COLOR=RED;      									//����LCD�Ļ���Ϊ��ɫ
	u8 key;																	//����key���ڱ��水�µİ�������һ��
	GPIO_SetBits(GPIOF,GPIO_Pin_9);					//LED����
	
	//������ѭ��
	while(1)
	{
		label_1:LCD_Clear(WHITE);							//�ð�ɫ������label_1���ں�����תʹ��
																					//���������Ϣ����ʾ��key0����ʱ�������棩
		LCD_ShowString(30,30,400,24,24,"NJUST SME Voltage measurement");	
		LCD_ShowString(30,60,400,24,24,"919101960122 LuShiHao");
		LCD_ShowString(30,110,400,24,24,"Press key to continue");
		LCD_ShowString(30,140,400,24,24,"Key1:Show the latest 10 data saved");
		LCD_ShowString(30,170,400,24,24,"Key2:Start collecting voltage");
		//ѭ��ɨ�谴��
		label_2:while(1)
		{
			//���浱ǰ�İ���ֵ
			key=KEY_Scan(0);										
			//�������������
			if(key)															
		{						   
			switch(key)
			{			
				//�������0������
        case KEY0_PRES:
					  //LED���� ��������	
						GPIO_SetBits(GPIOF,GPIO_Pin_9);    	
						x=0;                                              
						//���β������������������д��
						FLASH_EraseSector(FLASH_Sector_6,VoltageRange_3);	
						FLASH_EraseSector(FLASH_Sector_7,VoltageRange_3);
						FLASH_EraseSector(FLASH_Sector_8,VoltageRange_3);
						//��FLASHд���ѹ��ʱ�������,���ݳ���Ϊǰ�涨��ĺ�
						STMFLASH_Write(ADDR1,(u32*)op,OPSIZE);						
						STMFLASH_Write(ADDR2,(u32*)t,TSIZE);
						STMFLASH_Write(ADDR3,(u32*)d,DSIZE);
						//���ر�ǩ1�����������Ϣ
						goto label_1;
				
				//�������1������
				case KEY1_PRES:	
						//LED���� ��������	
						GPIO_SetBits(GPIOF,GPIO_Pin_9);
						x=0;			
						//LED�ð�ɫ����
						LCD_Clear(WHITE);
						//��ʾ������Ϣ�ͱ�ͷ��Ϣ
						LCD_ShowString(30,30,400,24,24,"NJUST SME Voltage measurement");	
						LCD_ShowString(30,60,400,24,24,"919101960122 LuShiHao");
						LCD_ShowString(30,110,400,24,24,"Voltage(V)");
						LCD_ShowString(210,110,400,24,24,"Time");
						//��FLASH�����������ж�ȡ����ĵ�ѹ��ʱ����������ݣ�������op��d��t��
						STMFLASH_Read(ADDR1,(u32*)op,OPSIZE);
						STMFLASH_Read(ADDR3,(u32*)d,DSIZE);
						STMFLASH_Read(ADDR2,(u32*)t,TSIZE);
						//��ѭ��������ʾ�ոն�ȡ������
						for(u32 i=0;i<=9;i++)
						{
							//ȡ��һ����ѹ����
							adcx=op[i];
							//��ȡ�����Ĵ�С����ʵ�ʵ�ѹֵ
							temp=(float)adcx*(3.3/4096);          
							adcx=temp;
							//��ʾ��ѹ����ֵ��С����
							LCD_ShowxNum(30,140+30*i,adcx,1,24,0);  
							LCD_ShowChar(47,140+30*i,'.',24,0);
							//�������ѹ��С��������ֵ
							temp-=adcx;                           
							temp*=1000;     	
							//��ʾ����ѹ��С��������ֵ
							LCD_ShowxNum(52,140+30*i,temp,3,24,0X80);
							//ȡ��һ��ʱ������
							RTC_TimeStruct=t[i];
							//��ʱ������ת��Ϊ�ַ���
							sprintf((char*)tbuf,"%02d:%02d:%02d",RTC_TimeStruct.RTC_Hours,RTC_TimeStruct.RTC_Minutes,RTC_TimeStruct.RTC_Seconds);
							//��ʾʱ��
							LCD_ShowString(210,140+30*i,210,24,24,tbuf);
							//ȡ��һ����������
							RTC_DateStruct=d[i];
							//����������ת��Ϊ�ַ���
							sprintf((char*)tbuf,"20%02d-%02d-%02d",RTC_DateStruct.RTC_Year,RTC_DateStruct.RTC_Month,RTC_DateStruct.RTC_Date);
							//��ʾ����
							LCD_ShowString(330,140+30*i,210,24,24,tbuf);
						}
						//�����ʾ��Ϣ
						LCD_ShowString(30,440,400,24,24,"Press key to continue");
						LCD_ShowString(30,470,400,24,24,"Key0:Return menu and save");
						LCD_ShowString(30,500,400,24,24,"Key1:Show the latest 10 data saved");
						LCD_ShowString(30,530,400,24,24,"Key2:Start collecting voltage");
						//���ذ���ɨ��ѭ����λ�ã����¿�ʼɨ���Ƿ��а�������
						goto label_2;
				
				//�������2������
				case KEY2_PRES:
						//LED���� ��������	
						GPIO_SetBits(GPIOF,GPIO_Pin_9);
						x=0;
						//��ȡADCͨ��5�ĵ�ѹת��ֵ��20��ȡƽ��
						adcx=Get_Adc_Average(ADC_Channel_5,20);
						//��õ�ǰ��ʱ�䣬���浽RTC_TimeStruct�ṹ�������
						RTC_GetTime(RTC_Format_BIN,&RTC_TimeStruct);
						//��ȡ�����Ĵ�С����ʵ�ʵ�ѹֵ
						temp=(float)adcx*(3.3/4096);
						//�ñ��������򴮿ڷ�������
						char a[10];
						//��һ���ַ�Ϊ�գ���Ϊ�����ϵĴ��������ʾ��������һ���ַ�
						a[0]=' ';
						//���ݵ�ѹ��ֵȷ���ڶ����ַ�
						if(temp>5)
						{a[1]='5';}
						else if(temp>4)
						{a[1]='4';}
						else if(temp>3)
						{a[1]='3';}
						else if(temp>2)
						{a[1]='2';}
						else if(temp>1)
						{a[1]='1';}
						else
						{a[1]='0';}
						//�����ѹС������ֵ
						int temp1=(int)(temp*1000);
						//��ȡģ���ķ�����ȡС�������ַ���ASCII��Ϊ��ǰ������48���������ֻ��ʾASCII�룩
						a[4]=(char)(temp1%10+48);
						temp1=temp1/10;
						a[3]=(char)(temp1%10+48);
						temp1=temp1/10;
						a[2]=(char)(temp1%10+48);
						a[5]='.';
						//m���������жϵ�ѹ��Χ���Ծ���LED�ͷ������Ķ���
						float m=temp;
						//����ѹ����2�����������10Hz�����Ҽ�¼�����ѹֵ���ں�������
						if(m>2)
						{
							x=1;
							op[k]=adcx;
						}
						//����ѹ����1С��2�����������20Hz�����Ҽ�¼�����ѹֵ���ں�������
						else if(m>1)
						{
							x=2;
						}
						adcx=temp;
						//��ʾ�����ĵ�ѹ��ʱ������
						LCD_Clear(WHITE);
						LCD_ShowString(30,30,400,24,24,"NJUST SME Voltage measurement");	
						LCD_ShowString(30,60,400,24,24,"919101960122 LuShiHao");
						LCD_ShowString(30,110,400,24,24,"Voltage(V)");
						LCD_ShowString(210,110,400,24,24,"Time");
						LCD_ShowxNum(30,140,adcx,1,24,0);  
						LCD_ShowChar(47,140,'.',24,0);
						temp-=adcx;                           
						temp*=1000;     		
						LCD_ShowxNum(52,140,temp,3,24,0X80); 
						//�򴮿ڷ��Ͳ������ĵ�ѹֵ
						USART_SendData(USART1,a[0]);
						while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);
						USART_SendData(USART1,a[1]);
						while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);
						USART_SendData(USART1,a[5]);
						while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);
						USART_SendData(USART1,a[2]);
						while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);
						USART_SendData(USART1,a[3]);
						while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);
						USART_SendData(USART1,a[4]);
						while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);
						printf("\r\n\r\n");
						//��ȡ��ǰʱ��
						RTC_GetTime(RTC_Format_BIN,&RTC_TimeStruct);
						//ת��Ϊ�ַ���
						sprintf((char*)tbuf,"%02d:%02d:%02d",RTC_TimeStruct.RTC_Hours,RTC_TimeStruct.RTC_Minutes,RTC_TimeStruct.RTC_Seconds); 
						//����ѹֵ����3�������鱣�浱ǰʱ��
						if(m>2)
						{t[k]=RTC_TimeStruct;}
						//��ʾ��ǰ��ʱ��
						LCD_ShowString(210,140,210,24,24,tbuf);	
						//��ȡ��ǰ����
						RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);
						//ת��Ϊ�ַ���
						sprintf((char*)tbuf,"20%02d-%02d-%02d",RTC_DateStruct.RTC_Year,RTC_DateStruct.RTC_Month,RTC_DateStruct.RTC_Date);	
						//����ѹֵ����3�������鱣�浱ǰ����
						if(m>2)
						{d[k]=RTC_DateStruct;}
						//��ʾ��ǰ������
						LCD_ShowString(330,140,210,24,24,tbuf);
						//�����ʾ��Ϣ
						LCD_ShowString(30,190,400,24,24,"Press key to continue");
						LCD_ShowString(30,220,400,24,24,"Key0:Return menu and save");
						LCD_ShowString(30,250,400,24,24,"Key2:Start collecting voltage");
						//����ѹ����3�򱣴浱ǰ��ѹֵ
						if(m>2)
						{
							//ָ��ָ��������һ����
							k=k+1;
							//�ж������Ƿ񳬳���Χ��������Χ������
							if(k>9){k=0;}
							//����ָ��k
							op[10]=k;
						}
						//���ذ���ɨ���λ��
						goto label_2;
				}
		}
		else 
			//��ʱ10ms�ȴ��´ΰ���ɨ��
			delay_ms(10);
		//��x=1�������10Hz�죬LED����˸
		if(x==1)
		{
			GPIO_SetBits(GPIOF,GPIO_Pin_9);
			GPIO_SetBits(GPIOF,GPIO_Pin_8);
			delay_ms(50);
			GPIO_ResetBits(GPIOF,GPIO_Pin_8);
			delay_ms(50);
			GPIO_ResetBits(GPIOF,GPIO_Pin_9);
		}
		//��x=1�������20Hz�죬LED����
		else if(x==2)
		{
			GPIO_SetBits(GPIOF,GPIO_Pin_8);
			delay_ms(100);
			GPIO_ResetBits(GPIOF,GPIO_Pin_8);
			delay_ms(100);
		}
		//�������������
		else
		{
			GPIO_ResetBits(GPIOF,GPIO_Pin_8);
		}
		}
	}
}

