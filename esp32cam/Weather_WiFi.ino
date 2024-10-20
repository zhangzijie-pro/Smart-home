#include <ArduinoJson.h>
#include <ArduinoJson.hpp>

#define BLINKER_WIFI
//#define BLINKER_ESP_SMARTCONFIG

#include <Blinker.h>
#include "./ESP_CAM_SERVER.h"

// Basic message
char auth[] = "0e70a562897a";
char ssid[] = "vivo";
char pswd[] = "zzj55555";
bool setup_camera = false;
int address = 410100;

const int packetSize = 4; // 数据包大小为4位
char hexData[packetSize + 1]; // 用于存储接收到的4位hex数据（+1是为了字符串的结束符）

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

BlinkerNumber Number1("num-temp");
BlinkerNumber Number2("num-humi");
BlinkerText tex_ok("tex_ok");

int temp=24;
int humi=49;
String state_home = "家庭状况:正常";
unsigned char sendcmd[4]={0x55,0x00,0x00,0xFF};    //定义数据包
unsigned long previousMillis = 0; // 上一次更新时间
const long interval = 3600000; // 1小时的毫秒数（3600000毫秒）

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

void deal_data(String data){
  if(data=="1"){        // 断开
    Serial.println("执行逻辑1");
    Blinker.disconnect();
  }else if(data=="2"){  // 连接
    Serial.println("执行逻辑2");
    Blinker.begin(auth, ssid, pswd);
    Blinker.connect();
  }else if(data=="3"){  // 异常
    Serial.println("执行逻辑3");
    state_home="家庭状况:异常";
    Blinker.vibrate(20000); 
  }else if(data=="4"){  // 正常
    state_home="家庭状况:正常";
    Serial.println("执行逻辑4");
  }else{
    sscanf(data.c_str(), "%d %d", &temp, &humi);
  }
}


//---------------------------Button事件---------------------------


void dataStorage(){
  tex_ok.print("tex_ok",state_home);
  Number1.print(temp);
  Number2.print(humi);
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
  } else if(state=="off"){
    // TODO
    sendcmd[1]=0x11;
    sendcmd[2]=0x00;
    Serial.write(sendcmd,sizeof(sendcmd));
    // 反馈开关状态
    Buttonl6.print("off");
  }
}

void setup()
{
    Serial.begin(115200);
    BLINKER_DEBUG.stream(Serial);

    Blinker.begin(auth, ssid, pswd);
    bool result = Blinker.connected();
    if(result){
      sendcmd[1]=0x20;
      sendcmd[2]=0x01;
      Serial.write(sendcmd,sizeof(sendcmd));
    }
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
    
    Blinker.attachDataStorage(dataStorage);
}

void loop()
{
    Blinker.run();
    if (Serial.available()) { // 检查是否有足够的数据可用
      if (Serial.read() == 0x55) { // 检查帧头
        String recv_data="";
        while(true){
          char c=Serial.read();
          if(c==0xFF){
            break;
          }
          recv_data+=c;
        }
        deal_data(recv_data);
      }
    }

    //192.168.43.239
    if (Blinker.connected() && !setup_camera)
    {   // 摄像头网络成功
        sendcmd[1]=0x40;
        sendcmd[2]=0x00;
        Serial.write(sendcmd,sizeof(sendcmd));
        setupCamera();
        setup_camera = true;

        Blinker.printObject("video", "{\"str\":\"mjpg\",\"url\":\"http://"+ WiFi.localIP().toString() + "\"}");
    }
}