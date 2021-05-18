
#include <SPIFFS.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <ArduinoJson.h>
#include <Wire.h>

#include "EPD_Http.h"
#include "EPD4IN_Device.h"
#include "EPD4IN_driver.h"
#include "driver/rtc_io.h"


#define KEY2 26
#define KEY1 27  



EPD4INC epd_drv_dx;

char buff_dx[128];
int good = 18;



void setup()
{
  Serial.begin(115200);

  pinMode(KEY1, INPUT); // 
  pinMode(KEY2, INPUT); // 

  SPIFFS.begin();
  Serial.println("\nSPIFFS on ");  

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

  // user_area.top = 180;
  // user_area.left = 35;   
  // user_area.width = 120;
  // user_area.height = 16;
  
//刷新背景
  // epd_drv.EPD_Update_Full(12000, S1D13541_LD_IMG_1BPP, gImage_InitPage);
//写入ID
  sprintf(buff_dx,"世界人民大团结万岁 dingxiao =%d",good);
  epd_drv_dx.EPD_SetFount(FONT12);
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
}


void loop()
{


}