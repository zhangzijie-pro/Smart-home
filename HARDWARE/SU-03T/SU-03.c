#include "SU-03.h"


uint8_t SU_Serial_Buffer[3];
uint8_t SU_receive_ok_flag=0;
uint8_t SU_counter=0;

void SU_init(void)
{
	  USART_InitTypeDef USART_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;
		
    // 使能时钟
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	
    // 配置 GPIO 引脚
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOA,&GPIO_InitStructure);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);
		
		//USART_DeInit(USART2);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    // 配置 USART 参数
    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(USART2, &USART_InitStructure);

		USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);
		
		NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; 
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
		
		    // 使能 USART
    USART_Cmd(USART2, ENABLE);
}

void USART2_IRQHandler(void){
		static uint8_t RxState = 0;
		static uint8_t pRxPacket = 0;
		if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET){
			uint8_t SU_data = USART_ReceiveData(USART2);
		
			if(RxState==0){
				if(SU_data==0xAA){
					RxState=1;
				}
			}else if(RxState==1){
				if(SU_data == 0x55){
					RxState=2;
				}else{
					RxState=0;
				}
			}else if(RxState==2){
				SU_Serial_Buffer[pRxPacket] = SU_data;
				pRxPacket++;
				if(pRxPacket>=3){
					RxState=3;
					pRxPacket=0;
				}
			}else if(RxState==3){
				if(SU_data == 0x55){
					RxState=4;
				}else{
					RxState=0;
				}
			}else if(RxState==4){
				if(SU_data == 0xAA){
					RxState=0;
					SU_receive_ok_flag=1;
				}else{
					RxState=0;
				}
			}
			
			
			USART_ClearITPendingBit(USART2,USART_IT_RXNE);
		}
}

void SU_sendbyte(uint8_t Byte){
	while(!(USART_GetFlagStatus(USART2,USART_FLAG_TC) == 1));
	USART_SendData(USART2,Byte);  
}

void SU_sendArray(uint8_t *Array, uint16_t len)
{
	char *p = (char *)Array;
	
	while(len --){
		SU_sendbyte(*p);
		p ++;
	}
}

void SU_sendCmd(char *String){
	SU_sendbyte(0xAA);
	SU_sendbyte(0x55);
	SU_sendString(String);
	SU_sendbyte(0x55);
	SU_sendbyte(0xAA);
	
}


void SU_sendString(char *String)
{
	while(*String!='\0')
	{
		while(!(USART_GetFlagStatus(USART2,USART_FLAG_TC) == 1));
		USART_SendData(USART2,*String++);
	}
}

void SU_prinf(char *format,...){
	char String[100];
	va_list arg;
	va_start(arg, format);
	vsprintf(String, format, arg);
	va_end(arg);
	SU_sendString(String);
}