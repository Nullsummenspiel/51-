#include<reg52.h> //����ͷ�ļ���һ���������Ҫ�Ķ���ͷ�ļ��������⹦�ܼĴ����Ķ���
#include<stdio.h>		 
#include<intrins.h>
#include "delay.h"
#include "1602.h"
#include "mlx90614.h"

sbit buzzer=P1^0; //���Ŷ���
sbit key1=P2^0;
sbit key2=P2^1;
sbit key3=P3^2;

unsigned long time_ms=0xaaaaaaaa; 	//	ϵͳ��ʱ����
char dis0[16];			  //Һ��������ʾ�ݴ�
char dis1[16];
unsigned int setNum = 31; //����ֵ
//---------------------------------------
//Name: CALTEMP           
//Temperature data is T=(Data)*0.02-273.15
//---------------------------------------
bit rekey=0;
bit flag0 = 0;//��������ʾ��־λ
bit flag =0;  //����������ʾ��־λ
bit flag1 = 0;	   //��������־λ
bit flag2 = 1;		  //�Ƿ���Ҫҳ���һ��־λ
bit disFlag =0;//������ʾ��־
unsigned int  Tem;	   //�¶�
float objTemp,envTemp;	 //�����¶Ⱥͻ����¶�
xdata float objtemps[5] = 0;
xdata float envtemps[5] = 0;
xdata unsigned int i = 0;
void Init_Timer0(void);	 //��������

void main (void)
{   
	Init_Timer0();        //��ʱ��0��ʼ��
	             
	LCD_Init();           //��ʼ��Һ��
	DelayMs(20);          //��ʱ�������ȶ�
	LCD_Clear();
	IT0=1;
	EX0=1;

	while (1)         //��ѭ��
	{
		if(disFlag==1)	   //��ʾ��־
		{
			if((flag1==0)&&(flag==0)&&(flag0==0))	//�����������־λ�ʹ�������־λ��Ϊ0ʱ��ʾ������
			{
				LCD_Clear();
				if(i>5)
				{i=0;}
				sprintf(dis0," Hello  %2d",i);
				LCD_Write_String(0,0,dis0);
				flag0=1;
			}
			if((key1==0)&&(flag1==0))				 //�����°���1
			{
				DelayMs(300);
				LCD_Clear();
				Tem=ReadObjectTemp();	 //��ȡʵ���¶�
				objTemp=(float)(Tem)*0.02-273.15;
				DelayMs(300);
				Tem=ReadEnvironTemp();	 //��ȡ�����¶�
				envTemp=(float)(Tem)*0.02-273.15;

				sprintf(dis0,"E: %4.1f'C   ",envTemp);//��ӡ�¶�ֵ
				LCD_Write_String(0,0,dis0);//��ʾ
				sprintf(dis1,"O:%5.1f'C   %3d  ",objTemp,setNum);//��ӡ�¶�����ֵ
				LCD_Write_String(0,1,dis1);//��ʾ
				flag=1;							  //��ʾ��������
			}
			if(objTemp>setNum)
			{buzzer =0;	}	//�¶ȸ߷���������
			else
			{buzzer=1;}		//�¶ȵ� ֹͣ
			disFlag = 0;
			if((key2==0)&&(flag==1))	 //�����°���2�Ҵ��ڲ�������
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
						flag=0;	flag0=0;				 //ʹ��־λ�����������״̬
						flag1=0;
						rekey=1;
						flag2=0;				 //ҳ���һ��־λ����
					}
				}
				
			}
			else
			{
				rekey=0;
			}
			DelayMs(300);
			if((key2==0)&&(flag==0))	   //�����������水�°�ť2
			{
				if(rekey==0)
				{
					if(key2==0)
					{
						if(flag2==0)		 //���ҳ���һ��־λ������ʹҳ����һ
						{i=i-1;}
						LCD_Clear();
						DelayMs(600);
						flag1=1;			 //�����־λ��1��һֱ��ʾ�������
						if(i>5)
						{i=0;}
						sprintf(dis0,"E: %4.1f'C page:%1d",envtemps[i],i);//��ӡ�¶�ֵ
						LCD_Write_String(0,0,dis0);//��ʾ
						sprintf(dis1,"O:%5.1f'C   %3d  ",objtemps[i],setNum);//��ӡ�¶�����ֵ
						LCD_Write_String(0,1,dis1);//��ʾ
						i=i+1;
						rekey=1;
						flag2=1;			  //ʹҳ���һ��־λ��2������ִ�м�һ����
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
	TMOD |= 0x01;	  //ʹ��ģʽ1��16λ��ʱ����ʹ��"|"���ſ�����ʹ�ö����ʱ��ʱ����Ӱ��		     
	TH0=(65536-7000)/256;		  //���¸�ֵ 20ms
	TL0=(65536-7000)%256;
	EA=1;            //���жϴ�
	ET0=1;           //��ʱ���жϴ�
	TR0=1;           //��ʱ�����ش�
}

void Timer0_isr(void) interrupt 1 
{
	TH0=(65536-7000)/256;		  //���¸�ֵ 20ms
	TL0=(65536-7000)%256;	
	time_ms++;
	if(time_ms%7==0)	  //��ʱ������ʾ
	{disFlag = 1;}
}
void Int0_ISR(void) interrupt 0
{
		flag = 0;
		flag1 = 0;
		flag0 = 0;
}
