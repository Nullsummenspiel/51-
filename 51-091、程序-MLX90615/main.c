#include<reg52.h> //包含头文件，一般情况不需要改动，头文件包含特殊功能寄存器的定义
#include<stdio.h>		 
#include<intrins.h>
#include "delay.h"
#include "1602.h"
#include "mlx90614.h"

sbit buzzer=P1^0; //引脚定义
sbit key1=P2^0;
sbit key2=P2^1;
sbit key3=P3^2;

unsigned long time_ms=0xaaaaaaaa; 	//	系统定时计数
char dis0[16];			  //液晶数组显示暂存
char dis1[16];
unsigned int setNum = 31; //设置值
//---------------------------------------
//Name: CALTEMP           
//Temperature data is T=(Data)*0.02-273.15
//---------------------------------------
bit rekey=0;
bit flag0 = 0;//主界面显示标志位
bit flag =0;  //测量界面显示标志位
bit flag1 = 0;	   //储存界面标志位
bit flag2 = 1;		  //是否需要页面减一标志位
bit disFlag =0;//更新显示标志
unsigned int  Tem;	   //温度
float objTemp,envTemp;	 //物体温度和环境温度
xdata float objtemps[5] = 0;
xdata float envtemps[5] = 0;
xdata unsigned int i = 0;
void Init_Timer0(void);	 //函数声明

void main (void)
{   
	Init_Timer0();        //定时器0初始化
	             
	LCD_Init();           //初始化液晶
	DelayMs(20);          //延时有助于稳定
	LCD_Clear();
	IT0=1;
	EX0=1;

	while (1)         //主循环
	{
		if(disFlag==1)	   //显示标志
		{
			if((flag1==0)&&(flag==0)&&(flag0==0))	//当测量界面标志位和储存界面标志位都为0时显示主界面
			{
				LCD_Clear();
				if(i>5)
				{i=0;}
				sprintf(dis0," Hello  %2d",i);
				LCD_Write_String(0,0,dis0);
				flag0=1;
			}
			if((key1==0)&&(flag1==0))				 //当按下按键1
			{
				DelayMs(300);
				LCD_Clear();
				Tem=ReadObjectTemp();	 //读取实物温度
				objTemp=(float)(Tem)*0.02-273.15;
				DelayMs(300);
				Tem=ReadEnvironTemp();	 //读取环境温度
				envTemp=(float)(Tem)*0.02-273.15;

				sprintf(dis0,"E: %4.1f'C   ",envTemp);//打印温度值
				LCD_Write_String(0,0,dis0);//显示
				sprintf(dis1,"O:%5.1f'C   %3d  ",objTemp,setNum);//打印温度设置值
				LCD_Write_String(0,1,dis1);//显示
				flag=1;							  //显示测量界面
			}
			if(objTemp>setNum)
			{buzzer =0;	}	//温度高蜂鸣器鸣叫
			else
			{buzzer=1;}		//温度低 停止
			disFlag = 0;
			if((key2==0)&&(flag==1))	 //当按下按键2且处于测量界面
			{
				if(rekey==0)
				{
					if(key2==0)
					{
						DelayMs(300);
						LCD_Clear();
						objtemps[i]=objTemp;
						envtemps[i]=envTemp;
						i=i+1;
						flag=0;	flag0=0;				 //使标志位返回主界面的状态
						flag1=0;
						rekey=1;
						flag2=0;				 //页面减一标志位就绪
					}
				}
				
			}
			else
			{
				rekey=0;
			}
			DelayMs(300);
			if((key2==0)&&(flag==0))	   //当处于主界面按下按钮2
			{
				if(rekey==0)
				{
					if(key2==0)
					{
						if(flag2==0)		 //如果页面减一标志位就绪则使页数减一
						{i=i-1;}
						LCD_Clear();
						DelayMs(600);
						flag1=1;			 //储存标志位置1，一直显示储存界面
						if(i>5)
						{i=0;}
						sprintf(dis0,"E: %4.1f'C page:%1d",envtemps[i],i);//打印温度值
						LCD_Write_String(0,0,dis0);//显示
						sprintf(dis1,"O:%5.1f'C   %3d  ",objtemps[i],setNum);//打印温度设置值
						LCD_Write_String(0,1,dis1);//显示
						i=i+1;
						rekey=1;
						flag2=1;			  //使页面减一标志位置2，不再执行减一操作
					}
				}
			}
			else
			{
				rekey=0;
			}
		}
		disFlag=0;					
	}
}


void Init_Timer0(void)
{
	TMOD |= 0x01;	  //使用模式1，16位定时器，使用"|"符号可以在使用多个定时器时不受影响		     
	TH0=(65536-7000)/256;		  //重新赋值 20ms
	TL0=(65536-7000)%256;
	EA=1;            //总中断打开
	ET0=1;           //定时器中断打开
	TR0=1;           //定时器开关打开
}

void Timer0_isr(void) interrupt 1 
{
	TH0=(65536-7000)/256;		  //重新赋值 20ms
	TL0=(65536-7000)%256;	
	time_ms++;
	if(time_ms%7==0)	  //定时更新显示
	{disFlag = 1;}
}
void Int0_ISR(void) interrupt 0
{
		flag = 0;
		flag1 = 0;
		flag0 = 0;
}
