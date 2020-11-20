/*
  Jack Din
  该项目为DC人机网络远程控制接收端，使用ESP NOW技术为无线通讯载体。
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

//@3-DSP网络CMD信息位定义
typedef union	  /* CMD_DI数据结构*/
{
	struct CMD_DI_str
	{
		short Reset:1;		         /* 0 */    /*Low:Reset  --->  High:IzkMod   高字节*/
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

//@4-DSP网络收发报文信息位定义
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


typedef union	  /* DSP Data数据结构*/
{
	struct DSP_Data_str
	{

        short FY_ECANA_INDEX_FLOWNO_CZP;     /* 0 */
        short FY_ECANA_INDEX_ERROR1;         /* 1 */
        short FY_ECANA_INDEX_POS_ACT1;       /* 2 */
        short FY_ECANA_INDEX_UU_A1;          /* 3 */

        short FY2_ECANA_INDEX_FLOWNO_CZP;    /* 4 */
        short FY2_ECANA_INDEX_ERROR1;        /* 5 */
        short FY2_ECANA_INDEX_POS_ACT1;      /* 6 */
        short FY2_ECANA_INDEX_UU_A1;         /* 7 */

        short XH_ECANA_INDEX_FLOWNO_CZP;     /* 8 */
        short XH_ECANA_INDEX_ERROR1;         /* 9 */
        short XH_ECANA_INDEX_POS_ACT1;       /* 10 */
        short XH_ECANA_INDEX_UU_A1;          /* 11 */

        short XH2_ECANA_INDEX_FLOWNO_CZP;    /* 12 */
        short XH2_ECANA_INDEX_ERROR1;        /* 13 */
        short XH2_ECANA_INDEX_POS_ACT1;      /* 14 */
        short XH2_ECANA_INDEX_UU_A1;         /* 15 */

//---------------------------------------------------------------------------

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
   
}_DSP_Data_Now;

//@5-DSP发送数据
_DSP_Data_Snet Snet_ESP32_fy_Send;
_DSP_Data_Snet Snet_ESP32_fy2_Send;
_DSP_Data_Snet Snet_ESP32_xh_Send;
_DSP_Data_Snet Snet_ESP32_xh2_Send;

//@6-DSP接收数据
_DSP_Data_Snet Snet_ESP32_fy_Recv;
_DSP_Data_Snet Snet_ESP32_fy2_Recv;
_DSP_Data_Snet Snet_ESP32_xh_Recv;
_DSP_Data_Snet Snet_ESP32_xh2_Recv;

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
_DSP_Data_Now  send_Data;
struct_message recv_Data;

//@10-本地网络MAC地址
byte mac[] = {
  // 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
  0x00, 0xE0, 0x4C, 0xA0, 0x7E, 0x61
};

//@11-DSP网络各节点IP
IPAddress fy_ip(172, 25, 9, 20);  //FY
IPAddress fy2_ip(172, 25, 9, 22); //FY2
IPAddress xh_ip(172, 25, 9, 30);  //XH
IPAddress xh2_ip(172, 25, 9, 32); //XH2
IPAddress local_ip(172,25,9,61);

//@12-DSP网络各节点Port
short local_fy_Port =  23600;       // local port to listen on
short local_fy2_Port = 23603;       // local port to listen on
short local_xh_Port =  23602;       // local port to listen on
short local_xh2_Port = 23604;       // local port to listen on
short fyPort =  23200;       // local port to listen on
short fy2Port = 23203;       // local port to listen on
short xhPort =  23302;       // local port to listen on
short xh2Port = 23304;       // local port to listen on

//@13-DSP网络组播地址及port
IPAddress ip_m(230, 1, 2, 3); 
short m_Port = 2300;      

//@14-测试用数据
int count = 0;
int count1 = 0;
int count_temp = 0;

//@15-测试用数据
// buffers for receiving and sending data
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];  // buffer to hold incoming packet,
byte  ReplyBuffer[128];        // a string to send back

//@16-UDP报文socket
EthernetUDP Udp_fy;
EthernetUDP Udp_fy2;
EthernetUDP Udp_xh;
EthernetUDP Udp_xh2;
EthernetUDP Udp_m;

//@17-DSP网络各节点数据报文接收计数
long fy_Recv_Packet_Count = 0;
long fy2_Recv_Packet_Count = 0;
long xh_Recv_Packet_Count = 0;
long xh2_Recv_Packet_Count = 0;

//@18-测试数据
int data = 0;

//@19-网络物理连接标志
bool Ethernet_Connect_Flag = false;

//@20-网络发送计数
int ESP32_Send_Count  = 0;

//@21-DSP控制参数
bool FY_DSP_Reset_Copy = false;
int  FY_DSP_Dir_Copy = 0;
bool FY_DSP_Run_Copy = false;
int FY_DSP_Speed_Copy = 0;

bool XH_DSP_Reset_Copy = false;
int  XH_DSP_Dir_Copy = 0;
bool XH_DSP_Run_Copy = false;
int XH_DSP_Speed_Copy = 0;



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
  if(FY_DSP_Reset_Copy != recv_Data.FY_DSP_Reset)
  {
    FY_DSP_Reset_Copy = recv_Data.FY_DSP_Reset;
    if(recv_Data.FY_DSP_Reset == true)
    {
      // Snet_ESP32_fy_Send.DSP_Data_str.ECANA_INDEX_CMD_DI = 0x0400;
      Snet_ESP32_fy_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Fwd = 0;
      Snet_ESP32_fy_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Rev = 0;
      Snet_ESP32_fy_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Ready = 0;
      Snet_ESP32_fy_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Run = 0;
      Snet_ESP32_fy_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Exit = 0;
      Snet_ESP32_fy_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.ResetErr = 1; 
      Snet_ESP32_fy_Send.DSP_Data_str.ECANA_INDEX_CMD_DI2 = 0x0400;

      // Snet_ESP32_fy2_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.SpdMod = 0;
      Snet_ESP32_fy2_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Fwd = 0;
      Snet_ESP32_fy2_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Rev = 0;
      Snet_ESP32_fy2_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Ready = 0;
      Snet_ESP32_fy2_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Run = 0;
      Snet_ESP32_fy2_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Exit = 0;
      Snet_ESP32_fy2_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.ResetErr = 1; 
      Snet_ESP32_fy2_Send.DSP_Data_str.ECANA_INDEX_CMD_DI2 = 0x0400;
    }
    else if(recv_Data.FY_DSP_Reset == false)
    {
      // Snet_ESP32_fy_Send.DSP_Data_str.ECANA_INDEX_CMD_DI = 0x0100;
      Snet_ESP32_fy_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Fwd = 0;
      Snet_ESP32_fy_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Rev = 0;
      Snet_ESP32_fy_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Ready = 0;
      Snet_ESP32_fy_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Run = 0;
      Snet_ESP32_fy_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Exit = 0;
      Snet_ESP32_fy_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.ResetErr = 0; 
      Snet_ESP32_fy_Send.DSP_Data_str.ECANA_INDEX_CMD_DI2 = 0x0000;

      // Snet_ESP32_fy2_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.SpdMod = 0;
      Snet_ESP32_fy2_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Fwd = 0;
      Snet_ESP32_fy2_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Rev = 0;
      Snet_ESP32_fy2_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Ready = 0;
      Snet_ESP32_fy2_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Run = 0;
      Snet_ESP32_fy2_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Exit = 0;
      Snet_ESP32_fy2_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.ResetErr = 0;
      Snet_ESP32_fy2_Send.DSP_Data_str.ECANA_INDEX_CMD_DI2 = 0x0000;
    }
  }

  //@-运行方向-正
  if(FY_DSP_Dir_Copy != recv_Data.FY_DSP_Dir)
  {
    FY_DSP_Dir_Copy = recv_Data.FY_DSP_Dir;
    if(recv_Data.FY_DSP_Dir == 1)
    {
      Snet_ESP32_fy_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Ready = 1;
      Snet_ESP32_fy_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Fwd = 1;
      Snet_ESP32_fy_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Rev = 0;
      Snet_ESP32_fy_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Exit = 0;

      Snet_ESP32_fy2_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Ready = 1;
      Snet_ESP32_fy2_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Fwd = 1;
      Snet_ESP32_fy2_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Rev = 0;
      Snet_ESP32_fy2_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Exit = 0;
    }
    else if(recv_Data.FY_DSP_Dir == 2)
    {
      Snet_ESP32_fy_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Ready = 1;
      Snet_ESP32_fy_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Fwd = 0;
      Snet_ESP32_fy_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Rev = 1;
      Snet_ESP32_fy_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Exit = 0;

      Snet_ESP32_fy2_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Ready = 1;
      Snet_ESP32_fy2_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Fwd = 0;
      Snet_ESP32_fy2_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Rev = 1;
      Snet_ESP32_fy2_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Exit = 0;
    }
    else if(recv_Data.FY_DSP_Dir == 3)
    {
      Snet_ESP32_fy_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Ready = 1;
      Snet_ESP32_fy_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Fwd = 0;
      Snet_ESP32_fy_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Rev = 0;
      Snet_ESP32_fy_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Exit = 0;

      Snet_ESP32_fy2_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Ready = 1;
      Snet_ESP32_fy2_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Fwd = 0;
      Snet_ESP32_fy2_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Rev = 0;
      Snet_ESP32_fy2_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Exit = 0;
    }
  }

  //@-运行
  if(FY_DSP_Run_Copy != recv_Data.FY_DSP_Run)
  {
    FY_DSP_Run_Copy = recv_Data.FY_DSP_Run;
    if(recv_Data.FY_DSP_Run == true)
    {
      Snet_ESP32_fy_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Ready = 1;
      Snet_ESP32_fy_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Run = 1;
      Snet_ESP32_fy_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Exit = 0;

      Snet_ESP32_fy2_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Ready = 1;
      Snet_ESP32_fy2_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Run = 1;
      Snet_ESP32_fy2_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Exit = 0;
    }
    else if(recv_Data.FY_DSP_Run == false)
    {
      Snet_ESP32_fy_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Ready = 0;
      Snet_ESP32_fy_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Run = 0;
      Snet_ESP32_fy_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Exit = 1;

      Snet_ESP32_fy2_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Ready = 0;
      Snet_ESP32_fy2_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Run = 0;
      Snet_ESP32_fy2_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Exit = 1;
    }
  }

  //@-速度
  if(FY_DSP_Speed_Copy != recv_Data.FY_DSP_Speed)
  {
    FY_DSP_Speed_Copy = recv_Data.FY_DSP_Speed;

    Snet_ESP32_fy_Send.DSP_Data_str.ECANA_INDEX_AnIZK = 10;
    Snet_ESP32_fy_Send.DSP_Data_str.ECANA_INDEX_AnSPD = FY_DSP_Speed_Copy;         //@-速度为0.1% 即：0.1 * 100 
    Snet_ESP32_fy_Send.DSP_Data_str.ECANA_INDEX_AnPOS = FY_DSP_Speed_Copy;         //@-速度为0.1% 即：0.1 * 100

    Snet_ESP32_fy2_Send.DSP_Data_str.ECANA_INDEX_AnSPD = FY_DSP_Speed_Copy;         //@-速度为0.1% 即：0.1 * 100 
    Snet_ESP32_fy2_Send.DSP_Data_str.ECANA_INDEX_AnPOS = FY_DSP_Speed_Copy;         //@-速度为0.1% 即：0.1 * 100
  }
  //-----------------------------------------------------------------------------------------

  //@-Dsp Reset
  if(XH_DSP_Reset_Copy != recv_Data.XH_DSP_Reset)
  {
    XH_DSP_Reset_Copy = recv_Data.XH_DSP_Reset;
    if(recv_Data.XH_DSP_Reset == true)
    {
      // Snet_ESP32_xh_Send.DSP_Data_str.ECANA_INDEX_CMD_DI = 0x0400;
      Snet_ESP32_xh_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Fwd = 0;
      Snet_ESP32_xh_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Rev = 0;
      Snet_ESP32_xh_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Ready = 0;
      Snet_ESP32_xh_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Run = 0;
      Snet_ESP32_xh_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Exit = 0;
      Snet_ESP32_xh_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.ResetErr = 1; 
      Snet_ESP32_xh_Send.DSP_Data_str.ECANA_INDEX_CMD_DI2 = 0x0400;

      // Snet_ESP32_xh2_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.SpdMod = 0;
      Snet_ESP32_xh2_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Fwd = 0;
      Snet_ESP32_xh2_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Rev = 0;
      Snet_ESP32_xh2_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Ready = 0;
      Snet_ESP32_xh2_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Run = 0;
      Snet_ESP32_xh2_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Exit = 0;
      Snet_ESP32_xh2_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.ResetErr = 1; 
      Snet_ESP32_xh2_Send.DSP_Data_str.ECANA_INDEX_CMD_DI2 = 0x0400;
    }
    else if(recv_Data.XH_DSP_Reset == false)
    {
      // Snet_ESP32_xh_Send.DSP_Data_str.ECANA_INDEX_CMD_DI = 0x0100;
      Snet_ESP32_xh_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Fwd = 0;
      Snet_ESP32_xh_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Rev = 0;
      Snet_ESP32_xh_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Ready = 0;
      Snet_ESP32_xh_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Run = 0;
      Snet_ESP32_xh_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Exit = 0;
      Snet_ESP32_xh_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.ResetErr = 0;
      Snet_ESP32_xh_Send.DSP_Data_str.ECANA_INDEX_CMD_DI2 = 0x0000;

      // Snet_ESP32_xh2_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.SpdMod = 0;
      Snet_ESP32_xh2_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Fwd = 0;
      Snet_ESP32_xh2_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Rev = 0;
      Snet_ESP32_xh2_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Ready = 0;
      Snet_ESP32_xh2_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Run = 0;
      Snet_ESP32_xh2_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Exit = 0;
      Snet_ESP32_xh2_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.ResetErr = 0;
      Snet_ESP32_xh2_Send.DSP_Data_str.ECANA_INDEX_CMD_DI2 = 0x0000;
    }
  }

  //@-运行方向-正
  if(XH_DSP_Dir_Copy != recv_Data.XH_DSP_Dir)
  {
    XH_DSP_Dir_Copy = recv_Data.XH_DSP_Dir;
    if(recv_Data.XH_DSP_Dir == 1)
    {
      Snet_ESP32_xh_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Ready = 1;
      Snet_ESP32_xh_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Fwd = 1;
      Snet_ESP32_xh_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Rev = 0;
      Snet_ESP32_xh_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Exit = 0;

      Snet_ESP32_xh2_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Ready = 1;
      Snet_ESP32_xh2_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Fwd = 1;
      Snet_ESP32_xh2_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Rev = 0;
      Snet_ESP32_xh2_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Exit = 0;
    }
    else if(recv_Data.XH_DSP_Dir == 2)
    {
      Snet_ESP32_xh_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Ready = 1;
      Snet_ESP32_xh_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Fwd = 0;
      Snet_ESP32_xh_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Rev = 1;
      Snet_ESP32_xh_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Exit = 0;

      Snet_ESP32_xh2_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Ready = 1;
      Snet_ESP32_xh2_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Fwd = 0;
      Snet_ESP32_xh2_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Rev = 1;
      Snet_ESP32_xh2_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Exit = 0;
    }
    else if(recv_Data.XH_DSP_Dir == 3)
    {
      Snet_ESP32_xh_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Ready = 1;
      Snet_ESP32_xh_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Fwd = 0;
      Snet_ESP32_xh_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Rev = 0;
      Snet_ESP32_xh_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Exit = 0;

      Snet_ESP32_xh2_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Ready = 1;
      Snet_ESP32_xh2_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Fwd = 0;
      Snet_ESP32_xh2_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Rev = 0;
      Snet_ESP32_xh2_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Exit = 0;
    }
  }

  //@-运行
  if(XH_DSP_Run_Copy != recv_Data.XH_DSP_Run)
  {
    XH_DSP_Run_Copy = recv_Data.XH_DSP_Run;
    if(recv_Data.XH_DSP_Run == true)
    {
      Snet_ESP32_xh_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Ready = 1;
      Snet_ESP32_xh_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Run = 1;
      Snet_ESP32_xh_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Exit = 0;

      Snet_ESP32_xh2_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Ready = 1;
      Snet_ESP32_xh2_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Run = 1;
      Snet_ESP32_xh2_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Exit = 0;
    }
    else if(recv_Data.XH_DSP_Run == false)
    {
      Snet_ESP32_xh_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Ready = 0;
      Snet_ESP32_xh_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Run = 0;
      Snet_ESP32_xh_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Exit = 1;

      Snet_ESP32_xh2_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Ready = 0;
      Snet_ESP32_xh2_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Run = 0;
      Snet_ESP32_xh2_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.Exit = 1;
    }
  }

  //@-速度
  if(XH_DSP_Speed_Copy != recv_Data.XH_DSP_Speed)
  {
    XH_DSP_Speed_Copy = recv_Data.XH_DSP_Speed;

    Snet_ESP32_xh_Send.DSP_Data_str.ECANA_INDEX_AnIZK = 10;
    Snet_ESP32_xh_Send.DSP_Data_str.ECANA_INDEX_AnSPD = XH_DSP_Speed_Copy;         //@-速度为0.1% 即：0.1 * 100 
    Snet_ESP32_xh_Send.DSP_Data_str.ECANA_INDEX_AnPOS = XH_DSP_Speed_Copy;         //@-速度为0.1% 即：0.1 * 100

    Snet_ESP32_xh2_Send.DSP_Data_str.ECANA_INDEX_AnSPD = XH_DSP_Speed_Copy;         //@-速度为0.1% 即：0.1 * 100 
    Snet_ESP32_xh2_Send.DSP_Data_str.ECANA_INDEX_AnPOS = XH_DSP_Speed_Copy;         //@-速度为0.1% 即：0.1 * 100
  }


}
//--------------------------------------------------------
//@-系统配置 
void setup() {

  //@-初始化串口
  Serial.begin(115200);

  //@-初始值
  FY_DSP_Reset_Copy = false;
  FY_DSP_Dir_Copy = 1;
  FY_DSP_Run_Copy = false;

  XH_DSP_Reset_Copy = false;
  XH_DSP_Dir_Copy = 1;
  XH_DSP_Run_Copy = false;

  for(int i=0; i<128; i++)
  {
    Snet_ESP32_fy_Send.DSP_Data_Buff128[i] =  0x00;
    Snet_ESP32_fy2_Send.DSP_Data_Buff128[i] = 0x00;
    Snet_ESP32_xh_Send.DSP_Data_Buff128[i] =  0x00;
    Snet_ESP32_xh2_Send.DSP_Data_Buff128[i] = 0x00; 
  }

  Snet_ESP32_fy_Send.DSP_Data_str.ECANA_INDEX_Send_Or_Recv = 0x0054;   //@-0x1154实际抓包结果为0x5411-低位在前
  Snet_ESP32_fy_Send.DSP_Data_str.ECANA_INDEX_PSWORD = 0x0453;
  Snet_ESP32_fy_Send.DSP_Data_str.ECANA_INDEX_AnDRV = 556;
  Snet_ESP32_fy2_Send.DSP_Data_str.ECANA_INDEX_Send_Or_Recv = 0x0054;  //@-0x1154实际抓包结果为0x5411-低位在前
  Snet_ESP32_fy2_Send.DSP_Data_str.ECANA_INDEX_PSWORD = 0x0453;
  Snet_ESP32_fy2_Send.DSP_Data_str.ECANA_INDEX_AnDRV = 555;
  Snet_ESP32_fy2_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.IzkMod = 1;

  Snet_ESP32_xh_Send.DSP_Data_str.ECANA_INDEX_Send_Or_Recv = 0x0054;   //@-0x1154实际抓包结果为0x5411-低位在前
  Snet_ESP32_xh_Send.DSP_Data_str.ECANA_INDEX_PSWORD = 0x0453;
  Snet_ESP32_xh_Send.DSP_Data_str.ECANA_INDEX_AnDRV = 556;
  Snet_ESP32_xh2_Send.DSP_Data_str.ECANA_INDEX_Send_Or_Recv = 0x0054;  //@-0x1154实际抓包结果为0x5411-低位在前
  Snet_ESP32_xh2_Send.DSP_Data_str.ECANA_INDEX_PSWORD = 0x0453;
  Snet_ESP32_xh2_Send.DSP_Data_str.ECANA_INDEX_AnDRV = 555;
  Snet_ESP32_xh2_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.IzkMod = 1;

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
  Snet_ESP32_fy_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.SpdMod = 1;        //@-高字节模式  低字节运行控制及方向 低字节-0x22-Fwd 
  Snet_ESP32_fy_Send.DSP_Data_str.ECANA_INDEX_AnSPD = 0x0064;                      //@-速度为0.1% 即：0.1 * 100 
  Snet_ESP32_fy_Send.DSP_Data_str.ECANA_INDEX_AnPOS = 0x0064;                      //@-速度为0.1% 即：0.1 * 100
  // Snet_ESP32_fy2_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.SpdMod = 1;    //@-高字节模式  低字节运行控制及方向 低字节-0x22-Fwd 
  Snet_ESP32_fy2_Send.DSP_Data_str.ECANA_INDEX_AnSPD = 0x0064;                     //@-速度为0.1% 即：0.1 * 100 
  Snet_ESP32_fy2_Send.DSP_Data_str.ECANA_INDEX_AnPOS = 0x0064;                     //@-速度为0.1% 即：0.1 * 100

  Snet_ESP32_xh_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.SpdMod = 1;        //@-高字节模式  低字节运行控制及方向 低字节-0x22-Fwd 
  Snet_ESP32_xh_Send.DSP_Data_str.ECANA_INDEX_AnSPD = 0x0064;                      //@-速度为0.1% 即：0.1 * 100 
  Snet_ESP32_xh_Send.DSP_Data_str.ECANA_INDEX_AnPOS = 0x0064;                      //@-速度为0.1% 即：0.1 * 100
  // Snet_ESP32_xh2_Send.DSP_Data_str.ECANA_INDEX_CMD_DI.CMD_DI_str.SpdMod = 1;    //@-高字节模式  低字节运行控制及方向 低字节-0x22-Fwd 
  Snet_ESP32_xh2_Send.DSP_Data_str.ECANA_INDEX_AnSPD = 0x0064;                     //@-速度为0.1% 即：0.1 * 100 
  Snet_ESP32_xh2_Send.DSP_Data_str.ECANA_INDEX_AnPOS = 0x0064;                     //@-速度为0.1% 即：0.1 * 100

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
    Udp_fy.begin(local_fy_Port);
    Udp_fy2.begin(local_fy2_Port);

    Udp_xh.begin(local_xh_Port);
    Udp_xh2.begin(local_xh2_Port);
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
    int fy_packetSize = Udp_fy.parsePacket();
    int fy2_packetSize = Udp_fy2.parsePacket();
    int xh_packetSize = Udp_xh.parsePacket();
    int xh2_packetSize = Udp_xh2.parsePacket();
    // int packetSize = Udp_m.parsePacket();

    if(fy_packetSize == 128)
    {
      fy_Recv_Packet_Count = fy_Recv_Packet_Count + 1;

      // read the packet into packetBufffer
      Udp_fy.read(Snet_ESP32_fy_Recv.DSP_Data_Buff128, 128);
    }

    if(fy2_packetSize == 128)
    {
      fy2_Recv_Packet_Count = fy2_Recv_Packet_Count + 1;

      // read the packet into packetBufffer
      Udp_fy2.read(Snet_ESP32_fy2_Recv.DSP_Data_Buff128, 128);
    }

    if (xh_packetSize == 128) 
    {

      xh_Recv_Packet_Count = xh_Recv_Packet_Count + 1;

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
      Udp_xh.read(Snet_ESP32_xh_Recv.DSP_Data_Buff128, 128);

      // Serial.println("Contents:");
      // Serial.println(packetBuffer);

      // myservo.write(packetBuffer[0]);

      // send a reply to the IP address and port that sent us the packet we received
  //    Udp_fy.beginPacket(Udp_fy.remoteIP(), Udp_fy.remotePort());
  //    Udp_fy.write(ReplyBuffer);
  //    Udp_fy.endPacket();
    }

    if(xh2_packetSize == 128)
    {
      xh2_Recv_Packet_Count = xh2_Recv_Packet_Count + 1;

      // read the packet into packetBufffer
      Udp_xh2.read(Snet_ESP32_xh2_Recv.DSP_Data_Buff128, 128);
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

      //------------------------------------------FY---------------------------------------------------------------------
      //@-接收到DSP数据的流水号
      Snet_ESP32_fy_Send.DSP_Data_str.ECANA_INDEX_FLOWNO_453 = Snet_ESP32_fy_Recv.DSP_Data_str.ECANA_INDEX_FLOWNO_453;
      //@-发送DSP数据流水号
      Snet_ESP32_fy_Send.DSP_Data_str.ECANA_INDEX_FLOWNO_CZP = ESP32_Send_Count;
      //------------------------------------------FY2---------------------------------------------------------------------
      //@-接收到DSP数据的流水号
      Snet_ESP32_fy2_Send.DSP_Data_str.ECANA_INDEX_FLOWNO_453 = Snet_ESP32_fy2_Recv.DSP_Data_str.ECANA_INDEX_FLOWNO_453;
      //@-发送DSP数据流水号
      Snet_ESP32_fy2_Send.DSP_Data_str.ECANA_INDEX_FLOWNO_CZP = ESP32_Send_Count;

      //------------------------------------------XH---------------------------------------------------------------------
      //@-接收到DSP数据的流水号
      Snet_ESP32_xh_Send.DSP_Data_str.ECANA_INDEX_FLOWNO_453 = Snet_ESP32_xh_Recv.DSP_Data_str.ECANA_INDEX_FLOWNO_453;
      //@-发送DSP数据流水号
      Snet_ESP32_xh_Send.DSP_Data_str.ECANA_INDEX_FLOWNO_CZP = ESP32_Send_Count;
      //------------------------------------------XH2---------------------------------------------------------------------
      //@-接收到DSP数据的流水号
      Snet_ESP32_xh2_Send.DSP_Data_str.ECANA_INDEX_FLOWNO_453 = Snet_ESP32_xh2_Recv.DSP_Data_str.ECANA_INDEX_FLOWNO_453;
      //@-发送DSP数据流水号
      Snet_ESP32_xh2_Send.DSP_Data_str.ECANA_INDEX_FLOWNO_CZP = ESP32_Send_Count;


      //------------------------------------------FY---------------------------------------------------------------------
      Udp_fy.beginPacket(fy_ip, fyPort);
      Udp_fy.write(Snet_ESP32_fy_Send.DSP_Data_Buff128,128);  //write byte
      Udp_fy.endPacket(); 
      //------------------------------------------FY2---------------------------------------------------------------------
      Udp_fy2.beginPacket(fy2_ip, fy2Port);
      Udp_fy2.write(Snet_ESP32_fy2_Send.DSP_Data_Buff128,128);  //write byte
      Udp_fy2.endPacket(); 


      // //------------------------------------------XH---------------------------------------------------------------------
      // send a reply to the IP address and port that sent us the packet we received
      Udp_xh.beginPacket(xh_ip, xhPort);
      // Udp_fy.write(ReplyBuffer);  //write str
      // Udp_fy.write(TestData,128);  //write byte
      Udp_xh.write(Snet_ESP32_xh_Send.DSP_Data_Buff128,128);  //write byte
      Udp_xh.endPacket();  
      //------------------------------------------XH2---------------------------------------------------------------------
      Udp_xh2.beginPacket(xh2_ip, xh2Port);
      Udp_xh2.write(Snet_ESP32_xh2_Send.DSP_Data_Buff128,128);  //write byte
      Udp_xh2.endPacket(); 

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

    send_Data.DSP_Data_str.FY_ECANA_INDEX_FLOWNO_CZP = data;
    send_Data.DSP_Data_str.FY_ECANA_INDEX_ERROR1     = Snet_ESP32_fy_Recv.DSP_Data_str.ECANA_INDEX_ERROR1;
    send_Data.DSP_Data_str.FY_ECANA_INDEX_POS_ACT1   = Snet_ESP32_fy_Recv.DSP_Data_str.ECANA_INDEX_POS_ACT1;
    send_Data.DSP_Data_str.FY_ECANA_INDEX_UU_A1      = Snet_ESP32_fy_Recv.DSP_Data_str.ECANA_INDEX_UU_A1;

    send_Data.DSP_Data_str.FY2_ECANA_INDEX_FLOWNO_CZP = data;
    send_Data.DSP_Data_str.FY2_ECANA_INDEX_ERROR1     = Snet_ESP32_fy2_Recv.DSP_Data_str.ECANA_INDEX_ERROR1;
    send_Data.DSP_Data_str.FY2_ECANA_INDEX_POS_ACT1   = Snet_ESP32_fy2_Recv.DSP_Data_str.ECANA_INDEX_POS_ACT1;
    send_Data.DSP_Data_str.FY2_ECANA_INDEX_UU_A1      = Snet_ESP32_fy2_Recv.DSP_Data_str.ECANA_INDEX_UU_A1;

    send_Data.DSP_Data_str.XH_ECANA_INDEX_FLOWNO_CZP = data;
    send_Data.DSP_Data_str.XH_ECANA_INDEX_ERROR1     = Snet_ESP32_xh_Recv.DSP_Data_str.ECANA_INDEX_ERROR1;
    send_Data.DSP_Data_str.XH_ECANA_INDEX_POS_ACT1   = Snet_ESP32_xh_Recv.DSP_Data_str.ECANA_INDEX_POS_ACT1;
    send_Data.DSP_Data_str.XH_ECANA_INDEX_UU_A1      = Snet_ESP32_xh_Recv.DSP_Data_str.ECANA_INDEX_UU_A1;

    send_Data.DSP_Data_str.XH2_ECANA_INDEX_FLOWNO_CZP = data;
    send_Data.DSP_Data_str.XH2_ECANA_INDEX_ERROR1     = Snet_ESP32_xh2_Recv.DSP_Data_str.ECANA_INDEX_ERROR1;
    send_Data.DSP_Data_str.XH2_ECANA_INDEX_POS_ACT1   = Snet_ESP32_xh2_Recv.DSP_Data_str.ECANA_INDEX_POS_ACT1;
    send_Data.DSP_Data_str.XH2_ECANA_INDEX_UU_A1      = Snet_ESP32_xh2_Recv.DSP_Data_str.ECANA_INDEX_UU_A1;
    

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
    Serial.println(xh_Recv_Packet_Count);

    // Serial.print("Error:");
    // Serial.println(Snet_ESP32_xh_Recv.DSP_Data_str.ECANA_INDEX_ERROR1);
  }
  
  //@-系统延时
  delay(1);   //@-1:500/s  2:250/s
}
