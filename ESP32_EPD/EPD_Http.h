#ifndef _EPD_HTTP_H_
#define _EPD_HTTP_H_

#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <I2C_BM8563.h>
 
#include <Wire.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include "EPD4IN_Device.h" 
#include "time.h"
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <esp_wifi.h>
 //////////////////////////////////////////////////////////
int Http_Post(String webp, String sendjson);


struct EPD_Device_Struct
{ 
  char devsn[16];           //addap时更新  获取的MAC地址
  
  int is_init;              //暂时未使用
  char token_str[128];      //token字符串，reg后填写
  
  bool weather_result;      //天气预报
  char Weather_City[16];    //
  char Weather_Tmp_High[4];
  char Weather_Tmp_Low[4];
  char Weather_Tmp_Real[4];
  char Weather_Type[16];
  int  Weather_TypeCode;
  char Weather_hum[4];
  char Weather_winddir[16];
  char Weather_windpwr[8];
  char Weather_uptime[8];


  
  bool Todo_result;         //
  int  Todo_count;
  int  Todo_point;   //当前显示位置
  char Todo_New_List[40][64];//todo或者新闻的列表
  char Todo_Uptime[16];

  
  char Msg_body[128];
  int  Msg_code;
  bool Msg_Read;
  
  bool Calendar_result;
  char Calendar_animalsYear[8];
  char Calendar_lunarYear[12];
  char Calendar_suit[256];
  char Calendar_avoid[256];
  char Calendar_lunar[16];
  char Calendar_holiday[16];
};

bool addap(void);

void setUpHttpClient();
bool AutoConfig();
int Auto_Web_Config(void);
void wifi_Config();
void RestoreWifi();

int WebHandle(void);

int Http_Set_Rtc(void);
int Http_Get_Token(void);
int Http_Get_RealWeather(void);
int Http_Get_Calendar(void);
uint16_t  NewGetRtcMinute(void);
uint16_t  NewGetRtcHour(void);
int Http_Get_TodoList(void);
int Http_Post(String webp, String sendjson);
int GetNews(const char **news_str);
#endif
