#include "dht11.h"

GPIO_InitTypeDef  GPIO_InitStructure;
void dht11_init(void)
{
   //使能PG9时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
	
	//配置PG9为输出模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//复用功能模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//上拉
	GPIO_Init(GPIOG, &GPIO_InitStructure);//初始化
	//配置PE6为输入模式

	
	PGout(9)=1;
}



int32_t dht11_read(uint8_t *pbuf)
{
	uint32_t t=0;
	int32_t i=0,j=0;
	uint8_t d=0;
	uint8_t *p=pbuf;
	uint32_t check_sum=0;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//复用功能模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//上拉
	GPIO_Init(GPIOG, &GPIO_InitStructure);//初始化
	
	PGout(9)=0;
	delay_ms(20);
	PGout(9)=1;
	delay_us(30);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//复用功能模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//上拉
	GPIO_Init(GPIOG, &GPIO_InitStructure);//初始化
	
	//等待低电平出现
	t=0;
	while(PGin(9))
	{
		//超时处理
		t++;
		delay_us(1);
		if(t>=4000)
		{
			return -1;
		}
	}
	
	//检验低电平的合法性
	t=0;
	while(PGin(9)==0)
	{
		t++;
		delay_us(1);
		if(t>=4000)
		{
			return -2;
		}
	}
	
	t=0;
	while(PGin(9))
	{
		t++;
		delay_us(1);
		if(t>=4000)
		{
			return -3;
		}
	}
	for(j=0;j<5;j++)
	{
		d=0;
		for(i=7;i>=0;i--)
		{
			t=0;
			while(PGin(9)==0)
			{
				t++;
				delay_us(1);
				if(t>=4000)
				{
					return -4;
				}
			
			}
			delay_us(40);
			//判断当前引脚电平
			if(PGin(9))
			{
				d|=1<<i;
				
				//等待高电平完毕
				t=0;
				while(PGin(9))
				{
					t++;
					delay_us(1);
					if(t>=1000)
					{
						return -5;
					}
				}
			}
				
		}
		p[j]=d;
	
	}
	//校验和
	check_sum = (p[0]+p[1]+p[2]+p[3])&0xFF;
	
	if(p[4]!=check_sum)
	{
		return -6;
		
	}
	return 0;
	
}