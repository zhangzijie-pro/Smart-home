#include "gpio_in.h"
 
// 检测人体
void Human_Init(void)
{
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);	 	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;				 
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; 	//初始化设置为输入状态 
 GPIO_Init(GPIOB, &GPIO_InitStructure);				
	
 GPIO_ResetBits(GPIOB,GPIO_Pin_9);
}

// 雨滴检测
void Rain_Init(void)
{
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);	 	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;				 
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; 	//初始化设置为输入状态 
 GPIO_Init(GPIOB, &GPIO_InitStructure);				
	
 //GPIO_ResetBits(GPIOB,GPIO_Pin_6);
}


// 火焰检测
void fire_Init(void)
{
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);	 	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;				 
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; 	//初始化设置为输入状态 
 GPIO_Init(GPIOB, &GPIO_InitStructure);				
	
 GPIO_ResetBits(GPIOB,GPIO_Pin_7);
}