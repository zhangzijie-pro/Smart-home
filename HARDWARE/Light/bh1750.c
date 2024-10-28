#include "BH1750.h"
#include "iic.h"

void Single_Write_BH1750(char REG_Address)
{
   IIC_Start();                  //起始信号
   IIC_Send_Byte(BHAddWrite);   //发送设备地址+写信号
   IIC_Send_Byte(REG_Address);    //内部寄存器地址
   IIC_Stop();                   //发送停止信号
}

void BH1750_Init(void)
{
	IIC_Init();

	//Single_Write_BH1750(0x01);
	//bh_data_send(BHPowOn);  //BH1750上电
	//bh_data_send(BHReset);	//BH1750复位
	//bh_data_send(BHModeH2); //BH1750写入测量模式：选择的是分辨率是4lx，测量时间是16ms的
}

/*************************************************************************************/
void bh_data_send(u8 command)
{
    IIC_Start();                      //iic起始信号
    IIC_Send_Byte(BHAddWrite);       //发送器件地址
		IIC_Wait_Ack();
    IIC_Send_Byte(command);          //发送指令
    IIC_Wait_Ack();                   //等待从机应答
    IIC_Stop();                       //iic停止信号
}

u16 bh_data_read(void)
{
	u16 buf;
	IIC_Start();                       //iic起始信号
	IIC_Send_Byte(BHAddRead);         //发送器件地址+读标志位
	IIC_Wait_Ack();                     //等待从机应答
	buf=IIC_Read_Byte(1);              //读取数据
	buf=buf<<8;                        //读取并保存高八位数据
	buf+=0x00ff&IIC_Read_Byte(0);      //读取并保存第八位数据
	IIC_Stop();                        //发送停止信号 
	return buf; 
}