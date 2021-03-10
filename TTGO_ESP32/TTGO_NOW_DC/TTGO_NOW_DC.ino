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

  int  FY_Angel_Byte1;
  int  FY_Angel_Byte2;
  int  XH_Angel_Byte1;
  int  XH_Angel_Byte2;

  int  ZZ_MainPower;

} struct_message;

//@9-测试用数据收发报文
struct_message  DC_send_Data;
struct_message  DC_recv_Data;

uint8_t broadcastAddress_DC[] = {0x10, 0x52, 0x1C, 0x67, 0x0B, 0xE8};    

int count = 0;
int now_recv_count = 0;

TFT_eSPI tft = TFT_eSPI(); 

int temp_data1 = 0; 
int temp_data2 = 0; 
int temp_data3 = 0;
int ZZ_MainPower = 0;
float FY_Angel = 0;
float XH_Angel = 0;

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

  tft.init();
  tft.fillScreen(TFT_BLACK);
  tft.setSwapBytes(true); 

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
  count =  count + 1;

  temp_data1 = DC_recv_Data.FY_Angel_Byte1;
  temp_data2 = DC_recv_Data.FY_Angel_Byte2;
  temp_data3 = ((temp_data1 * 256) + temp_data2);
  if(temp_data1 >= 128)
  {
      temp_data3 = temp_data3 - 65535 -1;
  }
  FY_Angel = temp_data3 * 0.02;

  temp_data1 = DC_recv_Data.XH_Angel_Byte1;
  temp_data2 = DC_recv_Data.XH_Angel_Byte2;
  temp_data3 = ((temp_data1 * 256) + temp_data2);
  if(temp_data1 >= 128)
  {
      temp_data3 = temp_data3 - 65535 -1;
  }
  XH_Angel = temp_data3 * 0.02;

  ZZ_MainPower = DC_recv_Data.ZZ_MainPower;

//   if(ZZ_MainPower == 0)
//   tft.fillScreen(TFT_RED);
//   else if(ZZ_MainPower == 1)
//   tft.fillScreen(TFT_GREEN);

  //@-drawString(const String &string, int32_t poX, int32_t poY, uint8_t font)
  tft.drawString("Count: "+String(now_recv_count),1,1,2);
  tft.drawString("FY_Angel: "+String(FY_Angel),1,17,2);
  tft.drawString("XH_Angel: "+String(XH_Angel),1,33,2);

  if(count > 200)
  {
    count = 0;
    Serial.println(now_recv_count);
  }

  //@-系统延时
  delay(2);

}