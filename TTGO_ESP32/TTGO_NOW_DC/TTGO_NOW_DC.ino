#include <SPI.h>
#include <TFT_eSPI.h> 

#include <esp_now.h>
#include <WiFi.h>

#define TFT_GREY 0x5AEB

//@8-测试用数据报文结构定义
typedef struct struct_message {
  char a[32];
  int b;
  float c;
  String d;
  bool e;

  bool FY_DSP_Reset;  
  bool FY_DSP_Run;
  int  FY_DSP_Dir;
  int  FY_DSP_Speed;

  bool XH_DSP_Reset;
  bool XH_DSP_Run;
  int  XH_DSP_Dir;
  int  XH_DSP_Speed;

  //@-20210311-DC远程控制设备协议
  int  FY_Angel_Byte1;
  int  FY_Angel_Byte2;
  int  XH_Angel_Byte1;
  int  XH_Angel_Byte2;

  int  ZZ_MainPower;

} struct_message;

//@9-测试用数据收发报文
struct_message  DC_send_Data;
struct_message  DC_recv_Data;

//@-esp32-w5500 now max
uint8_t broadcastAddress_DC[] = {0x10, 0x52, 0x1C, 0x67, 0x0B, 0xE8};    

int count = 0;
int now_recv_count = 0;

TFT_eSPI tft = TFT_eSPI(); 

const int pwmFreq = 5000;
const int pwmResolution = 8;
const int pwmLedChannelTFT = 0;

int temp_data1 = 0; 
int temp_data2 = 0; 
int temp_data3 = 0;
int ZZ_MainPower = 0;
float FY_Angel = 0;
float XH_Angel = 0;

//@-KEY状态
uint8_t KEY_Left_Status = 0;
uint8_t KEY_Right_Status = 0;

//-----------------------ESP NOW---------------------------------
// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  // Serial.print("\r\nLast Packet Send Status:\t");
  // Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
//   Serial.print("\r\nRecv Packet!");
  now_recv_count = now_recv_count + 1;
  memcpy(&DC_recv_Data, incomingData, sizeof(DC_recv_Data));
}


//@-系统配置 
void setup() 
{
  //@-初始化串口
  Serial.begin(115200);

  //@-TFT屏幕初始化
  tft.init();
  tft.fillScreen(TFT_BLACK);
  tft.setSwapBytes(true); 

  //@-设置屏幕背光
  ledcSetup(pwmLedChannelTFT, pwmFreq, pwmResolution);
  ledcAttachPin(TFT_BL, pwmLedChannelTFT);
  ledcWrite(pwmLedChannelTFT, 100);

  //@-配置按键
  pinMode(0,INPUT_PULLUP);
  pinMode(35,INPUT_PULLUP);

  //@-Set device as a Wi-Fi Station
  //@-TTGO 84:CC:A8:61:0A:28
  WiFi.mode(WIFI_STA);
//   Serial.println(WiFi.macAddress()); 

  //@-初始化ESP NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  // esp_now_register_send_cb(OnDataSent);

  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);

  //@-注册NOW通讯节点1
  esp_now_peer_info_t peerInfo1;
  memcpy(peerInfo1.peer_addr, broadcastAddress_DC, 6);
  peerInfo1.channel = 0;  
  peerInfo1.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo1) != ESP_OK){
    Serial.println("Failed to add peer1");
    return;
  }

}

//@-系统主循环
void loop() 
{
  //@-运行tick计数
  count =  count + 1;

  //@-检测按键状态
  KEY_Left_Status = digitalRead(0);
  KEY_Right_Status = digitalRead(35);

  //@-计算FY角度
  temp_data1 = DC_recv_Data.FY_Angel_Byte1;
  temp_data2 = DC_recv_Data.FY_Angel_Byte2;
  temp_data3 = ((temp_data1 * 256) + temp_data2);
  if(temp_data1 >= 128)
  {
      temp_data3 = temp_data3 - 65535 -1;
  }
  FY_Angel = temp_data3 * 0.02;

  //@-计算XH角度
  temp_data1 = DC_recv_Data.XH_Angel_Byte1;
  temp_data2 = DC_recv_Data.XH_Angel_Byte2;
  temp_data3 = ((temp_data1 * 256) + temp_data2);
  if(temp_data1 >= 128)
  {
      temp_data3 = temp_data3 - 65535 -1;
  }
  XH_Angel = temp_data3 * 0.02;

  //@-获得装置高压状态
  ZZ_MainPower = DC_recv_Data.ZZ_MainPower;

  //@-显示高压状态
  if(ZZ_MainPower == 0)
  // fillRect(0, 0, _width, _height, color)
  tft.fillRect(0,81,135,20,TFT_RED);
  else if(ZZ_MainPower == 1)
  tft.fillRect(0,81,135,20,TFT_GREEN);

  //@-按键左键状态
  if(KEY_Left_Status == 0)
  // fillRect(0, 0, _width, _height, color)
  tft.fillRect(0,51,60,20,TFT_RED);
  else if(KEY_Left_Status == 1)
  tft.fillRect(0,51,60,20,TFT_GREEN);

  //@-按键右键状态
  if(KEY_Right_Status == 0)
  tft.fillRect(75,51,60,20,TFT_RED);
  else if(KEY_Right_Status == 1)
  tft.fillRect(75,51,60,20,TFT_GREEN);

  //@-显示FY/XH 角度
  if(FY_Angel < 0)
  FY_Angel = FY_Angel - 0.01;
  else
  FY_Angel = FY_Angel + 0.01;

  if(XH_Angel < 0)
  XH_Angel = XH_Angel - 0.01;
  else
  XH_Angel = XH_Angel + 0.01;
  //@-drawString(const String &string, int32_t poX, int32_t poY, uint8_t font)
  tft.drawString("Count: "+String(now_recv_count),1,1,2);
  tft.drawString("FY_Angel: "+String(FY_Angel),1,17,2);
  // tft.setTextColor(TFT_RED,TFT_BLACK);
  tft.drawString("XH_Angel: "+String(XH_Angel),1,33,2);
  // tft.setTextColor(TFT_WHITE,TFT_BLACK);


  //@-串口调试输出
  if(count > 200)
  {
    count = 0;
    Serial.println(now_recv_count);
  }

  //@-系统延时
  delay(2);

}