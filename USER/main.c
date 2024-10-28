#include "main.h"
/***************************************************************
     
													函数定义

****************************************************************/
// 传感器数据
void upload_wd_sd(void);
void rain_check(void);
void human_check(void);
void light_check(void);


// ADC
void Get_ad_data(void);

// RTC时钟
void deal_time(uint8_t hours,uint8_t minute);
void upload_time(void);

// Servo
void open_window(void);
void close_window(void);
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
void change_max_temp(uint8_t action);
void change_max_wet(uint8_t action);
void change_min_temp(uint8_t action);
void change_min_wet(uint8_t action);

void temp_up(uint8_t action);
void temp_down(uint8_t action);
uint8_t temp_button_buffer[50];
void wet_up(uint8_t action);
void wet_down(uint8_t action);
uint8_t wet_button_buffer[50];
void window_open_close(uint8_t action);
uint8_t window_buffer[50];
void role_up_down(uint8_t action);
uint8_t role_buffer[50];
void door_open_close(uint8_t action);
uint8_t door_buffer[50];
void homelight(uint8_t action);
void holllight(uint8_t action);
void roomlight(uint8_t action);
void wclight(uint8_t action);
void cflight(uint8_t action);
void alllight(uint8_t action);

void wc_fan(uint8_t action);
uint8_t wc_fan_buffer[50];

void road_light(uint8_t action);
void atmosphere_light_lcd(uint8_t action);		// LCD氛围灯
void atmosphere_light_su(uint8_t action);			// 语音氛围灯
void music_atmosphere_light(void); 		// 音乐模式
void change_avager_temp(uint8_t action);
void change_avager_wet(uint8_t action);

// Dec -> Hex
uint8_t dec_to_hex(uint8_t num);
/***************************************************************
     
													变量定义

****************************************************************/
#define CAL_PPM  10  // 校准环境中PPM值
#define RL	     10  // RL阻值
#define R0	     16  // R0阻值
RTC_TimeTypeDef RTC_TimeStruct;
RTC_DateTypeDef RTC_DateStruct;

uint8_t hours=0;
uint8_t minute=0;
uint8_t time_do=0;		// while计时
// 温湿度
uint8_t temp,wet;

uint8_t temp_max=30;
uint8_t temp_min=20;

uint8_t wet_max=55;
uint8_t wet_min=45;

uint8_t temp_avager=25;
uint8_t humi_avager=50;

uint8_t cmd1[100];
uint8_t cmd2[100];
uint8_t cmd3[100];
uint8_t cmd4[100];

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
	10-26 : 3:38 -> RTC与语音播报时间 | 删除处理网络 | 手机平均阈值
****************************************************************/
int main(void)
{
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

	Servo_Angle_door(90);
	Servo_Angle_up(90);
	Servo_Angle_down(90);
	RTC_Set_WakeUp(RTC_WakeUpClock_CK_SPRE_16bits,0);
	while(1)
	{
		// 一秒事件
		time_do++;
		if((time_do%100)==0){
			rain_check();
			delay_ms(1);
			human_check();
			delay_ms(1);
			Get_ad_data();
			delay_ms(1);
			upload_wd_sd();
			delay_ms(1);
			time_do=0;
		}

		if(esp8266_receive_ok_flag){
			esp8266_receive_ok_flag=0;
			Do_Key_SU(esp8266_Serial_Buffer);
		} 
		delay_ms(1);
		
//	HMI 接收
		if(HMI_receive_ok_flag){
			HMI_receive_ok_flag=0;
			Do_Key_LCD(HMI_Serial_Buffer);
		}
		delay_ms(1);
		
//	 SU 接收
		if(SU_receive_ok_flag){
			SU_receive_ok_flag=0;
			Do_Key_SU(SU_Serial_Buffer);
		}

		upload_time();
	}
}

/***************************************************************
     
													函数

****************************************************************/

// 检测人体
void human_check(void){
	if(HUMAN_EXIST==0){		// 人体存在
		sprintf((char*)human_rain,"sleep=0");
		send_control_hmi((char*)human_rain);
	}
}

// 雨滴检测
void rain_check(void){
	if(RAIN_EXIST==0){		// 有雨
		sprintf((char*)human_rain,"page0.bt5.val=1");
		send_control_hmi((char*)ad_buf);
		sprintf((char*)human_rain,"page0.bt5.txt=\"晾衣杆缩\"");
		send_control_hmi((char*)ad_buf);
		esp_sendcmd(0x06);
		ROLE_DOWN=1;		// 语音播报收缩
		role_down();		// 收缩
	}else if(RAIN_EXIST){
		ROLE_DOWN=0;
	}
}

// AD 数据
void Get_ad_data(void){
		adc_MQ=Get_Adc_Average(ADC_Channel_5,20);
		float Voltage = adc_MQ * 3.3 / 4096.0;
		float ppm1 = (Voltage - 0.5) / 0.1 * 200;
		adc_CO=Get_Adc_Average(ADC_Channel_4,20);
		float Voltage2 = adc_CO * 3.3 / 4096;
		float RS = (3.3f - Voltage2) / Voltage2 * RL;
		float ppm2 = 98.322f * pow(RS/R0, -1.458f);
	
		if(FIRE_EXIST==0){
				FIRE_SIGNAL=1;
				sprintf((char*)ad_buf,"page0.t10.txt=\"异常\"");
				send_control_hmi((char*)ad_buf);
				sprintf((char*)ad_buf,"page0.t10.bco=63488");
				send_control_hmi((char*)ad_buf);
				esp_sendcmd(0x03);
		}else{
				FIRE_SIGNAL=0;
				sprintf((char*)ad_buf,"page0.t10.txt=\"正常\"");
				send_control_hmi((char*)ad_buf);	
				sprintf((char*)ad_buf,"page0.t10.bco=2016");
				send_control_hmi((char*)ad_buf);
				esp_sendcmd(0x04);			
		}
}

// 返回RTC
void upload_time(void){
		t++;
		uint8_t cmd_time[3]={0x01,0x00,0x00};
		if((t%100)==0){
			RTC_GetTime(RTC_Format_BIN,&RTC_TimeStruct);
			uint8_t hours_turn = RTC_TimeStruct.RTC_Hours;
			uint8_t minute_turn=RTC_TimeStruct.RTC_Minutes;
			sprintf((char*)time_buf,"page0.t8.txt=\"%02d:%02d\"",RTC_TimeStruct.RTC_Hours,RTC_TimeStruct.RTC_Minutes); 
			send_control_hmi((char*)time_buf);
			cmd_time[1]=dec_to_hex(hours_turn);
			cmd_time[2]=dec_to_hex(minute_turn);
			SU_sendCmd((char*)cmd_time);
		}
		delay_ms(10);
}

// 检测上传温湿度
void upload_wd_sd(void){
	uint8_t upload_wd_cmd[100];
	uint8_t upload_sd_cmd[100];
	uint8_t confg_wet[100];
	uint8_t confg_temp[100];
	int32_t rt=0;
	uint8_t buf[5]={0};
	rt=dht11_read(buf);
	
	uint8_t esp_buf[3];
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
			esp_buf[0]=0x05;
			esp_buf[1]=temp;
			esp_buf[2]=wet;
			esp_uploaddata(esp_buf,3);
			
			if((wet<wet_min)||(wet<(humi_avager-5))){
					sprintf((char*)confg_wet,"page0.bt3.val=1");
					send_control_hmi((char*)confg_wet);
					delay_ms(5);
					sprintf((char*)confg_wet,"page0.bt3.txt=\"加湿开\"");
					send_control_hmi((char*)confg_wet);
					WET_OUT=0;
					FAN_OUT=1;
			}else if((wet>wet_max)||(wet>(humi_avager+5))){
					sprintf((char*)confg_wet,"page0.bt4.val=1");
					send_control_hmi((char*)confg_wet);
					delay_ms(5);
					sprintf((char*)confg_wet,"page0.bt4.txt=\"除湿开\"");
					send_control_hmi((char*)confg_wet);
					FAN_OUT=0;
					WET_OUT=1;
			}
			
			if((temp<temp_min)||(temp<(temp_avager-5))){
					sprintf((char*)confg_temp,"page0.bt1.val=1");
					send_control_hmi((char*)confg_temp);
					delay_ms(5);
					sprintf((char*)confg_temp,"page0.bt1.txt=\"升温开\"");
					send_control_hmi((char*)confg_temp);
					TEMP_UP=0;	
					TEMP_Down=1;
			}else if((temp>temp_max)||(temp>(temp_avager+5))){
					sprintf((char*)confg_temp,"page0.bt2.val=1");
					send_control_hmi((char*)confg_temp);
					delay_ms(5);
					sprintf((char*)confg_temp,"page0.bt2.txt=\"降温开\"");
					send_control_hmi((char*)confg_temp);
					TEMP_UP=1;
					TEMP_Down=0;
			}
		}
}

// 开窗正转
void open_window(void){
	Servo_Angle_up(0);
}

// 开窗反转
void close_window(void){
	Servo_Angle_up(90);
}

// 伸晾衣杆
void role_up(void){
	Servo_Angle_down(0);
}

// 缩晾衣杆
void role_down(void){
	Servo_Angle_down(180);
}

void door_open(void){
	Servo_Angle_door(0);
}

void door_close(void){
	Servo_Angle_door(90);
}

// ----------------------LCD ACTION----------------------
void Do_Key_LCD(uint8_t *data){
    uint8_t mode = data[0];
    uint8_t action = data[1];

    switch (mode)
    {
    // page0 button to control temp and wet
    case 1:
        change_max_temp(action);
        break;
    case 2:
        change_max_wet(action);
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
//		case 9:
//				hmi_do_network();
//				break;
		case 10:
				change_min_temp(action);
				break;
		case 11:
				change_min_wet(action);
				break;
		case 12:
				road_light(action);
				break;
		case 13:
				atmosphere_light_lcd(action);
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

// ----------------------语音控制/手机----------------------
void Do_Key_SU(uint8_t *data){
    uint8_t mode = data[0];
    uint8_t action = data[1];
		uint8_t other = data[2];
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
		case 21:
				road_light(action);
				break;
		case 22:
				deal_time(action,other);
				break;
		case 23:
				atmosphere_light_su(action);
				break;
		case 25:
				atmosphere_light_lcd(action);
				break;
		
		// 平均温度阈值
		case 26:
				change_avager_temp(action);
				break;
		case 27:
				change_avager_wet(action);
				break;
		// 平均湿度阈值
		
		// esp content:
//		case 64:
//			check_network();
//			break;

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
		esp_sendcmd(0x01);
		network_connect=0;
		sprintf((char*)cmdnetwork,"page0.t11.txt=\"连接失败\"");
		send_control_hmi((char*)cmdnetwork);
		sprintf((char*)cmdnetwork,"page0.b4.bco=50712");
		send_control_hmi((char*)cmdnetwork);
		sprintf((char*)cmdnetwork,"page0.b4.txt=\"重新连接\"");
		send_control_hmi((char*)cmdnetwork);
	}else{		// 连接失败-> 发送重新连接
		esp_sendcmd(0x02);
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

void change_max_temp(uint8_t action){
    if(action==0x01){
        temp_max+=1;
    }else if(action==0x00){
        temp_max-=1;
    }
		sprintf((char *)cmd1,"page2.t3.txt=\"最高温度阈值:%d°\"",temp_max);
		send_control_hmi((char*)cmd1);
}

void change_min_temp(uint8_t action){
    if(action==0x01){
        temp_min+=1;
    }else if(action==0x00){
        temp_min-=1;
    }
		sprintf((char *)cmd3,"page3.t3.txt=\"最低温度阈值:%d°\"",temp_min);
		send_control_hmi((char*)cmd3);
}

void change_avager_temp(uint8_t action){
		if(action==0x01){
        temp_avager+=1;
    }else if(action==0x00){
        temp_avager-=1;
    }
}

void change_max_wet(uint8_t action){
    if(action==0x01){
        wet_max+=1;
    }else if(action==0x00){
        wet_max-=1;
    }
		sprintf((char *)cmd2,"page4.t3.txt=\"最高湿度阈值:%d%%\"",wet_max);
		send_control_hmi((char*)cmd2);
}

void change_min_wet(uint8_t action){
		if(action==0x01){
        wet_min+=1;
    }else if(action==0x00){
        wet_min-=1;
    }
		sprintf((char *)cmd4,"page5.t3.txt=\"最低湿度阈值:%d%%\"",wet_min);
		send_control_hmi((char*)cmd4);
}

void change_avager_wet(uint8_t action){
		if(action==0x01){
        humi_avager+=1;
    }else if(action==0x00){
        humi_avager-=1;
    }
}

void temp_up(uint8_t action){
    if(action==0x01){
				TEMP_UP=0;
				sprintf((char *)temp_button_buffer,"page0.bt1.val=1");
				send_control_hmi((char*)temp_button_buffer);
				delay_ms(5);
				sprintf((char *)temp_button_buffer,"page0.bt1.txt=\"升温开\"");
				send_control_hmi((char*)temp_button_buffer);				
    }else if(action==0x00){
				TEMP_UP=1;
				sprintf((char *)temp_button_buffer,"page0.bt1.val=0");
				send_control_hmi((char*)temp_button_buffer);
				delay_ms(5);
				sprintf((char *)temp_button_buffer,"page0.bt1.txt=\"升温关\"");
				send_control_hmi((char*)temp_button_buffer);
    }
}

void temp_down(uint8_t action){
    if(action==0x01){
				TEMP_Down=0;
				sprintf((char *)temp_button_buffer,"page0.bt2.val=1");
				send_control_hmi((char*)temp_button_buffer);
				delay_ms(5);
				sprintf((char *)temp_button_buffer,"page0.bt2.txt=\"降温开\"");
				send_control_hmi((char*)temp_button_buffer);
    }else if(action==0x00){
				TEMP_Down=1;
				sprintf((char *)temp_button_buffer,"page0.bt2.val=0");
				send_control_hmi((char*)temp_button_buffer);
				delay_ms(5);
				sprintf((char *)temp_button_buffer,"page0.bt2.txt=\"降温关\"");
				send_control_hmi((char*)temp_button_buffer);
    }
}

void wet_up(uint8_t action){
    if(action==0x01){
        WET_OUT=0;
				sprintf((char *)wet_button_buffer,"page0.bt3.val=1");
				send_control_hmi((char*)wet_button_buffer);
				delay_ms(5);
				sprintf((char *)wet_button_buffer,"page0.bt3.txt=\"加湿开\"");
				send_control_hmi((char*)wet_button_buffer);
    }else if(action==0x00){
				WET_OUT=1;
				sprintf((char *)wet_button_buffer,"page0.bt3.val=0");
				send_control_hmi((char*)wet_button_buffer);
				delay_ms(5);
				sprintf((char *)wet_button_buffer,"page0.bt3.txt=\"加湿关\"");
				send_control_hmi((char*)wet_button_buffer);
    }
}
void wet_down(uint8_t action){
    if(action==0x01){
        FAN_OUT=0;
				sprintf((char *)wet_button_buffer,"page0.bt4.val=1");
				send_control_hmi((char*)wet_button_buffer);
				delay_ms(5);
				sprintf((char *)wet_button_buffer,"page0.bt4.txt=\"除湿开\"");
				send_control_hmi((char*)wet_button_buffer);
    }else if(action==0x00){
				FAN_OUT=1;
				sprintf((char *)wet_button_buffer,"page0.bt4.val=0");
				send_control_hmi((char*)wet_button_buffer);
				delay_ms(5);
				sprintf((char *)wet_button_buffer,"page0.bt4.txt=\"除湿关\"");
				send_control_hmi((char*)wet_button_buffer);
    }
}

void window_open_close(uint8_t action){
    if(action==0x01){
        open_window();
				sprintf((char *)window_buffer,"page0.bt0.val=1");
				send_control_hmi((char*)window_buffer);
				delay_ms(5);
				sprintf((char *)window_buffer,"page0.bt0.txt=\"窗户开\"");
				send_control_hmi((char*)window_buffer);
    }else if(action==0x00){
				close_window();
				sprintf((char *)window_buffer,"page0.bt0.val=0");
				send_control_hmi((char*)window_buffer);
				delay_ms(5);
				sprintf((char *)window_buffer,"page0.bt0.txt=\"窗户关\"");
				send_control_hmi((char*)window_buffer);
    }
}

void role_up_down(uint8_t action){
		if(action==0x01){
        role_up();
				sprintf((char *)role_buffer,"page0.bt5.val=1");
				send_control_hmi((char*)role_buffer);
				delay_ms(5);
				sprintf((char *)role_buffer,"page0.bt5.txt=\"晾衣杆伸\"");
				send_control_hmi((char*)role_buffer);
    }else if(action==0x00){
				role_down();
				sprintf((char *)role_buffer,"page0.bt5.val=0");
				send_control_hmi((char*)role_buffer);
				delay_ms(5);
				sprintf((char *)role_buffer,"page0.bt5.txt=\"晾衣杆缩\"");
				send_control_hmi((char*)role_buffer);
    }
}

void door_open_close(uint8_t action){
		if(action==0x01){
        door_open();
				sprintf((char *)door_buffer,"page0.bt7.val=1");
				send_control_hmi((char*)door_buffer);
				delay_ms(5);
				sprintf((char *)door_buffer,"page0.bt7.txt=\"门开\"");
				send_control_hmi((char*)door_buffer);
    }else if(action==0x00){
				door_close();
				sprintf((char *)door_buffer,"page0.bt7.val=0");
				send_control_hmi((char*)door_buffer);
				delay_ms(5);
				sprintf((char *)door_buffer,"page0.bt7.txt=\"门关\"");
				send_control_hmi((char*)door_buffer);
    }
}

void homelight(uint8_t action){
	    if(action==0x01){
        LED1=0;
    }else if(action==0x00){
				LED1=1;
    }
}

void holllight(uint8_t action){
	  if(action==0x01){
        LED2=0;
    }else if(action==0x00){
				LED2=1;
    }
}

void roomlight(uint8_t action){
	  if(action==0x01){
        LED3=0;
    }else if(action==0x00){
				LED3=1;
    }
}
void wclight(uint8_t action){
	   if(action==0x01){
        LED4=0;
    }else if(action==0x00){
				LED4=1;
    }
}

void cflight(uint8_t action){
	    if(action==0x01){
        LED5=0;
    }else if(action==0x00){
				LED5=1;
    }
}
void alllight(uint8_t action){
	    if(action==0x01){
        LED1=0;
				LED2=0;
				LED3=0;
				LED4=0;
				LED5=0;
    }else if(action==0x00){
				LED1=1;
				LED2=1;
				LED3=1;
				LED4=1;
				LED5=1;
    }
}

void wc_fan(uint8_t action){
		if(action==0x01){
        WC_FAN=0;
				sprintf((char *)wc_fan_buffer,"page1.bt6.val=1");
				send_control_hmi((char*)wc_fan_buffer);
				delay_ms(5);
				sprintf((char *)wc_fan_buffer,"page1.bt6.txt=\"开\"");
				send_control_hmi((char*)wc_fan_buffer);
    }else if(action==0x00){
				WC_FAN=1;
				sprintf((char *)wc_fan_buffer,"page1.bt6.val=0");
				send_control_hmi((char*)wc_fan_buffer);
				delay_ms(5);
				sprintf((char *)wc_fan_buffer,"page1.bt6.txt=\"关\"");
				send_control_hmi((char*)wc_fan_buffer);
    }
}

void deal_time(uint8_t hour,uint8_t minut){
		hours = hour;
		minute = minut;
		if(hours>12){
			RTC_Set_Time(hours,minute,40,RTC_H12_PM);
		}else{
			RTC_Set_Time(hours,minute,40,RTC_H12_AM);
		}
}

void road_light(uint8_t action){
		if(action==0x01){
        Light_road=1;
				sprintf((char *)wc_fan_buffer,"page1.bt7.val=1");
				send_control_hmi((char*)wc_fan_buffer);
				delay_ms(5);
				sprintf((char *)wc_fan_buffer,"page1.bt7.txt=\"开\"");
				send_control_hmi((char*)wc_fan_buffer);
    }else if(action==0x00){
				Light_road=0;
				sprintf((char *)wc_fan_buffer,"page1.bt7.val=0");
				send_control_hmi((char*)wc_fan_buffer);
				delay_ms(5);
				sprintf((char *)wc_fan_buffer,"page1.bt7.txt=\"关\"");
				send_control_hmi((char*)wc_fan_buffer);
    }
}

void atmosphere_light_su(uint8_t action){
		if(action==0x01){	//初始化开启蓝
			BLUE=0;
			RED=1;
			GREEN=1;
    }else if(action==0x02){	// 变为红色
			BLUE=1;
			RED=0;
			GREEN=1;
		}else if(action==0x03){	// 变为绿色
			BLUE=1;
			RED=1;
			GREEN=0;
		}else if(action==0x04){	// 音乐模式
			music_atmosphere_light();
		}else if(action==0x05){
			BLUE=1;
			RED=1;
			GREEN=1;
		}
}

void atmosphere_light_lcd(uint8_t action){
		if(action==0x01){	//初始化开启蓝
			BLUE=0;
    }else if(action==0x02){
			BLUE=1;
		}else if(action==0x03){	// 开红
			RED=0;
		}else if(action==0x04){
			RED=1;
		}else if(action==0x05){	// 开绿
			GREEN=0;
		}else if(action==0x06){
			GREEN=1;
		}else if(action==0x07){	// 音乐模式
			music_atmosphere_light();
		}else if(action==0x08){
			BLUE=1;
			RED=1;
			GREEN=1;			
		}
		
}


void music_atmosphere_light(void){
	RED=0;
	delay_ms(500);
	BLUE=0;
	delay_ms(500);
	GREEN=0;
	delay_ms(500);
	RED=1;	// B G
	delay_ms(500);
	RED=0;	// R G
	BLUE=1;
	delay_ms(500);
	BLUE=0;	// R B
	GREEN=1;
	delay_ms(500);
}

uint8_t dec_to_hex(uint8_t num){
	switch(num){
		case 1:
			return 0x01;
			break;
		case 2:
			return 0x02;
			break;
		case 3:
			return 0x03;
			break;
		case 4:
			return 0x04;
			break;
		case 5:
			return 0x05;
			break;
		case 6:
			return 0x06;
			break;
		case 7:
			return 0x07;
			break;
		case 8:
			return 0x08;
			break;
		case 9:
			return 0x09;
			break;
		case 10:
			return 0x0A;
			break;
		case 11:
			return 0x0B;
			break;
		case 12:
			return 0x0C;
			break;
		case 13:
			return 0x0D;
			break;
		case 14:
			return 0x0E;
			break;
		case 15:
			return 0x0F;
			break;
		case 16:
			return 0x10;
			break;
		case 17:
			return 0x11;
			break;
		case 18:
			return 0x12;
			break;
		case 19:
			return 0x13;
			break;
		case 20:
			return 0x14;
			break;
		case 21:
			return 0x15;
			break;
		case 22:
			return 0x16;
			break;
		case 23:
			return 0x17;
			break;
		case 24:
			return 0x18;
			break;
		case 25:
			return 0x19;
			break;
		case 26:
			return 0x1A;
			break;
		case 27:
			return 0x1B;
			break;
		case 28:
			return 0x1C;
			break;
		case 29:
			return 0x1D;
			break;
		case 30:
			return 0x1E;
			break;
		case 31:
			return 0x1F;
			break;
		case 32:
			return 0x20;
			break;
		case 33:
			return 0x21;
			break;
		case 34:
			return 0x22;
			break;
		case 35:
			return 0x23;
			break;
		case 36:
			return 0x24;
			break;
		case 37:
			return 0x25;
			break;
		case 38:
			return 0x26;
			break;
		case 39:
			return 0x27;
			break;
		case 40:
			return 0x28;
			break;
		case 41:
			return 0x29;
			break;
		case 42:
			return 0x2A;
			break;
		case 43:
			return 0x2B;
			break;
		case 44:
			return 0x2C;
			break;
		case 45:
			return 0x2D;
			break;
		case 46:
			return 0x2E;
			break;
		case 47:
			return 0x2F;
			break;
		case 48:
			return 0x30;
			break;
		case 49:
			return 0x31;
			break;
		case 50:
			return 0x32;
			break;
		case 51:
			return 0x33;
			break;
		case 52:
			return 0x34;
			break;
		case 53:
			return 0x35;
			break;
		case 54:
			return 0x36;
			break;
		case 55:
			return 0x37;
			break;
		case 56:
			return 0x38;
			break;
		case 57:
			return 0x39;
			break;
		case 58:
			return 0x3A;
			break;
		case 59:
			return 0x3B;
			break;
		default:
			return 0x00;
			break;
	}
}