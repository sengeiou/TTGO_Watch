
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

#define ESP32

#define KEY2 26
#define KEY1 27  



EPD4INC epd_drv_dx;

//@-创建RTC
// I2C_BM8563 rtc(I2C_BM8563_DEFAULT_ADDRESS, Wire1);  //Wire1已在Wire.h文件中定义，为ESP32的I2C接口

//@-创建温湿度传感器
SHT3X sht30(0x44);


char buff_dx[128];
int good = 18;

int dis_tick = 0;
struct pl_area user_area_dx;
// uint64_t chipid; 

const char* ssid     = "wuyiyi";
const char* password = "dingxiao";

// const char* ntpServer = "pool.ntp.org";   ----->ok
// const char* ntpServer = "cn.pool.ntp.org";  ---->OK
const char* ntpServer = "cn.ntp.org.cn";


const long  gmtOffset_sec = 7 * 3600;
const int   daylightOffset_sec = 3600;




void setup()
{
  Serial.begin(115200);

  // chipid=ESP.getEfuseMac();

  pinMode(KEY1, INPUT); // 
  pinMode(KEY2, INPUT); // 

  SPIFFS.begin();
  Serial.println("\nSPIFFS on ");  

    if(sht30.get()==0){
    Serial.print("Temperature in Celsius : ");
    Serial.println(sht30.cTemp);
    Serial.print("Temperature in Fahrenheit : ");
    Serial.println(sht30.fTemp);
    Serial.print("Relative Humidity : ");
    Serial.println(sht30.humidity);
    Serial.println();
  }


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
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  
  // Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();

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
  Serial.println(timeinfo.tm_hour);
  Serial.println(timeinfo.tm_min);
  Serial.println(timeinfo.tm_sec);
}

void loop()
{
  dis_tick = dis_tick + 1;
  if(dis_tick >= 10)
  {
    dis_tick = 0;

    good = good + 1;

    sht30.get();

    Serial.println("----------DrawTimeUpdata-----------");
    epd_drv_dx.EPD4INC_Port_Reinit();      //SPI初始化
    epd_drv_dx.EPD4INC_HVEN();
    delay(2); 
    epd_drv_dx.EPD_CLK_EX();               //其中时钟
    epd_drv_dx.s1d135xx_set_epd_power(1);
    delay(2);

    user_area_dx.top = 180;
    user_area_dx.left = 35;
    user_area_dx.width = 200;
    user_area_dx.height = 16;

    sprintf(buff_dx,"温度:%0.1f  湿度:%0.1f", sht30.cTemp, sht30.humidity);
    epd_drv_dx.EPD_SetFount(FONT16);
    epd_drv_dx.DrawUTF( 35 , 180, buff_dx, 1);    //显示设备序列号

    // epd_drv.DrawTime(10,30, timeStruct.hours, timeStruct.minutes, FONT48_NUM, 1);

    epd_drv_dx.EPD_UpdateUser(2, UPDATE_PARTIAL_AREA, &user_area_dx);

    // epd_drv_dx.EPD_UpdateUser(1, UPDATE_FULL, NULL);
    
    // Serial.println("\n Epd UPDATE_PARTIAL_AREA updat  Over");
    delay(300);
    epd_drv_dx.EPD4INC_HVDISEN();
    epd_drv_dx.s1d135xx_set_power_state(PL_EPDC_SLEEP);
    epd_drv_dx.EPD_CLK_STOP();
  }
  
  delay(10);
}