#include "main.h"
/***************************************************************
     
													函数定义

****************************************************************/
// 传感器数据
void upload_wd_sd(void);
void rain_check(void);
void human_check(void);

// ADC
void Get_ad_data(void);

// RTC时钟
RTC_TimeTypeDef RTC_TimeStruct;
RTC_DateTypeDef RTC_DateStruct;
void upload_time(void);

// Servo
void open_window();
void close_window();
void role_up(void);
void role_down(void);
void door_open(void);
void door_close(void);

// voice control
void Do_Key_SU(uint8_t *data);

// LCD control
void send_control_hmi(char *String);
void Do_Key_LCD(uint8_t *data);

// network
void check_network(void);
void hmi_do_network(void);

// ACTION
void change_temp(uint8_t action);
void change_wet(uint8_t action);
void temp_up(uint8_t action);
void temp_down(uint8_t action);
void wet_up(uint8_t action);
void wet_down(uint8_t action);
void window_open_close(uint8_t action);
void role_up_down(uint8_t action);
void door_open_close(uint8_t action);
void homelight(uint8_t action);
void holllight(uint8_t action);
void roomlight(uint8_t action);
void wclight(uint8_t action);
void cflight(uint8_t action);
void alllight(uint8_t action);
void wc_fan(uint8_t action);
/***************************************************************
     
													变量定义

****************************************************************/
// 温湿度
uint8_t temp,wet;
uint8_t temp_max=20;
uint8_t wet_max=40;

uint8_t cmd1[100];
uint8_t cmd2[100];
// 光照
u8 light_max = 0;
uint16_t Light=0;

// 雨滴 人体发送内容
uint8_t human_rain[100];

// network
uint8_t network_connect = 0;
uint8_t cmdnetwork[100];

// AD采集
uint16_t adc_MQ;
uint16_t adc_CO;

uint8_t MQ_max=2;
uint8_t CO_max=1;
uint8_t ad_buf[40];
// RTC
uint16_t t=0;
uint8_t date_buf[40];
uint8_t time_buf[40];
/***************************************************************
     
													主函数

****************************************************************/
int main(void)
{
	// 光照强度
	//BH1750_Init();		 
	// bug 不能使用串口2 3 4
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	delay_init(168);
	// ADC
	Adc_Init();
	// esp8266
	esp8266_init();
	// RTC
	My_RTC_Init();
	// 输出引脚
	gpio_out_init();
	// 语音
	SU_init();
	// DHT11
	dht11_init();
	// LCD
	HMI_init();
	// 舵机初始
	Servo_Init();
	// 雨滴 人体 火焰
	Rain_Init();
	Human_Init();
	fire_Init();

//	RTC_Set_Time(22,04,50,RTC_H12_PM);
//	RTC_Set_Date(24,10,13,7);
	RTC_Set_WakeUp(RTC_WakeUpClock_CK_SPRE_16bits,0);
	while(1)
	{
		/*upload_time();
		
		if(esp8266_receive_ok_flag){
			esp8266_receive_ok_flag=0;
			for(int i=0;i<2;i++){
				esp8266_prinf("%d\r\n",esp8266_Serial_Buffer[i]);
			}
		}
		
// 		HMI 接收
		if(HMI_receive_ok_flag){
			HMI_receive_ok_flag=0;
			Do_Key_LCD(HMI_Serial_Buffer);
		}
		
		
//		 SU 接收
		if(SU_receive_ok_flag){
			SU_receive_ok_flag=0;
			SU_prinf("receive\r\n");
			for(int i=0;i<2;i++){
				SU_prinf("%d\r\n",SU_Serial_Buffer[i]);
			}
		}*/
		
	}
}
/***************************************************************
     
													函数

****************************************************************/
// 光照
void light_check(void){
		Light=bh_data_read();		// 读取光照强度
}

// 检测人体
void human_check(void){
	if(HUMAN_EXIST){		// 人体存在
		sprintf((char*)human_rain,"sleep=0");
		send_control_hmi((char*)ad_buf);
	}
}

// 雨滴检测
void rain_check(void){
	if(RAIN_EXIST==0){		// 有雨
		sprintf((char*)human_rain,"page0.bt5.val=0");
		send_control_hmi((char*)ad_buf);
		sprintf((char*)human_rain,"page0.bt5.txt=\"晾衣杆伸\"");
		send_control_hmi((char*)ad_buf);
		ROLE_DOWN=1;		// 语音播报收缩
		role_down();		// 收缩
	}
}

// AD 数据
void Get_ad_data(void){
		adc_MQ=Get_Adc_Average(ADC_Channel_5,20);
		float Voltage = adc_MQ * 3.3 / 4096;
		//float ppm = (Voltage - 0.5) / 0.1 * 200;
		adc_CO=Get_Adc_Average(ADC_Channel_6,20);
		float Voltage2 = adc_CO * 3.3 / 4096;
	
		if((Voltage<MQ_max&&Voltage2>CO_max)||!FIRE_EXIST){
				sprintf((char*)ad_buf,"page0.t10.txt=\"异常\"");
				send_control_hmi((char*)ad_buf);
				esp_sendcmd("3");
		}else{
				sprintf((char*)ad_buf,"page0.t10.txt=\"正常\"");
				send_control_hmi((char*)ad_buf);	
				esp_sendcmd("4");			
		}
}

// 返回RTC
void upload_time(void){
	t++;
	if(t%60000==0){
		RTC_GetTime(RTC_Format_BIN,&RTC_TimeStruct);
		
		sprintf((char*)time_buf,"page0.t8.txt=\"%02d:%02d\"",RTC_TimeStruct.RTC_Hours,RTC_TimeStruct.RTC_Minutes); 
		esp8266_prinf((char*)time_buf);
		
		RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);
		
		sprintf((char*)date_buf,"page0.t7.txt=\"20%02d-%02d-%02d 星期 %d\"",RTC_DateStruct.RTC_Year,RTC_DateStruct.RTC_Month,RTC_DateStruct.RTC_Date,RTC_DateStruct.RTC_WeekDay); 
		esp8266_prinf((char*)date_buf);
	}
}

// 检测上传温湿度
void upload_wd_sd(void){
	uint8_t upload_wd_cmd[100];
	uint8_t upload_sd_cmd[100];
	uint8_t confg_wet[100];
	
	int32_t rt=0;
	uint8_t buf[5]={0};
	rt=dht11_read(buf);
	
	uint8_t esp_buf[4];
	if(rt==0)
		{
			temp=buf[2];
			wet=buf[0];
			sprintf((char *)upload_wd_cmd,"page0.t5.txt=\"%d.%d°C\"",buf[2],buf[3]);
			sprintf((char *)upload_sd_cmd,"page0.t6.txt=\"%d.%d%%\"",buf[0],buf[1]);
//			HMI_prinf("T:%d.%d,H:%d.%d\r\n",buf[2],buf[3],buf[0],buf[1]);
			delay_ms(5);
			send_control_hmi((char*)upload_wd_cmd);
			delay_ms(5);
			send_control_hmi((char*)upload_sd_cmd);
			
			// 上报esp
			sprintf((char*)esp_buf,"%d %d",temp,wet);
			esp_sendcmd((char*)esp_buf);
			
			if(wet<45){
					sprintf((char*)confg_wet,"page0.bt3.val=1");
					send_control_hmi((char*)confg_wet);
					delay_ms(5);
					sprintf((char*)confg_wet,"page0.bt3.txt=\"加湿开\"");
					send_control_hmi((char*)confg_wet);
					WET_OUT=1;
					FAN_OUT=0;
			}else if(wet>55){
					sprintf((char*)confg_wet,"page0.bt4.val=1");
					send_control_hmi((char*)confg_wet);
					delay_ms(5);
					sprintf((char*)confg_wet,"page0.bt4.txt=\"除湿开\"");
					send_control_hmi((char*)confg_wet);
					FAN_OUT=1;
					WET_OUT=0;
			}
		}
}

// 开窗正转
void open_window(void){
	Servo_Angle_up(180);
}

// 开窗反转
void close_window(void){
	Servo_Angle_up(0);
}

// 伸晾衣杆
void role_up(void){
	Servo_Angle_down(180);
}

// 缩晾衣杆
void role_down(void){
	Servo_Angle_down(0);
}

void door_open(void){
	Servo_Angle_door(180);
}

void door_close(void){
	Servo_Angle_door(0);
}

// LCD
void Do_Key_LCD(uint8_t *data){
    uint8_t mode = data[0];
    uint8_t action = data[1];

    switch (mode)
    {
    // page0 button to control temp and wet
    case 1:
        change_temp(action);
        break;
    case 2:
        change_wet(action);
        break;
    case 3:
        temp_up(action);
        break;
    case 4:
        temp_down(action);
        break;
    case 5:
        window_open_close(action);
        break;
		case 6:
				wet_up(action);
				break;
		case 7:
				wet_down(action);
				break;
		case 8:
				role_up_down(action);
				break;
		case 9:
				hmi_do_network();
				break;
		
    // page1 button to control light
    case 16:
				homelight(action);
        break;
    case 32:
				holllight(action);
        break;
    case 48:
				roomlight(action);
        break;
    case 64:
				wclight(action);
        break;
    case 80:
				cflight(action);
        break;
    case 96:
				alllight(action);
        break;
		case 112:		// 厕所风扇
				wc_fan(action);
        break;
		case 128:
				door_open_close(action);
				break;

    default:
        break;
    }
}

// 语音控制
void Do_Key_SU(uint8_t *data){
    uint8_t mode = data[0];
    uint8_t action = data[1];

    switch (mode)
    {
    // page0 button to control temp and wet
    case 1:
        temp_up(action);
        break;
    case 2:
        temp_down(action);
        break;
    case 3:
        wet_up(action);
        break;
    case 4:
        wet_down(action);
        break;
    case 5:
        window_open_close(action);
        break;
		// page1 button to control light
		case 6:
				homelight(action);
				break;
		case 7:
				holllight(action);
				break;
    case 8:
				roomlight(action);
        break;
    case 9:
				wclight(action);
        break;
    case 16:
				cflight(action);
        break;
    case 17:
				alllight(action);
        break;
		case 18:	// 厕所风扇
				wc_fan(action);
        break;
		case 19:
				role_up_down(action);
				break;
		case 20:
				door_open_close(action);
				break;
		// weather
    case 48:	// 开始查询天气
				
        break;
		
		// esp content:
		case 32|64:
			check_network();
			break;

    default:
        break;
    }
}

// ----------------------网络连接----------------------
void check_network(void){
	network_connect=1;
	sprintf((char*)cmdnetwork,"page0.t11.txt=\"连接成功\"");
	send_control_hmi((char*)cmdnetwork);
	sprintf((char*)cmdnetwork,"page0.b4.bco=1024");
	send_control_hmi((char*)cmdnetwork);
	sprintf((char*)cmdnetwork,"page0.b4.txt=\"断开连接\"");
	send_control_hmi((char*)cmdnetwork);
}

void hmi_do_network(void){
	if(network_connect){		// 连接成功-> 发送断开连接
		esp_sendcmd("1");
		network_connect=0;
		sprintf((char*)cmdnetwork,"page0.t11.txt=\"连接失败\"");
		send_control_hmi((char*)cmdnetwork);
		sprintf((char*)cmdnetwork,"page0.b4.bco=50712");
		send_control_hmi((char*)cmdnetwork);
		sprintf((char*)cmdnetwork,"page0.b4.txt=\"重新连接\"");
		send_control_hmi((char*)cmdnetwork);
	}else{		// 连接失败-> 发送重新连接
		esp_sendcmd("2");
	}
}

// ----------------------屏幕按键/语音事件----------------------
// 发送HMI数据
void send_control_hmi(char *String){
	HMI_sendString(String);
	HMI_sendbyte(0xff);
	HMI_sendbyte(0xff);
	HMI_sendbyte(0xff);
}

void change_temp(uint8_t action){
    if(action==0x01){
        temp_max+=1;
    }else if(action==0x00){
        temp_max-=1;
    }
		sprintf((char *)cmd1,"page0.t3.txt=\"温度阈值:%d°\"",temp_max);
		send_control_hmi((char*)cmd1);
}

void change_wet(uint8_t action){
    if(action==0x01){
        wet_max+=1;
    }else if(action==0x00){
        wet_max-=1;
    }
		sprintf((char *)cmd2,"page0.t4.txt=\"湿度阈值:%d%%\"",wet_max);
		send_control_hmi((char*)cmd2);
}

void temp_up(uint8_t action){
    if(action==0x01){
				TEMP_UP=1;
    }else if(action==0x00){
				TEMP_UP=0;
    }
}

void temp_down(uint8_t action){
    if(action==0x01){
				TEMP_Down=1;
    }else if(action==0x00){
				TEMP_Down=0;
    }
}

void wet_up(uint8_t action){
    if(action==0x01){
        WET_OUT=1;
    }else if(action==0x00){
				WET_OUT=0;
    }
}
void wet_down(uint8_t action){
    if(action==0x01){
        FAN_OUT=1;
    }else if(action==0x00){
				FAN_OUT=0;
    }
}

void window_open_close(uint8_t action){
    if(action==0x01){
        open_window();
    }else if(action==0x00){
				close_window();
    }
}

void role_up_down(uint8_t action){
		if(action==0x01){
        role_up();
    }else if(action==0x00){
				role_down();
    }
}

void door_open_close(uint8_t action){
		if(action==0x01){
        door_open();
    }else if(action==0x00){
				door_close();
    }
}

void homelight(uint8_t action){
	    if(action==0x01){
        LED1=1;
    }else if(action==0x00){
				LED1=0;
    }
}

void holllight(uint8_t action){
	  if(action==0x01){
        LED2=1;
    }else if(action==0x00){
				LED2=0;
    }
}

void roomlight(uint8_t action){
	  if(action==0x01){
        LED3=1;
    }else if(action==0x00){
				LED3=0;
    }
}
void wclight(uint8_t action){
	   if(action==0x01){
        LED4=1;
    }else if(action==0x00){
				LED4=0;
    }
}

void cflight(uint8_t action){
	    if(action==0x01){
        LED5=1;
    }else if(action==0x00){
				LED5=0;
    }
}
void alllight(uint8_t action){
	    if(action==0x01){
        LED1=1;
				LED2=1;
				LED3=1;
				LED4=1;
				LED5=1;
    }else if(action==0x00){
				LED1=0;
				LED2=0;
				LED3=0;
				LED4=0;
				LED5=0;
    }
}

void wc_fan(uint8_t action){
		if(action==0x01){
        WC_FAN=1;
    }else if(action==0x00){
				WC_FAN=0;
    }
}