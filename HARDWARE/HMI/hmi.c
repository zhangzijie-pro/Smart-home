#include "hmi.h"


uint8_t HMI_receive_ok_flag=0;
uint8_t HMI_counter;
uint8_t HMI_Serial_Buffer[2];

void HMI_init(void)
{
		USART_InitTypeDef USART_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;
	
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
		// ���� GPIO ����
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;  // PA0 -> TX
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;  //PA1 -> RX
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_UART4);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_UART4);
	
	
		USART_DeInit(UART4);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
		// ���� USART4 ����
    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(UART4, &USART_InitStructure);


		USART_ITConfig(UART4,USART_IT_RXNE,ENABLE);
		
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
		
		NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; 
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
		
				// ʹ�� USART
    USART_Cmd(UART4, ENABLE);
}


void UART4_IRQHandler(void){
		static uint8_t RxState = 0;
		static uint8_t pRxPacket = 0;
		if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET){
			uint8_t HMI_data = USART_ReceiveData(UART4);
		
			if(RxState==0){
				if(HMI_data==0x55){
					RxState=1;
				}
			}else if(RxState==1){
				HMI_Serial_Buffer[pRxPacket] = HMI_data;
				pRxPacket++;
				if(pRxPacket>=2){
					RxState=2;
					pRxPacket=0;
				}
			}else if(RxState==2){
				if(HMI_data == 0xff){
					RxState=3;
				}else{
					RxState=0;
				}
			}else if(RxState==3){
				if(HMI_data == 0xff){
					RxState=4;
				}else{
					RxState=0;
				}
			}else if(RxState==4){
				if(HMI_data == 0xff){
					RxState=0;
					HMI_receive_ok_flag=1;
				}else{
					RxState=0;
				}
			}
			
			
			USART_ClearITPendingBit(UART4,USART_IT_RXNE);
		}
}

void HMI_sendbyte(uint8_t Byte){
	while(!(USART_GetFlagStatus(UART4,USART_FLAG_TC) == 1));
	USART_SendData(UART4,Byte);
}


void HMI_sendArray(const uint8_t *Array, uint16_t len)
{
		char *p = (char *)Array;
		
		while(len --){
			HMI_sendbyte(*p);
			p ++;
		}
}

void HMI_sendString(char *String)
{
	while(*String!='\0')
	{
		while(!(USART_GetFlagStatus(UART4,USART_FLAG_TC) == 1));
		USART_SendData(UART4,*String++);
	}
}

void HMI_Sendb(uint8_t byte){
	for(int i=0;i<3;i++){
		HMI_sendbyte(byte);
	}
}

void HMI_prinf(char *format,...){
	char String[100];
	va_list arg;
	va_start(arg, format);
	vsprintf(String, format, arg);
	va_end(arg);
	HMI_sendString(String);
}