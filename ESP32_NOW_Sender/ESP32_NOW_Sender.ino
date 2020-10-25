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

#define ESP32_Servo

#ifdef ESP32_Servo
Servo myservo;
// Recommended PWM GPIO pins on the ESP32 include 2,4,12-19,21-23,25-27,32-33 
int servoPin = 13;
#endif


typedef union	  /* CMD_DI数据结构*/
{
	struct CMD_DI_str
	{
		short Reset:1;		         /* 0 */
		short Fwd:1;		         /* 1 */
		short Rev:1;		         /* 2 */
		short BigJumpMod:1;		     /* 3 */
		short LowSpdMod:1;		     /* 4 */
		short Ready:1;		         /* 5 */
		short Run:1;		         /* 6 */
		short IzkMod:1;		         /* 7 */
		
		short SpdMod:1;		         /* 8 */
		short Exit:1;		         /* 9 */
		short ResetErr:1;		     /* 10 */
		short SelfTuneMod:1;		 /* 11 */
		short FirstTurnMod:1;		 /* 12 */
		short ZHY_Mode:1;		     /* 13 */
		short ManBrakeMod:1;		 /* 14 */
		short PosMod:1;		         /* 15  sin*/
		
	}CMD_DI_str;

   byte CMD_DI_Buff2[2];
   
}_DSP_CMD_DI_Snet;

typedef union	  /* DSP Data数据结构*/
{
	struct DSP_Data_str
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
		_DSP_CMD_DI_Snet ECANA_INDEX_CMD_DI;            /* 12 ******/
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
		short ECANA_INDEX_FBK_DI;            /* 31 信号反馈通道*/
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
	   
	}DSP_Data_str;

   byte DSP_Data_Buff128[128];
   
}_DSP_Data_Snet;

//@-DSP数据发送
_DSP_Data_Snet Snet_ESP32_Send;
_DSP_Data_Snet Snet_ESP32_Recv;


// REPLACE WITH YOUR RECEIVER MAC Address
// uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};  
uint8_t broadcastAddress[] = {0x8C, 0xAA, 0xB5, 0x82, 0xEA, 0x58};    //TTGO_2020 白色
// uint8_t broadcastAddress[] = {0x10, 0x52, 0x1C, 0x65, 0x42, 0x84};    //TTGO_2020 黑色

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
  char a[32];
  int b;
  float c;
  String d;
  bool e;

  bool DSP_Reset;
  bool DSP_Run;
  int  DSP_Dir;
  int  DSP_Speed;
} struct_message;

// Create a struct_message called myData
struct_message send_Data;
struct_message recv_Data;


byte TestData[128] = {
0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,
0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,
0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,
0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,
0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,
0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,
0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,
0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x54 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,
0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,
0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x04 ,0x00 ,0x31 ,0x00
};

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  // 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
  0x00, 0xE0, 0x4C, 0xA0, 0x7E, 0x61
};
IPAddress fy_ip(172, 25, 9, 20); //FY
IPAddress xh_ip(172, 25, 9, 18);  //XH
IPAddress local_ip(172,25,9,61);
IPAddress ip_m(230, 1, 2, 3); 
short m_Port = 2300;      // local port to listen on

short local_fy_Port = 23600;      // local port to listen on
short local_xh_Port = 23602;      // local port to listen on
short fyPort = 23200;      // local port to listen on
short xhPort = 23302;      // local port to listen on
int count = 0;
int count1 = 0;
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

int ESP32_Send_Count  = 0;


bool DSP_Reset_Copy = false;
int  DSP_Dir_Copy = 1;
bool DSP_Run_Copy = false;
int DSP_Speed_Copy = 0;



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

  //@-Dsp Reset
  if(DSP_Reset_Copy != recv_Data.DSP_Reset)
  {
    DSP_Reset_Copy = recv_Data.DSP_Reset;
    if(recv_Data.DSP_Reset == true)
    {
      // Snet_ESP32_Send.DSP_Data_str.ECANA_INDEX_CMD_DI = 0x0400;
      Snet_ESP32_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.SpdMod = 0;
      Snet_ESP32_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Fwd = 0;
      Snet_ESP32_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Rev = 0;
      Snet_ESP32_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Ready = 0;
      Snet_ESP32_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Run = 0;
      Snet_ESP32_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Exit = 0;
      Snet_ESP32_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.ResetErr = 1; 
      Snet_ESP32_Send.DSP_Data_str.ECANA_INDEX_CMD_DI2 = 0x0400;
    }
    else if(recv_Data.DSP_Reset == false)
    {
      // Snet_ESP32_Send.DSP_Data_str.ECANA_INDEX_CMD_DI = 0x0100;
      Snet_ESP32_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.SpdMod = 0;
      Snet_ESP32_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Fwd = 0;
      Snet_ESP32_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Rev = 0;
      Snet_ESP32_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Ready = 0;
      Snet_ESP32_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Run = 0;
      Snet_ESP32_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Exit = 0;
      Snet_ESP32_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.ResetErr = 0;
      Snet_ESP32_Send.DSP_Data_str.ECANA_INDEX_CMD_DI2 = 0x0000;
    }
  }

  //@-运行方向-正
  if(DSP_Dir_Copy != recv_Data.DSP_Dir)
  {
    DSP_Dir_Copy = recv_Data.DSP_Dir;
    if(recv_Data.DSP_Dir == 1)
    {
      Snet_ESP32_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Ready = 1;
      Snet_ESP32_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.SpdMod = 1;
      Snet_ESP32_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Fwd = 1;
      Snet_ESP32_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Rev = 0;

      Snet_ESP32_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Exit = 0;
    }
    else if(recv_Data.DSP_Dir == 2)
    {
      Snet_ESP32_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Ready = 1;
      Snet_ESP32_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.SpdMod = 1;
      Snet_ESP32_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Fwd = 0;
      Snet_ESP32_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Rev = 1;

      Snet_ESP32_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Exit = 0;
    }
  }

  //@-运行
  if(DSP_Run_Copy != recv_Data.DSP_Run)
  {
    DSP_Run_Copy = recv_Data.DSP_Run;
    if(recv_Data.DSP_Run == true)
    {
      Snet_ESP32_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Ready = 1;
      Snet_ESP32_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Run = 1;
      Snet_ESP32_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Exit = 0;
    }
    else if(recv_Data.DSP_Run == false)
    {
      Snet_ESP32_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Ready = 0;
      Snet_ESP32_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Run = 0;
      Snet_ESP32_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Exit = 1;
    }
  }

  //@-速度
  if(DSP_Speed_Copy != recv_Data.DSP_Speed)
  {
    DSP_Speed_Copy = recv_Data.DSP_Speed;

    Snet_ESP32_Send.DSP_Data_str.ECANA_INDEX_AnSPD = DSP_Speed_Copy;         //@-速度为0.1% 即：0.1 * 100 
    Snet_ESP32_Send.DSP_Data_str.ECANA_INDEX_AnPOS = DSP_Speed_Copy;         //@-速度为0.1% 即：0.1 * 100
  }


}
//--------------------------------------------------------

//@-系统配置 
void setup() {
  // Init Serial Monitor
  Serial.begin(115200);


  //@-初始值
  DSP_Reset_Copy = false;
  DSP_Dir_Copy = 1;
  DSP_Run_Copy = false;

  for(int i=0; i<128; i++)
  {
    // ReplyBuffer[i] = 0x00;
    Snet_ESP32_Send.DSP_Data_Buff128[i] = 0x00;
  }
  // ReplyBuffer[96] = 0x54;
  // Snet_ESP32_Send.DSP_Data_Buff128[96] = 0x54;
  Snet_ESP32_Send.DSP_Data_str.ECANA_INDEX_Send_Or_Recv = 0x0054;  //@-0x1154实际抓包结果为0x5411-低位在前
  Snet_ESP32_Send.DSP_Data_str.ECANA_INDEX_PSWORD = 0x0453;

  //-------------------------Low Byte----------------------------------
		// UWORD Reset:1;		         /* 0 */
		// UWORD Fwd:1;		         /* 1 */
		// UWORD Rev:1;		         /* 2 */
		// UWORD BigJumpMod:1;		     /* 3 */
		// UWORD LowSpdMod:1;		     /* 4 */
		// UWORD Ready:1;		         /* 5 */
		// UWORD Run:1;		         /* 6 */
		// UWORD IzkMod:1;		         /* 7 */
	//-------------------------High Byte----------------------------------
		// UWORD SpdMod:1;		         /* 8 */
		// UWORD Exit:1;		         /* 9 */
		// UWORD ResetErr:1;		     /* 10 */
		// UWORD SelfTuneMod:1;		 /* 11 */
		// UWORD FirstTurnMod:1;		 /* 12 */
		// UWORD ZHY_Mode:1;		     /* 13 */
		// UWORD ManBrakeMod:1;		 /* 14 */
		// UWORD PosMod:1;		         /* 15  sin*/
  Snet_ESP32_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.SpdMod = 1;        //@-高字节模式  低字节运行控制及方向 低字节-0x22-Fwd 
  Snet_ESP32_Send.DSP_Data_str.ECANA_INDEX_AnSPD = 0x0064;         //@-速度为0.1% 即：0.1 * 100 
  Snet_ESP32_Send.DSP_Data_str.ECANA_INDEX_AnPOS = 0x0064;         //@-速度为0.1% 即：0.1 * 100

  Snet_ESP32_Send.DSP_Data_str.ECANA_AfterINDEX_End = 0x0031;

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
    int packetSize = Udp_fy.parsePacket();
    // int packetSize = Udp_m.parsePacket();
    if (packetSize == 128) 
    {

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
      Udp_fy.read(Snet_ESP32_Recv.DSP_Data_Buff128, 128);

      // Serial.println("Contents:");
      // Serial.println(packetBuffer);

      // myservo.write(packetBuffer[0]);

      // send a reply to the IP address and port that sent us the packet we received
  //    Udp_fy.beginPacket(Udp_fy.remoteIP(), Udp_fy.remotePort());
  //    Udp_fy.write(ReplyBuffer);
  //    Udp_fy.endPacket();
    }
  }

  //@-DSP以太网发送
  if(count > 1)
  {
    count = 0;

    if(Ethernet_Connect_Flag == true)
    {
      ESP32_Send_Count = ESP32_Send_Count + 1;

      if(ESP32_Send_Count > 65535)
      ESP32_Send_Count = 0;

      //@-接收到DSP数据的流水号
      Snet_ESP32_Send.DSP_Data_str.ECANA_INDEX_FLOWNO_453 = Snet_ESP32_Recv.DSP_Data_str.ECANA_INDEX_FLOWNO_453;
      //@-发送DSP数据流水号
      Snet_ESP32_Send.DSP_Data_str.ECANA_INDEX_FLOWNO_CZP = ESP32_Send_Count;

      // send a reply to the IP address and port that sent us the packet we received
      Udp_fy.beginPacket(fy_ip, fyPort);
      // Udp_fy.write(ReplyBuffer);  //write str
      // Udp_fy.write(TestData,128);  //write byte
      Udp_fy.write(Snet_ESP32_Send.DSP_Data_Buff128,128);  //write byte
      Udp_fy.endPacket();  

      #if xh_send
      Udp_xh.beginPacket(xh_ip, xhPort);
      // Udp_fy.write(ReplyBuffer);  //write str
      Udp_xh.write(Snet_ESP32_Send.DSP_Data_Buff128,128);  //write byte
      Udp_xh.endPacket(); 
      #endif
    }
  }

  //@-ESP NOW定时发送
  if(count1 > 40)
  {
    count1 = 0;

  //   // Set values to send
  //   strcpy(send_Data.a, "THIS IS A CHAR");
  // // send_Data.b = random(1,20);
  //   data = data + 1;
  //   if(data > 500)
  //   data = 0;
  //   send_Data.b = data;
  //   send_Data.c = 1.2;
  //   send_Data.d = "Hello";
  //   send_Data.e = false;

    //@-转发接收到的DSP数据 Send message via ESP-NOW 
    // esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &send_Data, sizeof(send_Data));
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &Snet_ESP32_Recv.DSP_Data_Buff128, sizeof(Snet_ESP32_Recv.DSP_Data_Buff128));
  }

  //@-串口数据出数据
  if(count_temp > 400)
  {
    count_temp = 0;

    // Snet_ESP32_Recv.DSP_Data_str.ECANA_INDEX_ERROR1
    // Snet_ESP32_Recv.DSP_Data_str.ECANA_INDEX_FLOWNO_453

    Serial.print("Recv:");
    Serial.println(Recv_Packet_Count);

    // Serial.print("Error:");
    // Serial.println(Snet_ESP32_Recv.DSP_Data_str.ECANA_INDEX_ERROR1);
  }
  
  //@-系统延时
  delay(2);   //@-1:500/s  2:250/s
}
