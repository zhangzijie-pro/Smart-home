#include <ArduinoJson.h>
#include <ArduinoJson.hpp>

#define BLINKER_WIFI
//#define BLINKER_ESP_SMARTCONFIG

#include <WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <Blinker.h>
#include "./ESP_CAM_SERVER.h"

// Wifi time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "ntp1.aliyun.com",60*60*8, 30*60*1000);

// Basic message
char auth[] = "0e70a562897a";
char ssid[] = "vivo";
char pswd[] = "zzj55555";
bool setup_camera = false;

// 新建组件对象
BlinkerButton Buttontemp1("btn-tempup");
BlinkerButton Buttontemp2("btn-tempdown");
BlinkerButton Buttonwet1("btn-wetup");
BlinkerButton Buttonwet2("btn-wetdown");
BlinkerButton Buttonwindow("btn-window");
BlinkerButton Buttonrole("btn-role");
BlinkerButton Buttondoor("btn-door");
BlinkerButton Buttonf("btn-f2");
BlinkerButton Buttonl1("btn-l1");
BlinkerButton Buttonl2("btn-l2");
BlinkerButton Buttonl3("btn-l3");
BlinkerButton Buttonl4("btn-l4");
BlinkerButton Buttonl5("btn-l5");
BlinkerButton Buttonl6("btn-l6");
BlinkerButton Buttontime("btn-time");
BlinkerButton Buttonroad("btn-road");
BlinkerButton Buttonfl4("btn-fl4");
BlinkerButton Buttonyu1("btn-yu1");
BlinkerButton Buttonyu2("btn-yu2");
BlinkerButton Buttonyu3("btn-yu3");
BlinkerButton Buttonyu4("btn-yu4");

BlinkerNumber Number1("num-temp");
BlinkerNumber Number2("num-humi");
BlinkerNumber Number3("num-yutemp");
BlinkerNumber Number4("num-yuhumi");
int temp=24;
int humi=49;
int temp_avager = 25;
int humi_avager = 50;
String state_home = "家庭状况:正常";
byte recv_data[4];
unsigned char sendcmd[5]={0x55,0x00,0x00,0x00,0xFF};    //定义数据包
unsigned long previousMillis = 0; // 上一次更新时间
const long interval = 3600000; // 1小时的毫秒数（3600000毫秒）
//unsigned char hex_time;
int hours;
int minutes;
/*
---------------------------串口数据---------------------------
*/
// 湿度,温度.  正常，异常   连接，断开连接 
/*
      数据包格式
    0x55 data 0xff
      data: 2位
*/
// send 55 00 01 01 ff
// 解析数据包

void deal_data(byte data[],int len){
  if(data[0]==0x01){        // 断开
    //Serial.println("执行逻辑1");
    Blinker.disconnect();
  }else if(data[0]==0x02){  // 连接
    //Serial.println("执行逻辑2");
    Blinker.begin(auth, ssid, pswd);
    Blinker.connect();
  }else if(data[0]==0x03){  // 异常
    //Serial.println("执行逻辑3");
    state_home="家庭状况:异常";
    Blinker.vibrate(20000); 
    Blinker.vibrate(20000); 
    Blinker.vibrate(20000); 
    Blinker.vibrate(20000); 
    Blinker.vibrate(20000);
  }else if(data[0]==0x04){  // 正常
    state_home="家庭状况:正常";
    //Serial.println("执行逻辑4");
  }else if(data[0]==0x05){  
   temp=data[1];
   humi=data[2];
  }else if(data[0]==0x06){  // 关闭晾衣杆
    Buttonrole.print("off");
  }
}


//---------------------------Button事件---------------------------


void dataStorage(){
  Number1.print(temp);
  Number2.print(humi);
  Number3.print(temp_avager);
  Number4.print(humi_avager);
}
void buttonroad_callback(const String & state){
    if (state=="on") {
    // TODO
    sendcmd[1]=0x15;
    sendcmd[2]=0x01;
    Serial.write(sendcmd,sizeof(sendcmd));
    // 反馈开关状态
    Buttonroad.print("on");
  } else if(state=="off"){
    // TODO
    sendcmd[1]=0x15;
    sendcmd[2]=0x00;
    Serial.write(sendcmd,sizeof(sendcmd));
    // 反馈开关状态
    Buttonroad.print("off");
  }
}

void buttonyu1_callback(const String & state){
    temp_avager++;
    Number3.print(temp_avager); //回传数据到app组件
    sendcmd[1]=0x1A;
    sendcmd[2]=0x01;
    Serial.write(sendcmd,sizeof(sendcmd));
}

void buttonyu2_callback(const String & state){
    temp_avager--;
    Number3.print(temp_avager); //回传数据到app组件
    sendcmd[1]=0x1A;
    sendcmd[2]=0x00;
    Serial.write(sendcmd,sizeof(sendcmd));
}

void buttonyu3_callback(const String & state){
    humi_avager++;
    Number4.print(humi_avager); //回传数据到app组件
    sendcmd[1]=0x1B;
    sendcmd[2]=0x01;
    Serial.write(sendcmd,sizeof(sendcmd));
}

void buttonyu4_callback(const String & state){
    humi_avager--;
    Number4.print(humi_avager); //回传数据到app组件
    sendcmd[1]=0x1B;
    sendcmd[2]=0x01;
    Serial.write(sendcmd,sizeof(sendcmd));
}

void buttonfl4_callback(const String & state){
    if (state=="on") {
    // TODO
    sendcmd[1]=0x19;
    sendcmd[2]=0x07;
    Serial.write(sendcmd,sizeof(sendcmd));
    // 反馈开关状态
    Buttonfl4.print("on");
  } else if(state=="off"){
    // TODO
    sendcmd[1]=0x19;
    sendcmd[2]=0x08;
    Serial.write(sendcmd,sizeof(sendcmd));
    // 反馈开关状态
    Buttonfl4.print("off");
  }
}

void buttontime_callback(const String & state){
    if (state=="on") {
    // TODO
    sendcmd[1]=0x16;
    sendcmd[2]=hours;
    sendcmd[3]=minutes;
    Serial.write(sendcmd,sizeof(sendcmd));
    // 反馈开关状态
    Buttontime.print("on");
  } else if(state=="off"){
    // TODO
    sendcmd[1]=0x16;
    sendcmd[2]=hours;
    sendcmd[3]=minutes;
    Serial.write(sendcmd,sizeof(sendcmd));
    // 反馈开关状态
    Buttontime.print("off");
  }
}

// 按下按键即会执行该函数
// 升温回调
void buttontemp1_callback(const String & state)
{
  //BLINKER_LOG("get buttontemp1 state: ", state);
  if (state=="on") {
    // TODO
    sendcmd[1]=0x01;
    sendcmd[2]=0x01;
    Serial.write(sendcmd,sizeof(sendcmd));
    // 反馈开关状态
    Buttontemp1.print("on");
  } else if(state=="off"){
    // TODO
    sendcmd[1]=0x01;
    sendcmd[2]=0x00;
    Serial.write(sendcmd,sizeof(sendcmd));
    // 反馈开关状态
    Buttontemp1.print("off");
  }
}

// 降温
void buttontemp2_callback(const String & state){
  //BLINKER_LOG("get buttontemp2 state: ", state);
  if (state=="on") {
    // TODO
    sendcmd[1]=0x02;
    sendcmd[2]=0x01;
    Serial.write(sendcmd,sizeof(sendcmd));
    // 反馈开关状态
    Buttontemp2.print("on");
  } else if(state=="off"){
    // TODO
    sendcmd[1]=0x02;
    sendcmd[2]=0x00;
    Serial.write(sendcmd,sizeof(sendcmd));
    // 反馈开关状态
    Buttontemp2.print("off");
  }
}

// 加湿
void buttonwet1_callback(const String & state){
  //BLINKER_LOG("get buttonwet1 state: ", state);
  if (state=="on") {
    // TODO
    sendcmd[1]=0x03;
    sendcmd[2]=0x01;
    Serial.write(sendcmd,sizeof(sendcmd));
    // 反馈开关状态
    Buttonwet1.print("on");
  } else if(state=="off"){
    // TODO
    sendcmd[1]=0x03;
    sendcmd[2]=0x00;
    Serial.write(sendcmd,sizeof(sendcmd));
    // 反馈开关状态
    Buttonwet1.print("off");
  }
}

// 抽湿
void buttonwet2_callback(const String & state){
  //BLINKER_LOG("get buttonwet2 state: ", state);
  if (state=="on") {
    // TODO
    sendcmd[1]=0x04;
    sendcmd[2]=0x01;
    Serial.write(sendcmd,sizeof(sendcmd));
    // 反馈开关状态
    Buttonwet2.print("on");
  } else if(state=="off"){
    // TODO
    sendcmd[1]=0x04;
    sendcmd[2]=0x00;
    Serial.write(sendcmd,sizeof(sendcmd));
    // 反馈开关状态
    Buttonwet2.print("off");
  }
}

// 窗户
void buttonwindow_callback(const String & state){
  //BLINKER_LOG("get buttonwindow state: ", state);
  if (state=="on") {
    // TODO
    sendcmd[1]=0x05;
    sendcmd[2]=0x01;
    Serial.write(sendcmd,sizeof(sendcmd));
    // 反馈开关状态
    Buttonwindow.print("on");
  } else if(state=="off"){
    // TODO
    sendcmd[1]=0x05;
    sendcmd[2]=0x00;
    Serial.write(sendcmd,sizeof(sendcmd));
    // 反馈开关状态
    Buttonwindow.print("off");
  }
}

// 晾衣杆
void buttonrole_callback(const String & state){
  //BLINKER_LOG("get Buttonrole state: ", state);
  if (state=="on") {
    // TODO
    sendcmd[1]=0x13;
    sendcmd[2]=0x01;
    Serial.write(sendcmd,sizeof(sendcmd));
    // 反馈开关状态
    Buttonrole.print("on");
  } else if(state=="off"){
    // TODO
    sendcmd[1]=0x13;
    sendcmd[2]=0x00;
    Serial.write(sendcmd,sizeof(sendcmd));
    // 反馈开关状态
    Buttonrole.print("off");
  }
}

// 晾衣杆
void buttondoor_callback(const String & state){
  //BLINKER_LOG("get Buttondoor state: ", state);
  if (state=="on") {
    // TODO
    sendcmd[1]=0x14;
    sendcmd[2]=0x01;
    Serial.write(sendcmd,sizeof(sendcmd));
    // 反馈开关状态
    Buttondoor.print("on");
  } else if(state=="off"){
    // TODO
    sendcmd[1]=0x14;
    sendcmd[2]=0x00;
    Serial.write(sendcmd,sizeof(sendcmd));
    // 反馈开关状态
    Buttondoor.print("off");
  }
}

// 厕所风扇
void buttonf2_callback(const String & state){
  //BLINKER_LOG("get buttonf2 state: ", state);
  if (state=="on") {
    // TODO
    sendcmd[1]=0x12;
    sendcmd[2]=0x01;
    Serial.write(sendcmd,sizeof(sendcmd));
    // 反馈开关状态
    Buttonf.print("on");
  } else if(state=="off"){
    // TODO
    sendcmd[1]=0x12;
    sendcmd[2]=0x00;
    Serial.write(sendcmd,sizeof(sendcmd));
    // 反馈开关状态
    Buttonf.print("off");
  }
}

// 客厅灯
void buttonl1_callback(const String & state){
  //BLINKER_LOG("get buttonl1 state: ", state);
  if (state=="on") {
    // TODO
    sendcmd[1]=0x06;
    sendcmd[2]=0x01;
    Serial.write(sendcmd,sizeof(sendcmd));
    // 反馈开关状态
    Buttonl1.print("on");
  } else if(state=="off"){
    // TODO
    sendcmd[1]=0x06;
    sendcmd[2]=0x00;
    Serial.write(sendcmd,sizeof(sendcmd));
    // 反馈开关状态
    Buttonl1.print("off");
  }
}

// 走廊灯
void buttonl2_callback(const String & state){
  //BLINKER_LOG("get buttonl1 state: ", state);
  if (state=="on") {
    // TODO
    sendcmd[1]=0x07;
    sendcmd[2]=0x01;
    Serial.write(sendcmd,sizeof(sendcmd));
    // 反馈开关状态
    Buttonl2.print("on");
  } else if(state=="off"){
    // TODO
    sendcmd[1]=0x07;
    sendcmd[2]=0x00;
    Serial.write(sendcmd,sizeof(sendcmd));
    // 反馈开关状态
    Buttonl2.print("off");
  }
}

// 卧室灯
void buttonl3_callback(const String & state){
  //BLINKER_LOG("get buttonl1 state: ", state);
  if (state=="on") {
    // TODO
    sendcmd[1]=0x08;
    sendcmd[2]=0x01;
    Serial.write(sendcmd,sizeof(sendcmd));
    // 反馈开关状态
    Buttonl3.print("on");
  } else if(state=="off"){
    // TODO
    sendcmd[1]=0x08;
    sendcmd[2]=0x00;
    Serial.write(sendcmd,sizeof(sendcmd));
    // 反馈开关状态
    Buttonl3.print("off");
  }
}

// 厕所灯
void buttonl4_callback(const String & state){
  //BLINKER_LOG("get buttonl1 state: ", state);
  if (state=="on") {
    // TODO
    sendcmd[1]=0x09;
    sendcmd[2]=0x01;
    Serial.write(sendcmd,sizeof(sendcmd));
    // 反馈开关状态
    Buttonl4.print("on");
  } else if(state=="off"){
    // TODO
    sendcmd[1]=0x09;
    sendcmd[2]=0x00;
    Serial.write(sendcmd,sizeof(sendcmd));
    // 反馈开关状态
    Buttonl4.print("off");
  }
}

// 厨房灯
void buttonl5_callback(const String & state){
  //BLINKER_LOG("get buttonl1 state: ", state);
  if (state=="on") {
    // TODO
    sendcmd[1]=0x10;
    sendcmd[2]=0x01;
    Serial.write(sendcmd,sizeof(sendcmd));
    // 反馈开关状态
    Buttonl5.print("on");
  } else if(state=="off"){
    // TODO
    sendcmd[1]=0x10;
    sendcmd[2]=0x00;
    Serial.write(sendcmd,sizeof(sendcmd));
    // 反馈开关状态
    Buttonl5.print("off");
  }
}

// 全部灯
void buttonl6_callback(const String & state){
  //BLINKER_LOG("get buttonl1 state: ", state);
  if (state=="on") {
    // TODO
    sendcmd[1]=0x11;
    sendcmd[2]=0x01;
    Serial.write(sendcmd,sizeof(sendcmd));
    // 反馈开关状态
    Buttonl6.print("on");
    Buttonl5.print("on");
    Buttonl4.print("on");
    Buttonl3.print("on");
    Buttonl2.print("on");
    Buttonl1.print("on");
  } else if(state=="off"){
    // TODO
    sendcmd[1]=0x11;
    sendcmd[2]=0x00;
    Serial.write(sendcmd,sizeof(sendcmd));
    // 反馈开关状态
    Buttonl6.print("off");
    Buttonl5.print("off");
    Buttonl4.print("off");
    Buttonl3.print("off");
    Buttonl2.print("off");
    Buttonl1.print("off");
  }
}

void setup()
{
    Serial.begin(9600);
    //BLINKER_DEBUG.stream(Serial);

    Blinker.begin(auth, ssid, pswd);
    timeClient.begin();
    Buttontemp1.attach(buttontemp1_callback);
    Buttontemp2.attach(buttontemp2_callback);
    Buttonwet1.attach(buttonwet1_callback);
    Buttonwet2.attach(buttonwet2_callback);
    Buttonwindow.attach(buttonwindow_callback);
    Buttonrole.attach(buttonrole_callback);
    Buttondoor.attach(buttondoor_callback);
    Buttonf.attach(buttonf2_callback);
    Buttonl1.attach(buttonl1_callback);
    Buttonl2.attach(buttonl2_callback);
    Buttonl3.attach(buttonl3_callback);
    Buttonl4.attach(buttonl4_callback);
    Buttonl5.attach(buttonl5_callback);
    Buttonl6.attach(buttonl6_callback);
    Buttontime.attach(buttontime_callback);
    Buttonroad.attach(buttonroad_callback);
    Buttonfl4.attach(buttonfl4_callback);

    // 阈值
    Buttonyu1.attach(buttonyu1_callback);
    Buttonyu2.attach(buttonyu2_callback);
    Buttonyu3.attach(buttonyu3_callback);
    Buttonyu4.attach(buttonyu4_callback);

    Blinker.attachDataStorage(dataStorage);
}

void loop()
{
    Blinker.run();
    if (Serial.available()) { // 检查是否有足够的数据可用
      if (Serial.read() == 0x55) { // 检查帧头
        int index=0;
        while(true){
          byte c=Serial.read();
          if(c==0xFF){
            break;
          }
          recv_data[index++]=c;
        }
        deal_data(recv_data,index+1);
      }
    }

    //192.168.43.239
    if (Blinker.connected() && !setup_camera)
    {   // 摄像头网络成功
        setupCamera();
        setup_camera = true;

        Blinker.printObject("video", "{\"str\":\"mjpg\",\"url\":\"http://"+ WiFi.localIP().toString() + "\"}");
    }

    timeClient.update();
    // Get current time
    hours = timeClient.getHours();
    minutes =  timeClient.getMinutes();
}