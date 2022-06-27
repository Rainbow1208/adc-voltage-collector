//包含需要的头文件
#include "sys.h"			//系统文件
#include "delay.h"		//延时函数声明
#include "usart.h"		//串口声明
#include "led.h"			//LED灯声明
#include "lcd.h"			//LCD屏幕声明
#include "key.h"			//按键声明
#include "adc.h"			//ADC通道声明
#include "rtc.h"			//RTC时钟声明
#include "stmflash.h"	//FLASH声明
#include "beep.h"			//蜂鸣器声明
//声明FLASH使用的扇区地址，我使用了三块扇区分别保存电压、时间和日期
#define ADDR1  0X08040000		//电压保存地址
#define ADDR2  0X08060000		//时间保存地址
#define ADDR3  0X08080000 	//日期保存地址
//声明要写入FLASH的数据长度
//时间
#define T_LENTH sizeof(t)	 		  	
#define TSIZE T_LENTH/4+((T_LENTH%4)?1:0)
//日期
#define D_LENTH sizeof(d)	 		  
#define DSIZE D_LENTH/4+((D_LENTH%4)?1:0)
//电压以及最后一次保存数据的指针
#define OP_LENTH sizeof(op)	 		  
#define OPSIZE OP_LENTH/4+((OP_LENTH%4)?1:0)

//主函数
int main(void)
{
	//设置系统中断优先级分组2
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	u8 x=0;																	//x变量用于判断是否控制蜂鸣器叫和灯的闪烁
	u16 k=0;																//k是最后一次保存数据的指针（不是真的指针，指的是0-9内的一个数，用于判断保存到了第几个数）
	u8 tbuf[40];														//tbuf是用于保存时间日期的数组
	RTC_TimeTypeDef RTC_TimeStruct;					//RTC时间结构体，后续用于获取时间
	RTC_DateTypeDef RTC_DateStruct;					//RTC日期结构体，后续用于获取日期
	u16 op[11]={0};													//op变量用于保存十个电压值，最后一个值用于保存数据指针k
	STMFLASH_Read(ADDR1,(u32*)op,OPSIZE);		//从ADDR1扇区读取之前保存的op变量
	if(op[10]>9)														//判断op最后一个变量，即上一次保存的k的值是否超出了范围，超出了则初始化，未超出则直接赋值
	{k=0;}
	else
	{k=op[10];}
	RTC_TimeTypeDef t[10]={0};							//声明RTC时间结构体变量t,方便后续向FLASH写入时间，宏定义里的t变量长度即是这里的t变量
	RTC_DateTypeDef d[10]={0};							//声明RTC日期结构体变量t,方便后续向FLASH写入日期，宏定义里的d变量长度即是这里的d变量
	u16 adcx;																//adcx用于保存ADC采集到的电压值
	float temp;															//temp用于电压值转换计算时的中间变量
	delay_init(168);      									//初始化延时函数，系统时钟168
	uart_init(115200);											//初始化串口,波特率为115200
	Adc_Init();															//初始化ADC
	KEY_Init();															//初始化按键
	LED_Init();					  									//初始化LED灯
 	LCD_Init();           									//初始化LCD屏幕
	My_RTC_Init();													//初始化RTC时钟
	BEEP_Init();													  //初始化蜂鸣器
	POINT_COLOR=RED;      									//设置LCD的画笔为红色
	u8 key;																	//变量key用于保存按下的按键是哪一个
	GPIO_SetBits(GPIOF,GPIO_Pin_9);					//LED灯灭
	
	//程序主循环
	while(1)
	{
		label_1:LCD_Clear(WHITE);							//用白色清屏，label_1用于后续跳转使用
																					//输出个人信息和提示（key0按下时的主界面）
		LCD_ShowString(30,30,400,24,24,"NJUST SME Voltage measurement");	
		LCD_ShowString(30,60,400,24,24,"919101960122 LuShiHao");
		LCD_ShowString(30,110,400,24,24,"Press key to continue");
		LCD_ShowString(30,140,400,24,24,"Key1:Show the latest 10 data saved");
		LCD_ShowString(30,170,400,24,24,"Key2:Start collecting voltage");
		//循环扫描按键
		label_2:while(1)
		{
			//保存当前的按键值
			key=KEY_Scan(0);										
			//如果按键被按下
			if(key)															
		{						   
			switch(key)
			{			
				//如果按键0被按下
        case KEY0_PRES:
					  //LED灯灭 蜂鸣器关	
						GPIO_SetBits(GPIOF,GPIO_Pin_9);    	
						x=0;                                              
						//依次擦除三个扇区方便后续写入
						FLASH_EraseSector(FLASH_Sector_6,VoltageRange_3);	
						FLASH_EraseSector(FLASH_Sector_7,VoltageRange_3);
						FLASH_EraseSector(FLASH_Sector_8,VoltageRange_3);
						//向FLASH写入电压、时间和日期,数据长度为前面定义的宏
						STMFLASH_Write(ADDR1,(u32*)op,OPSIZE);						
						STMFLASH_Write(ADDR2,(u32*)t,TSIZE);
						STMFLASH_Write(ADDR3,(u32*)d,DSIZE);
						//返回标签1输出主界面信息
						goto label_1;
				
				//如果按键1被按下
				case KEY1_PRES:	
						//LED灯灭 蜂鸣器关	
						GPIO_SetBits(GPIOF,GPIO_Pin_9);
						x=0;			
						//LED用白色清屏
						LCD_Clear(WHITE);
						//显示个人信息和表头信息
						LCD_ShowString(30,30,400,24,24,"NJUST SME Voltage measurement");	
						LCD_ShowString(30,60,400,24,24,"919101960122 LuShiHao");
						LCD_ShowString(30,110,400,24,24,"Voltage(V)");
						LCD_ShowString(210,110,400,24,24,"Time");
						//从FLASH的三个扇区中读取保存的电压、时间和日期数据，保存在op、d和t中
						STMFLASH_Read(ADDR1,(u32*)op,OPSIZE);
						STMFLASH_Read(ADDR3,(u32*)d,DSIZE);
						STMFLASH_Read(ADDR2,(u32*)t,TSIZE);
						//该循环用于显示刚刚读取的数据
						for(u32 i=0;i<=9;i++)
						{
							//取得一个电压数据
							adcx=op[i];
							//获取计算后的带小数的实际电压值
							temp=(float)adcx*(3.3/4096);          
							adcx=temp;
							//显示电压整数值和小数点
							LCD_ShowxNum(30,140+30*i,adcx,1,24,0);  
							LCD_ShowChar(47,140+30*i,'.',24,0);
							//计算出电压的小数点后面的值
							temp-=adcx;                           
							temp*=1000;     	
							//显示出电压的小数点后面的值
							LCD_ShowxNum(52,140+30*i,temp,3,24,0X80);
							//取得一个时间数据
							RTC_TimeStruct=t[i];
							//将时间数据转换为字符串
							sprintf((char*)tbuf,"%02d:%02d:%02d",RTC_TimeStruct.RTC_Hours,RTC_TimeStruct.RTC_Minutes,RTC_TimeStruct.RTC_Seconds);
							//显示时间
							LCD_ShowString(210,140+30*i,210,24,24,tbuf);
							//取得一个日期数据
							RTC_DateStruct=d[i];
							//将日期数据转换为字符串
							sprintf((char*)tbuf,"20%02d-%02d-%02d",RTC_DateStruct.RTC_Year,RTC_DateStruct.RTC_Month,RTC_DateStruct.RTC_Date);
							//显示日期
							LCD_ShowString(330,140+30*i,210,24,24,tbuf);
						}
						//输出提示信息
						LCD_ShowString(30,440,400,24,24,"Press key to continue");
						LCD_ShowString(30,470,400,24,24,"Key0:Return menu and save");
						LCD_ShowString(30,500,400,24,24,"Key1:Show the latest 10 data saved");
						LCD_ShowString(30,530,400,24,24,"Key2:Start collecting voltage");
						//返回按键扫描循环的位置，重新开始扫描是否有按键按下
						goto label_2;
				
				//如果按键2被按下
				case KEY2_PRES:
						//LED灯灭 蜂鸣器关	
						GPIO_SetBits(GPIOF,GPIO_Pin_9);
						x=0;
						//获取ADC通道5的电压转换值，20次取平均
						adcx=Get_Adc_Average(ADC_Channel_5,20);
						//获得当前的时间，保存到RTC_TimeStruct结构体变量中
						RTC_GetTime(RTC_Format_BIN,&RTC_TimeStruct);
						//获取计算后的带小数的实际电压值
						temp=(float)adcx*(3.3/4096);
						//该变量用于向串口发送数据
						char a[10];
						//第一个字符为空，因为电脑上的串口软件显示不出来第一个字符
						a[0]=' ';
						//根据电压的值确定第二个字符
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
						//计算电压小数点后的值
						int temp1=(int)(temp*1000);
						//用取模数的方法获取小数点后的字符（ASCII码为当前的数加48，串口软件只显示ASCII码）
						a[4]=(char)(temp1%10+48);
						temp1=temp1/10;
						a[3]=(char)(temp1%10+48);
						temp1=temp1/10;
						a[2]=(char)(temp1%10+48);
						a[5]='.';
						//m变量用于判断电压范围，以决定LED和蜂鸣器的动作
						float m=temp;
						//若电压大于2，则蜂鸣器响10Hz，并且记录这个电压值用于后续保存
						if(m>2)
						{
							x=1;
							op[k]=adcx;
						}
						//若电压大于1小于2，则蜂鸣器响20Hz，并且记录这个电压值用于后续保存
						else if(m>1)
						{
							x=2;
						}
						adcx=temp;
						//显示测量的电压和时间日期
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
						//向串口发送测量到的电压值
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
						//获取当前时间
						RTC_GetTime(RTC_Format_BIN,&RTC_TimeStruct);
						//转换为字符串
						sprintf((char*)tbuf,"%02d:%02d:%02d",RTC_TimeStruct.RTC_Hours,RTC_TimeStruct.RTC_Minutes,RTC_TimeStruct.RTC_Seconds); 
						//若电压值大于3则向数组保存当前时间
						if(m>2)
						{t[k]=RTC_TimeStruct;}
						//显示当前的时间
						LCD_ShowString(210,140,210,24,24,tbuf);	
						//获取当前日期
						RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);
						//转换为字符串
						sprintf((char*)tbuf,"20%02d-%02d-%02d",RTC_DateStruct.RTC_Year,RTC_DateStruct.RTC_Month,RTC_DateStruct.RTC_Date);	
						//若电压值大于3则向数组保存当前日期
						if(m>2)
						{d[k]=RTC_DateStruct;}
						//显示当前的日期
						LCD_ShowString(330,140,210,24,24,tbuf);
						//输出提示信息
						LCD_ShowString(30,190,400,24,24,"Press key to continue");
						LCD_ShowString(30,220,400,24,24,"Key0:Return menu and save");
						LCD_ShowString(30,250,400,24,24,"Key2:Start collecting voltage");
						//若电压大于3则保存当前电压值
						if(m>2)
						{
							//指针指向数组下一个数
							k=k+1;
							//判断数组是否超出范围，超出范围则重置
							if(k>9){k=0;}
							//保存指针k
							op[10]=k;
						}
						//返回按键扫描的位置
						goto label_2;
				}
		}
		else 
			//延时10ms等待下次按键扫描
			delay_ms(10);
		//若x=1则蜂鸣器10Hz响，LED灯闪烁
		if(x==1)
		{
			GPIO_SetBits(GPIOF,GPIO_Pin_9);
			GPIO_SetBits(GPIOF,GPIO_Pin_8);
			delay_ms(50);
			GPIO_ResetBits(GPIOF,GPIO_Pin_8);
			delay_ms(50);
			GPIO_ResetBits(GPIOF,GPIO_Pin_9);
		}
		//若x=1则蜂鸣器20Hz响，LED灯灭
		else if(x==2)
		{
			GPIO_SetBits(GPIOF,GPIO_Pin_8);
			delay_ms(100);
			GPIO_ResetBits(GPIOF,GPIO_Pin_8);
			delay_ms(100);
		}
		//否则蜂鸣器不响
		else
		{
			GPIO_ResetBits(GPIOF,GPIO_Pin_8);
		}
		}
	}
}

