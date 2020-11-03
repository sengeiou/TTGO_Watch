/*
  Jack Din
  该项目为DC人机网络远程控制接收端192.168.0.107，使用ESP NOW技术为无线通讯载体。
*/

//@1-头文件包含
#include <SPI.h> 
#include <Ethernet.h>
#include <EthernetUdp.h>

#include <esp_now.h>
#include <WiFi.h>
#include <ESP32Servo.h>

//@2-简单的伺服控制
#ifdef ESP32_Servo
Servo myservo;
// Recommended PWM GPIO pins on the ESP32 include 2,4,12-19,21-23,25-27,32-33 
int servoPin = 13;
#endif


//@7-手表MAC地址表
uint8_t broadcastAddress1[] = {0x8C, 0xAA, 0xB5, 0x82, 0xEA, 0x58};    //TTGO_2020 白色
uint8_t broadcastAddress2[] = {0x10, 0x52, 0x1C, 0x65, 0x42, 0x84};    //TTGO_2020 黑色

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

} struct_message;

//@9-测试用数据收发报文
struct_message  send_Data;
struct_message  recv_Data;

//@10-本地网络MAC地址
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
  // 0x00, 0xE0, 0x4C, 0xA0, 0x7E, 0x61
};

//@11-DSP网络各节点IP
IPAddress local_IMX6(192,168,0,106);
IPAddress local_ip(192,168,0,107);

//@12-DSP网络组播地址及port
IPAddress ip_m(224, 100, 23, 200); 
short port_m = 6000;      

//@14-测试用数据
int count = 0;
int count1 = 0;
int count_temp = 0;

//@15-测试用数据
// buffers for receiving and sending data
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];  // buffer to hold incoming packet,
byte  ReplyBuffer[128];        // a string to send back

//@16-UDP报文socket
EthernetUDP Udp_m;

//@17-DSP网络各节点数据报文接收计数
long Recv_Packet_Count = 0;

//@18-测试数据
int data = 0;

//@19-网络物理连接标志
bool Ethernet_Connect_Flag = false;

//@20-网络发送计数
int ESP32_Send_Count  = 0;


//-----------------------ESP NOW---------------------------------
// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  // Serial.print("\r\nLast Packet Send Status:\t");
  // Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&recv_Data, incomingData, sizeof(recv_Data));
}


//--------------------------------------------------------
//@-系统配置 
void setup() {

  //@-初始化串口
  Serial.begin(115200);

  //@-配置板载LED指示灯
  pinMode(2, OUTPUT);

  #ifdef ESP32_Servo
  myservo.setPeriodHertz(50);    // standard 50 hz servo
  myservo.attach(servoPin, 500, 2500); // attaches the servo on pin 18 to the servo object
  #endif

  //@-网络接口初始化
  Ethernet.init(5);

  //@-绑定自身IP和MAC
  Ethernet.begin(mac, local_ip);  //@-测试用(mac2, local_ip_test)

  //@-检测网络物理连接
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    // while (true) {
    //   delay(1); // do nothing, no point running without Ethernet hardware
    // }
  }
  if (Ethernet.linkStatus() == LinkOFF) {  
    Serial.println("Ethernet cable is not connected.");
    Ethernet_Connect_Flag = false;
  }
  else if (Ethernet.linkStatus() == LinkON)
  {
    Ethernet_Connect_Flag = true;
  }
  
  //@-配置UDP发送
  if(Ethernet_Connect_Flag ==  true)
  {
    //@-配置UDP接口信息
    // Udp_fy.begin(local_fy_Port);
    // Udp_fy2.begin(local_fy2_Port);

    // Udp_xh.begin(local_xh_Port);
    // Udp_xh2.begin(local_xh2_Port);

    //@-组播
    Udp_m.beginMulticast(ip_m, port_m);
  }
 
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  //@-10:52:1C:67:0B:E8 - esp32
  //@-84:0D:8E:0B:B2:54 - esp32
  // Serial.println(WiFi.macAddress());    


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
  memcpy(peerInfo1.peer_addr, broadcastAddress1, 6);
  peerInfo1.channel = 0;  
  peerInfo1.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo1) != ESP_OK){
    Serial.println("Failed to add peer1");
    return;
  }

  //@-注册NOW通讯节点2
  esp_now_peer_info_t peerInfo2;
  memcpy(peerInfo2.peer_addr, broadcastAddress2, 6);
  peerInfo2.channel = 1;  
  peerInfo2.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo2) != ESP_OK){
    Serial.println("Failed to add peer2");
    return;
  }

  #ifdef ESP32_Servo
  myservo.write(180);
  #endif

}
 

//@-系统主循环
void loop() 
{

  count =  count + 1;
  count1 =  count1 + 1;
  count_temp =  count_temp + 1;

  #ifdef ESP32_Servo
  myservo.write(recv_Data.b);
  #endif

  //@-LED指示灯显示
  if(recv_Data.e == false)
  {
    // led_flag = true;
    digitalWrite(2, HIGH);
  }
  else if(recv_Data.e == true)
  {
    // led_flag = false;
    digitalWrite(2, LOW);
  }

  //@-DSP以太网解包
  if(Ethernet_Connect_Flag == true)
  {
    // if there's data available, read a packet
    int packetSize = Udp_m.parsePacket();

    if(packetSize == 128)
    {
      Recv_Packet_Count = Recv_Packet_Count + 1;

      // read the packet into packetBufffer
      Udp_m.read(packetBuffer, 128);
    }

  
      //@-网络数据包信息
      // Serial.print("Received packet of size ");
      // Serial.println(packetSize);
      // Serial.print("From ");
      // IPAddress remote = Udp_fy.remoteIP();
      // for (int i=0; i < 4; i++) {
      //   Serial.print(remote[i], DEC);
      //   if (i < 3) {
      //     Serial.print(".");
      //   }
      // }
      // Serial.print(", port ");
      // Serial.println(Udp_fy.remotePort());

      // read the packet into packetBufffer
      // Udp_xh.read(Snet_ESP32_xh_Recv.DSP_Data_Buff128, 128);

      // Serial.println("Contents:");
      // Serial.println(packetBuffer);

      // myservo.write(packetBuffer[0]);

      // send a reply to the IP address and port that sent us the packet we received
  //    Udp_fy.beginPacket(Udp_fy.remoteIP(), Udp_fy.remotePort());
  //    Udp_fy.write(ReplyBuffer);
  //    Udp_fy.endPacket();

  }

  //@-DSP以太网发送
  if(count > 10)
  {
    count = 0;

    if(Ethernet_Connect_Flag == true)
    {
      ESP32_Send_Count = ESP32_Send_Count + 1;

      if(ESP32_Send_Count > 65535)
      ESP32_Send_Count = 0;

      Udp_m.beginPacket(ip_m, port_m);
      Udp_m.write(ReplyBuffer,128);  //write byte
      Udp_m.endPacket(); 

    }
  }

  //@-ESP NOW定时发送
  if(count1 > 40)
  {
    count1 = 0;

  //   // Set values to send
  //   strcpy(send_Data.a, "THIS IS A CHAR");
  // // send_Data.b = random(1,20);
    data = data + 1;
    if(data > 500)
    data = 0;
  //   send_Data.b = data;
  //   send_Data.c = 1.2;
  //   send_Data.d = "Hello";
  //   send_Data.e = false;

    // Snet_ESP32_xh_Recv.DSP_Data_str.ECANA_INDEX_FLOWNO_CZP = data;


    //@-转发接收到的DSP数据 Send message via ESP-NOW 
    // esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &send_Data, sizeof(send_Data));
    esp_err_t result;
    // result = esp_now_send(broadcastAddress1, (uint8_t *) &Snet_ESP32_xh_Recv.DSP_Data_Buff128, sizeof(Snet_ESP32_xh_Recv.DSP_Data_Buff128));
    // result = esp_now_send(broadcastAddress2, (uint8_t *) &Snet_ESP32_xh_Recv.DSP_Data_Buff128, sizeof(Snet_ESP32_xh_Recv.DSP_Data_Buff128));
    result = esp_now_send(broadcastAddress1, (uint8_t *) &send_Data, sizeof(send_Data));
    result = esp_now_send(broadcastAddress2, (uint8_t *) &send_Data, sizeof(send_Data));


  }

  //@-串口数据出数据
  if(count_temp > 400)
  {
    count_temp = 0;

    // Snet_ESP32_xh_Recv.DSP_Data_str.ECANA_INDEX_ERROR1
    // Snet_ESP32_xh_Recv.DSP_Data_str.ECANA_INDEX_FLOWNO_453

    Serial.print("Recv:");
    Serial.println(Recv_Packet_Count);

    // Serial.print("Error:");
    // Serial.println(Snet_ESP32_xh_Recv.DSP_Data_str.ECANA_INDEX_ERROR1);
  }
  
  //@-系统延时
  delay(2);   //@-1:500/s  2:250/s
}
