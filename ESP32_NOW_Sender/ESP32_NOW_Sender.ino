/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp-now-esp32-arduino-ide/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

#include <SPI.h> 
#include <Ethernet.h>
#include <EthernetUdp.h>

#include <esp_now.h>
#include <WiFi.h>
#include <ESP32Servo.h>


#ifdef ESP32_Servo
Servo myservo;
// Recommended PWM GPIO pins on the ESP32 include 2,4,12-19,21-23,25-27,32-33 
int servoPin = 13;
#endif


typedef union	  /* DSP Send数据结构*/
{
	struct DSP_Send_str
	{
		short ECANA_INDEX_Iu;		             /* 0 -in 0.1A*/
		short ECANA_INDEX_Iv;		             /* 1 -in 0.1A*/
		short ECANA_INDEX_Iw;		             /* 2 -in 0.1A*/
		short ECANA_INDEX_BAT_Iuvw;		       /* 3 -in 0.1V*/
		short ECANA_INDEX_Uzk;		           /* 4 -in 0.1V*/
		short ECANA_INDEX_ID;		             /* 5 */
		short ECANA_INDEX_IQ;		             /* 6 */
		short ECANA_INDEX_SPEED;             /* 7 */
		short ECANA_INDEX_T1_LT;             /* 8 */
		short ECANA_INDEX_T3_MT;             /* 9 */

		short ECANA_INDEX_AnDRV;	    	     /* 10 */
		short ECANA_INDEX_AnBRK;		         /* 11 */
		short ECANA_INDEX_CMD_DI;            /* 12 ******/
		short ECANA_INDEX_ERROR1;		         /* 13 */
		short ECANA_INDEX_ERROR2;		         /* 14 */
		short ECANA_INDEX_ERROR3;		         /* 15 */
		short ECANA_INDEX_ERROR4;		         /* 16 */
		short ECANA_INDEX_PSWORD;            /* 17 */
		short ECANA_INDEX_FLOWNO_453;        /* 18 */
		short ECANA_INDEX_FLOWNO_CZP;        /* 19 */

		short ECANA_INDEX_UU_A1;		         /* 20 */
		short ECANA_INDEX_UV_A2;		         /* 21 */
		short ECANA_INDEX_UW;		             /* 22 */
		short ECANA_INDEX_IZK;		           /* 23 -in 0.1A*/
		short ECANA_INDEX_PZK;		           /* 24 -in W*/
		short ECANA_INDEX_UUV;		           /* 25 -in 0.1V*/
		short ECANA_INDEX_UVW;		           /* 26 -in 0.1V*/
		short ECANA_INDEX_UWV;               /* 27 -in 0.1V*/
		short ECANA_INDEX_AnIZK;             /* 28 *******/
		short ECANA_INDEX_AnSPD;             /* 29 *******/
 
 		short ECANA_INDEX_AnSPDRef;		       /* 30  ******/
		short  ECANA_INDEX_FBK_DI;           /* 31 信号反馈通道*/
		short ECANA_INDEX_CMD_DI2;           /* 32 speaker & ZZQ第二通道 & WC KM Brake******/
		short ECANA_INDEX_AnZzqCh2;          /* 33 ******/
		short ECANA_INDEX_I34;		           /* 34 */
		short ECANA_INDEX_ZC_DICAN;          /* 35 */
		short ECANA_INDEX_ZC_DOCAN;          /* 36 */
		short ECANA_INDEX_WC_DICANB;         /* 37 ******/
		short ECANA_INDEX_WC_DOCANB;         /* 38 ******/
		short ECANA_INDEX_ZZ_DICANB;         /* 39 ******/
 
 		short ECANA_INDEX_ZZ_DOCANB;         /* 40 ******/
		short ECANA_INDEX_POS_ACT1;		       /* 41 */
		short ECANA_INDEX_POS_ACT2;          /* 42 */
		short ECANA_INDEX_AnZzqCh;           /* 43 ******/
		short ECANA_INDEX_ModnNum;		       /* 44 */
		short ECANA_INDEX_AnPOS;		         /* 45 ******/
		short ECANA_INDEX_pid_posErr;		     /* 46 */
		short ECANA_INDEX_IamWho;            /* 47 */


		short ECANA_INDEX_Send_Or_Recv;          /* 48 */
		
		short ECANA_AfterINDEX_ecanRxOkBit;      /* 49 */
		short ECANA_AfterINDEX_ecanRxOkCounting; /* 50 */
		short ECANA_AfterINDEX_pid1_pos_Ref;     /* 51 */

		short ECANA_AfterINDEX_DX1;              /* 52 */		
		short ECANA_AfterINDEX_DX2;              /* 53 */	
		short ECANA_AfterINDEX_DX3;              /* 54 */	
		short ECANA_AfterINDEX_DX4;              /* 55 */	
		short ECANA_AfterINDEX_DX5;              /* 56 */	
		short ECANA_AfterINDEX_DX6;              /* 57 */	
		short ECANA_AfterINDEX_DX7;              /* 58 */	
		short ECANA_AfterINDEX_DX8;              /* 59 */	
		short ECANA_AfterINDEX_DX9;              /* 60 */	
		short ECANA_AfterINDEX_DX10;             /* 61 */																					

		short ECANA_AfterINDEX_FlowNum;          /* 62 */
		short ECANA_AfterINDEX_End;              /* 63 */
	   
	}DSP_Send_str;

   byte DSP_Send_Buff128[128];
   
}_DSP_Send_Snet;

//@-DSP数据发送
_DSP_Send_Snet Snet_ESP32_Send;


// REPLACE WITH YOUR RECEIVER MAC Address
// uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};  
uint8_t broadcastAddress[] = {0x8C, 0xAA, 0xB5, 0x82, 0xEA, 0x58};    //TTGO_2020 白色

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
  char a[32];
  int b;
  float c;
  String d;
  bool e;
} struct_message;

// Create a struct_message called myData
struct_message send_Data;
struct_message recv_Data;


// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress fy_ip(172, 25, 9, 20); //FY
IPAddress xh_ip(172, 25, 9, 30);  //XH
IPAddress local_ip(172,25,9,61);

IPAddress ip_m(230, 1, 2, 3); 
short m_Port = 2300;      // local port to listen on

short local_fy_Port = 23600;      // local port to listen on
short local_xh_Port = 23602;      // local port to listen on
short fyPort = 23200;      // local port to listen on
short xhPort = 23302;      // local port to listen on
int count = 0;
int count_temp = 0;

// buffers for receiving and sending data
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];  // buffer to hold incoming packet,
byte  ReplyBuffer[128];        // a string to send back

// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP Udp_fy;
EthernetUDP Udp_xh;
EthernetUDP Udp_m;

long Recv_Packet_Count = 0;

int data = 0;

bool Ethernet_Connect_Flag = false;

//-----------------------ESP NOW---------------------------------
// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  // Serial.print("\r\nLast Packet Send Status:\t");
  // Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&recv_Data, incomingData, sizeof(recv_Data));
//   Serial.print("Bytes received: ");
//   Serial.println(len);
//   Serial.print("Char: ");
//   Serial.println(send_Data.a);
//   Serial.print("Int: ");
//   Serial.println(send_Data.b);
//   Serial.print("Float: ");
//   Serial.println(send_Data.c);
//   Serial.print("String: ");
//   Serial.println(send_Data.d);
//   Serial.print("Bool: ");
//   Serial.println(send_Data.e);
//   Serial.println();
}
//--------------------------------------------------------
 
void setup() {
  // Init Serial Monitor
  Serial.begin(115200);

  for(int i=0; i<128; i++)
  {
    // ReplyBuffer[i] = 0x00;
    Snet_ESP32_Send.DSP_Send_Buff128[i] = 0x00;
  }
  // ReplyBuffer[96] = 0x54;
  // Snet_ESP32_Send.DSP_Send_Buff128[96] = 0x54;
  Snet_ESP32_Send.DSP_Send_str.ECANA_INDEX_Send_Or_Recv = 0x0054;  //@-0x1154实际抓包结果为0x5411-低位在前
  Snet_ESP32_Send.DSP_Send_str.ECANA_INDEX_PSWORD = 0x0453;
  Snet_ESP32_Send.DSP_Send_str.ECANA_INDEX_CMD_DI = 0x0100;        //@-高字节模式  低字节运行控制及方向
  Snet_ESP32_Send.DSP_Send_str.ECANA_INDEX_AnSPD = 0x000A;         //@-速度为0.1% 即：0.1 * 100 
  Snet_ESP32_Send.DSP_Send_str.ECANA_INDEX_AnPOS = 0x000A;         //@-速度为0.1% 即：0.1 * 100

  // Snet_ESP32_Send.DSP_Send_strECANA_INDEX_CMD_DI2 = 0x0038;        //@-在给出运行方向时给出

  pinMode(2, OUTPUT);

  #ifdef ESP32_Servo
  myservo.setPeriodHertz(50);    // standard 50 hz servo
  myservo.attach(servoPin, 500, 2500); // attaches the servo on pin 18 to the servo object
  #endif

  #if 1
  Ethernet.init(5);

  // start the Ethernet
  Ethernet.begin(mac, local_ip);

  // Check for Ethernet hardware present
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
    // start UDP
    Udp_fy.begin(local_fy_Port);
    Udp_xh.begin(local_xh_Port);
  }
  #endif
 
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  // Serial.println(WiFi.macAddress());  //84:0D:8E:0B:B2:54


  // Init ESP-NOW
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
  
  // Register peer
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

  #ifdef ESP32_Servo
  myservo.write(180);
  #endif

}
 


void loop() {

  count =  count + 1;
  count_temp =  count_temp + 1;

  #ifdef ESP32_Servo
  myservo.write(recv_Data.b);
  #endif

  // if (result == ESP_OK) {
  //   Serial.println("Sent with success");
  // }
  // else {
  //   Serial.println("Error sending the data");
  // }

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

  #if 1
  if(Ethernet_Connect_Flag == true)
  {
    // if there's data available, read a packet
    int packetSize = Udp_fy.parsePacket();
    // int packetSize = Udp_m.parsePacket();
    if (packetSize) {

      Recv_Packet_Count = Recv_Packet_Count + 1;

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
      // Udp_fy.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
      // Serial.println("Contents:");
      // Serial.println(packetBuffer);

      // myservo.write(packetBuffer[0]);

      // send a reply to the IP address and port that sent us the packet we received
  //    Udp_fy.beginPacket(Udp_fy.remoteIP(), Udp_fy.remotePort());
  //    Udp_fy.write(ReplyBuffer);
  //    Udp_fy.endPacket();
    }
  }
  #endif


  if(count > 10)
  {
    count = 0;

    #if 1
    if(Ethernet_Connect_Flag == true)
    {
      // send a reply to the IP address and port that sent us the packet we received
      Udp_fy.beginPacket(fy_ip, fyPort);
      // Udp_fy.write(ReplyBuffer);  //write str
      // Udp_fy.write(ReplyBuffer,128);  //write byte
      Udp_fy.write(Snet_ESP32_Send.DSP_Send_Buff128,128);  //write byte
      Udp_fy.endPacket();  


      // Udp_xh.beginPacket(xh_ip, xhPort);
      // // Udp_fy.write(ReplyBuffer);  //write str
      // Udp_xh.write(ReplyBuffer,128);  //write byte
      // Udp_xh.endPacket(); 
    }
    #endif


    // Set values to send
    strcpy(send_Data.a, "THIS IS A CHAR");
  // send_Data.b = random(1,20);
    data = data + 1;
    if(data > 500)
    data = 0;
    send_Data.b = data;

    send_Data.c = 1.2;
    send_Data.d = "Hello";
    send_Data.e = false;

    // Send message via ESP-NOW
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &send_Data, sizeof(send_Data));
  }

  if(count_temp > 200)
  {
    count_temp = 0;

    Serial.print("Recv:");
    Serial.println(Recv_Packet_Count);
  }
  
  delay(5);
}
