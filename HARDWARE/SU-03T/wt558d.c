#include "wt558d.h"

void Wt588d_Init(){
	RCC_AHB1PeriphClockCmd(WT588D_RCC,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructre;
	GPIO_InitStructre.GPIO_Pin=WT588D_CS_Pin|WT588D_SCL_Pin|WT588D_SDA_Pin;
	GPIO_InitStructre.GPIO_Mode=GPIO_Mode_OUT;
	GPIO_InitStructre.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructre.GPIO_OType=GPIO_OType_PP;
	
	GPIO_Init(WT588D_PORT,&GPIO_InitStructre);
}

void Send_threelines(unsigned char addr){
	uint8_t i;
	delay_ms(17);
	CS_BEGNI;
	delay_ms(2);
	for(i=0;i<8;i++){
		SCL_BEGNI;
		if(addr&1) SDA_END;
		else SDA_BEGNI;
		addr>>=1;
		delay_us(100);
		SCL_END;
		delay_us(100);
	}
	CS_END;
}