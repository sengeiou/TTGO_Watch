
#include <SPIFFS.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <I2C_BM8563.h>
#include <WEMOS_SHT3X.h>


#include "EPD_Http.h"
#include "EPD4IN_Device.h"
#include "EPD4IN_driver.h"
#include "driver/rtc_io.h"


#include "DX_EPD_Test.h"


//20210520-该版本EPD无光感传感器
// #define XDOT  240
// #define YDOT  400

#define ESP32

#define Button0_PIN   0  
#define Button1_PIN  27  
#define Button2_PIN  26
#define BAT_EN_PIN   23
#define BAT_V_PIN    35
#define BAT_AD_Solution 12




EPD4INC epd_drv_dx;
//@-创建RTC
I2C_BM8563 rtc_dx(I2C_BM8563_DEFAULT_ADDRESS, Wire);

//@-创建温湿度传感器
SHT3X sht30(0x44);


char buff_dx[128];
int good = 18;

int dis_tick = 0;
struct pl_area user_area_dx;
// uint64_t chipid; 

const char* ssid     = "wuyiyi";
const char* password = "dingxiao";


//@-网络授时
// const char* ntpServer = "pool.ntp.org";   ----->ok
// const char* ntpServer = "cn.pool.ntp.org";  ---->OK
const char* ntpServer = "cn.ntp.org.cn";

//@-设置时区参数
const long  gmtOffset_sec = 7 * 3600;
const int   daylightOffset_sec = 3600;

int wifi_connect_tick = 0;
int wifi_connect_flag = 0;  //@-0:没有连接  1:连接成功  2:连接超时

I2C_BM8563_DateTypeDef dx_dateStruct;
I2C_BM8563_TimeTypeDef dx_timeStruct;

int Button0_State = 0;          //@-按键0状态
int Button1_State = 0;          //@-按键1状态
int Button2_State = 0;          //@-按键2状态
bool Key_Flag = false;

float BAT_V = 0;


void setup()
{
  Serial.begin(115200);

  // chipid=ESP.getEfuseMac();

  //@-注册按键
  pinMode(Button0_PIN, INPUT|PULLUP);
  pinMode(Button1_PIN, INPUT|PULLUP);
  pinMode(Button2_PIN, INPUT|PULLUP);

  //@-AD采样检测
  pinMode(BAT_EN_PIN, OUTPUT);
  digitalWrite(BAT_EN_PIN, HIGH);
  analogReadResolution(BAT_AD_Solution);
  pinMode(BAT_V_PIN,INPUT);


  SPIFFS.begin();
  Serial.println("\nSPIFFS on ");  

  Wire.begin(21, 22);

  //   if(sht30.get()==0){
  //   Serial.print("Temperature in Celsius : ");
  //   Serial.println(sht30.cTemp);
  //   Serial.print("Temperature in Fahrenheit : ");
  //   Serial.println(sht30.fTemp);
  //   Serial.print("Relative Humidity : ");
  //   Serial.println(sht30.humidity);
  //   Serial.println();
  // }

  Serial.println("----------Start_Init_EPD()-----------");
  
  Serial.println("Init Epd Port");
  epd_drv_dx.EPD4INC_Port_init();
  epd_drv_dx.EPD4INC_Port_Reinit();      //SPI初始化
  epd_drv_dx.EPD_CLK_EX();
  Serial.println("01 Epd Power Up");

  delay(10);
  epd_drv_dx.s1d135xx_soft_reset();
  delay(10);

  epd_drv_dx.epson_epdc_init();
  // epd_drv_dx.epson_epdc_init_s1d13541();
  delay(5);
  epd_drv_dx.s1d135xx_set_epd_power(1);
  delay(10);


  //通过BUF处理图层
  epd_drv_dx.Buf_Clear();
  // epd_drv_dx.Buf_DrawLine(140,0,140,240);
  // epd_drv_dx.Buf_DrawLine(0,140,140,140); 
  

  epd_drv_dx.EPD4INC_HVEN();
  delay(2);

  epd_drv_dx.Buf_UpdateFull(1);

  user_area_dx.top = 0;
  user_area_dx.left = 0;   
  user_area_dx.width = 226;
  user_area_dx.height = 357;


  // S1D13541_LD_IMG_1BPP  ---->  单色
  // S1D13541_LD_IMG_2BPP  ---->  4灰
  // S1D13541_LD_IMG_4BPP  ---->  16灰
  // epd_drv_dx.User_Img_Tran(226,357,gImage_DX_EPD_Test,S1D13541_LD_IMG_4BPP,&user_area_dx,1);

  
//刷新背景
  // epd_drv.EPD_Update_Full(12000, S1D13541_LD_IMG_1BPP, gImage_InitPage);
//写入ID
  sprintf(buff_dx,"世界人民大团结万岁 =%d",good);
  epd_drv_dx.EPD_SetFount(FONT16);
  epd_drv_dx.DrawUTF( 35 , 180, buff_dx, 1);    //显示设备序列号
  
  epd_drv_dx.EPD4INC_HVEN();
  delay(10);
  epd_drv_dx.EPD_UpdateUser(1, UPDATE_FULL, NULL);
 
  Serial.println("04 Epd updat background Over");
  delay(900);
 
  epd_drv_dx.EPD4INC_HVDISEN();
  epd_drv_dx.s1d135xx_set_power_state(PL_EPDC_SLEEP);
  epd_drv_dx.EPD_CLK_STOP();

  Serial.println("----------Start_Init_EPD()Return -----------");


  // Connect to Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    wifi_connect_tick = wifi_connect_tick + 1;
    if(wifi_connect_tick > 30)
    {
      wifi_connect_tick = 0;
      wifi_connect_flag = 2;  //wifi连接超时
      break;
    }
  }
  if(wifi_connect_flag != 2)
  {
    wifi_connect_flag = 1;
    Serial.println("");
    Serial.println("WiFi connected.");
  }

  //@-wifi连接成功
  if(wifi_connect_flag == 1)
  {
    // Init and get the time
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    printLocalTime();
  }

  //disconnect WiFi as it's no longer needed
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}

void printLocalTime(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");

  // tm_sec	int	seconds after the minute	0-60*
  // tm_min	int	minutes after the hour	0-59
  // tm_hour	int	hours since midnight	0-23
  // tm_mday	int	day of the month	1-31
  // tm_mon	int	months since January	0-11
  // tm_year	int	years since 1900	
  // tm_wday	int	days since Sunday	0-6

  Serial.println("----------dx--------------");
  Serial.println(timeinfo.tm_year);
  Serial.println(timeinfo.tm_mon);
  Serial.println(timeinfo.tm_mday);
  Serial.println(timeinfo.tm_wday);
  Serial.println(timeinfo.tm_hour);
  Serial.println(timeinfo.tm_min);
  Serial.println(timeinfo.tm_sec);

  SetRTC((timeinfo.tm_year+1900), (timeinfo.tm_mon+1), timeinfo.tm_mday, timeinfo.tm_wday, 
         timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
}


void SetRTC(uint16_t rtc_year,uint8_t rtc_month,uint8_t rtc_date,uint8_t rtc_weekDay,uint8_t rtc_hours,uint8_t rtc_minutes,uint8_t rtc_seconds)
{
  rtc_dx.begin();
  // Set RTC Date
  I2C_BM8563_DateTypeDef set_dateStruct;
  set_dateStruct.weekDay = rtc_weekDay;
  set_dateStruct.month = rtc_month;
  set_dateStruct.date = rtc_date;
  set_dateStruct.year = rtc_year;
  rtc_dx.setDate(&set_dateStruct);

  // Set RTC Time
  I2C_BM8563_TimeTypeDef set_timeStruct;
  set_timeStruct.hours   = rtc_hours;
  set_timeStruct.minutes = rtc_minutes;
  set_timeStruct.seconds = rtc_seconds;
  rtc_dx.setTime(&set_timeStruct);
  }


void  ReadRTC(void)
{
  rtc_dx.begin();
  // Get RTC
  rtc_dx.getDate(&dx_dateStruct);
  rtc_dx.getTime(&dx_timeStruct);

  // Print RTC
  Serial.printf("%04d/%02d/%02d %02d:%02d:%02d\n",
                dx_dateStruct.year,
                dx_dateStruct.month,
                dx_dateStruct.date,
                dx_timeStruct.hours,
                dx_timeStruct.minutes,
                dx_timeStruct.seconds
               );
}

//@-键盘扫描
void Button_Check()
{
  //@-读取键盘值
  Button0_State = digitalRead(Button0_PIN);
  Button1_State = digitalRead(Button1_PIN);
  Button2_State = digitalRead(Button2_PIN);

  if((Button0_State == LOW) && (Key_Flag == false))
  {
    Key_Flag = true;
    Serial.println("Button0\n");
  }
  else if((Button1_State == LOW) && (Key_Flag == false))
  {
    Key_Flag = true;
    Serial.println("Button1\n");
  }
  else if((Button2_State == LOW) && (Key_Flag == false))
  {
    Key_Flag = true;
    Serial.println("Button2\n");
  }

  //@-所有功能按键均没有按下
  if((Button0_State == HIGH)&&(Button1_State == HIGH)&&(Button2_State == HIGH))
  {
    Key_Flag = false;
  }
}

void loop()
{
  //@-键盘扫描
  Button_Check();

  //@-读取AD
  BAT_V = analogRead(BAT_V_PIN)/560.1;

  dis_tick = dis_tick + 1;
  if(dis_tick >= 10)
  {
    dis_tick = 0;

    good = good + 1;

    sht30.get();  //获得温湿度数据
    delay(5);
    ReadRTC();

    Serial.println("----------DrawTimeUpdata-----------");
    epd_drv_dx.EPD4INC_Port_Reinit();      //SPI初始化
    epd_drv_dx.EPD4INC_HVEN();
    delay(2); 
    epd_drv_dx.EPD_CLK_EX();               //其中时钟
    epd_drv_dx.s1d135xx_set_epd_power(1);
    delay(2);


    //@-局部刷新
    user_area_dx.top = 180;
    user_area_dx.left = 35;
    user_area_dx.width = 200;
    user_area_dx.height = 50;

    //@-全部刷新
    // user_area_dx.top = 0;
    // user_area_dx.left = 0;
    // user_area_dx.width = 200;
    // user_area_dx.height = 400;

    epd_drv_dx.EPD_SetFount(FONT16);
    sprintf(buff_dx,"温度:%0.1f  湿度:%0.1f", sht30.cTemp, sht30.humidity);
    // if(wifi_connect_flag == 1)
    // sprintf(buff_dx,"WIFI连接成功 =%d",good);
    // else if(wifi_connect_flag != 1)
    // sprintf(buff_dx,"WIFI没有连接 =%d",good);
    epd_drv_dx.DrawUTF( 35 , 180, buff_dx, 1);    


    sprintf(buff_dx,"时间:%2d:%2d:%2d V:%0.1f", dx_timeStruct.hours, dx_timeStruct.minutes, dx_timeStruct.seconds, BAT_V);
    epd_drv_dx.DrawUTF( 35 , 200, buff_dx, 1);  


    // epd_drv.DrawTime(10,30, timeStruct.hours, timeStruct.minutes, FONT48_NUM, 1);
    epd_drv_dx.EPD_UpdateUser(2, UPDATE_PARTIAL_AREA, &user_area_dx);

    //   epd_drv_dx.EPD_UpdateUser(1, UPDATE_FULL, NULL);

    
    // Serial.println("\n Epd UPDATE_PARTIAL_AREA updat  Over");
    delay(300);
    epd_drv_dx.EPD4INC_HVDISEN();
    epd_drv_dx.s1d135xx_set_power_state(PL_EPDC_SLEEP);
    epd_drv_dx.EPD_CLK_STOP();
  }
  
  delay(10);
}