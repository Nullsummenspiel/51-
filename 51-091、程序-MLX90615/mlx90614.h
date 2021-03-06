#include<reg52.h> //包含头文件，一般情况不需要改动，头文件包含特殊功能寄存器的定义		 
#include<intrins.h>
#include "delay.h"


//mlx90614 端口定义
sbit SCL=P1^1;//时钟线
sbit SDA=P1^2;//数据线

void start_bit(); //MLX90614 发起始位子程序
void stop_bit(); //MLX90614 发结束位子程序
unsigned char rx_byte(void); //MLX90614 接收字节子程序
void send_bit(void); //MLX90614 发送位子程序
void tx_byte(unsigned char dat_byte); //MLX90614 接收字节子程序
void receive_bit(void); //MLX90614 接收位子程序
unsigned int ReadObjectTemp(void); //读温度数据
unsigned int ReadEnvironTemp(void);