#include "esp8266.h"

uint8_t esp8266_Serial_Buffer[MAX_BUFFER_LEN];
uint8_t esp8266_receive_ok_flag=0;
uint8_t esp8266_counter=0;
// 1.天气查询
// 连接与断开连接
// 是否正常

void esp8266_init(void)
{
		// 2. ????PB10 (TX) ?? PB11 (RX) ????ù???
		USART_InitTypeDef USART_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;
	
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
		// ???? GPIO ????
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;  // PB10 -> TX
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;  //PB11 -> RX
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);
	
	
		//USART_DeInit(USART3);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
		// ???? USART3 ????
    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(USART3, &USART_InitStructure);

		// ??? USART
		//USART_ClearFlag(USART2, USART_FLAG_TC);
		//USART_ITConfig(USART3,USART_IT_PE,ENABLE);
		USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);
		//USART_ITConfig(USART3,USART_IT_IDLE,ENABLE);
		
		NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; 
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);

		USART_Cmd(USART3, ENABLE);
}

void USART3_IRQHandler(void){
		static uint8_t RxState = 0;
		static uint8_t pRxPacket = 0;
		if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET){
			uint8_t esp8266_data = USART_ReceiveData(USART3);
		
			if(RxState==0){
				if(esp8266_data==0x55){
					RxState=1;
				}
			}else if(RxState==1){
				esp8266_Serial_Buffer[pRxPacket] = esp8266_data;
				pRxPacket++;
				if(pRxPacket>=3){
					RxState=2;
					pRxPacket=0;
				}
			}else if(RxState==2){
				if(esp8266_data == 0xFF){
					RxState=0;
					esp8266_receive_ok_flag=1;
				}else{
					RxState=0;
				}
			}
			
			
			USART_ClearITPendingBit(USART3,USART_IT_RXNE);
		}
}

void esp8266_sendbyte(uint8_t Byte){
	while(!(USART_GetFlagStatus(USART3,USART_FLAG_TC) == 1));
	USART_SendData(USART3,Byte);
}

void esp8266_sendArray(uint8_t *Array, uint16_t len)
{
		char *p = (char *)Array;
		
		while(len --){
			esp8266_sendbyte(*p);
			p ++;
		}
}

void esp8266_sendString(char *String)
{
	 while(*String!='\0')
	{
		while(!(USART_GetFlagStatus(USART3,USART_FLAG_TC) == 1));
		USART_SendData(USART3,*String++);
	}
}

void esp8266_prinf(char *format,...){
	char String[100];
	va_list arg;
	va_start(arg, format);
	vsprintf(String, format, arg);
	va_end(arg);
	esp8266_sendString(String);
}

void esp_sendcmd(uint8_t Byte){
	esp8266_sendbyte(0x55);
	esp8266_sendbyte(Byte);
	esp8266_sendbyte(0xff);
}

void esp_uploaddata(uint8_t *Array, uint16_t len){
	esp8266_sendbyte(0x55);
	esp8266_sendArray(Array,len);
	esp8266_sendbyte(0xff);
}