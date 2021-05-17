#include "EPD4IN_Device.h"

//
//
EPD4INC epd_drv;
I2C_BM8563 rtc(I2C_BM8563_DEFAULT_ADDRESS, Wire1);
RTC_DATA_ATTR     I2C_BM8563_DateTypeDef dateStruct;
RTC_DATA_ATTR     I2C_BM8563_TimeTypeDef timeStruct;
//SHTSensor sht;
 

extern RTC_DATA_ATTR EPD_Device_Struct epd_rtc_data;;
char buff[128];
 
const char softver[]="V2.05";
struct pl_area;
struct pl_area user_area;


RTC_DATA_ATTR int bootcounter = 0;       //系统启动的类型 

uint8_t EPD4IN_Device:: IIC_Device_Dected(uint8_t *device_id)
{
  Serial.println ("I2C scanner. Scanning ...");
  uint8_t count = 0;
  Wire.begin();
  for (uint8_t i = 8; i < 120; i++) {
    Wire.beginTransmission (i);
    if (Wire.endTransmission () == 0) {
      Serial.print ("Found address: ");
      device_id[count] = i;
      Serial.print (i, DEC);
      Serial.print (" (0x");
      Serial.print (i, HEX);
      Serial.println (")");
      count++;
      delay (1); // maybe unneeded?
    } // end of good response
  } // end of for loop
  return count;
}
//uint8_t EPD4IN_Device:: SHT_Init(void)
//{
//  Wire1.begin(21, 22);
//  if (sht.init()) {
//    Serial.print("init(): success\n");
//  } else {
//    Serial.print("init(): failed\n");
//  }
//
//}
uint8_t EPD4IN_Device:: RTC_Init(void)
{

  Wire1.begin(21, 22);
  rtc.begin();

  // Set RTC Date
  //  I2C_BM8563_DateTypeDef dateStruct;
  //  dateStruct.weekDay = 3;
  //  dateStruct.month = 3;
  //  dateStruct.date = 22;
  //  dateStruct.year = 2019;
  //  rtc.setDate(&dateStruct);
  //  // Set RTC Time
  //  I2C_BM8563_TimeTypeDef timeStruct;
  //  timeStruct.hours   = 18;
  //  timeStruct.minutes = 56;
  //  timeStruct.seconds = 10;
  //  rtc.setTime(&timeStruct);
}
//初始化EPD    
//初始化页面，完成开机页面显示
void EPD4IN_Device::Start_Init_EPD(void)
{
  Serial.println("----------Start_Init_EPD()-----------");
  
  Serial.println("Init Epd Port");
  epd_drv.EPD4INC_Port_init();
  epd_drv.EPD_CLK_EX();
  Serial.println("01 Epd Power Up");

  delay(10);
  epd_drv.s1d135xx_soft_reset();
  delay(10);

  epd_drv.epson_epdc_init();
  delay(5);
  epd_drv.s1d135xx_set_epd_power(1);
  delay(10);

  user_area.top = 180;
  user_area.left = 35;   
  user_area.width = 120;
  user_area.height = 16;
  
//刷新背景
  // epd_drv.EPD_Update_Full(12000, S1D13541_LD_IMG_1BPP, gImage_InitPage);
//写入ID
  sprintf(buff,"Devide ID=%llx",ESP.getEfuseMac());
  epd_drv.EPD_SetFount(FONT16);
  epd_drv.DrawUTF( user_area.left , user_area.top, buff, 1);    //显示设备序列号
  
  epd_drv.EPD4INC_HVEN();
  delay(10);
  epd_drv.EPD_UpdateUser(1, UPDATE_FULL, NULL);
 
  Serial.println("04 Epd updat background Over");
  delay(900);
 
  epd_drv.EPD4INC_HVDISEN();
  epd_drv.s1d135xx_set_power_state(PL_EPDC_SLEEP);
  epd_drv.EPD_CLK_STOP();

  
  Serial.println("----------Start_Init_EPD()Return -----------");
}
void EPD4IN_Device::SEt_EPD_NeedConfig(void)
{
  Serial.println("----------SEt_EPD_NeedConfig()-----------");
  Serial.println("DrawStartScreen");
  epd_drv.EPD4INC_Port_Reinit();      //SPI初始化
  epd_drv.EPD_CLK_EX();               //其中时钟
 
 
  epd_drv.s1d135xx_set_epd_power(1);
  delay(2);
 //通过BUF处理图层
  // epd_drv.EPD_Update_Full(12000, S1D13541_LD_IMG_1BPP, gImage_needinit);
  epd_drv.EPD4INC_HVEN();
   
  user_area.top = 270;
  user_area.left = 30;   
  user_area.width = 120;
  user_area.height = 16;

  sprintf(buff,"Devide ID=%llx",ESP.getEfuseMac());
  epd_drv.EPD_SetFount(FONT16);
  epd_drv.DrawUTF( user_area.left , user_area.top, buff, 1);    //显示设备序列号

  delay(10);
  epd_drv.EPD_UpdateUser(1, UPDATE_FULL, NULL);
  Serial.println("04 Epd updat background Over");
  delay(900);

  Serial.println("05 shuban delay over");
  Serial.println("06 Epd Go To sleep");
  epd_drv.EPD4INC_HVDISEN();
  epd_drv.s1d135xx_set_power_state(PL_EPDC_SLEEP);
  epd_drv.EPD_CLK_STOP();
 
     
  epd_drv.EPD_UpdateUser(1, UPDATE_FULL, NULL);
  
  Serial.println("\n04 Epd updat  Over");
  delay(900);
  epd_drv.EPD4INC_HVDISEN();
  epd_drv.s1d135xx_set_power_state(PL_EPDC_SLEEP);
  epd_drv.EPD_CLK_STOP();
  Serial.println("----------SEt_EPD_NeedConfig()-----------");
}


//wifisetimg

void EPD4IN_Device::SEt_EPD_WIFICONFIG(void)
{
  Serial.println("DrawStartScreen");
  epd_drv.EPD4INC_Port_Reinit();      //SPI初始化
  epd_drv.EPD_CLK_EX();               //其中时钟
  epd_drv.EPD_CLK_EX(); 
  epd_drv.s1d135xx_set_epd_power(1);
  epd_drv.s1d135xx_set_epd_power(1);
  delay(2);
 //通过BUF处理图层
  // epd_drv.EPD_Update_Full(12000, S1D13541_LD_IMG_1BPP, wifisetimg);
  epd_drv.EPD4INC_HVEN();
   
  user_area.top = 350;
  user_area.left = 30;   
  user_area.width = 120;
  user_area.height = 16;

  sprintf(buff,"Devide ID=%llx",ESP.getEfuseMac());
  epd_drv.EPD_SetFount(FONT16);
  epd_drv.DrawUTF( user_area.left , user_area.top, buff, 1);    //显示设备序列号

  delay(10);
  epd_drv.EPD_UpdateUser(1, UPDATE_FULL, NULL);
  Serial.println("04 Epd updat background Over");
  delay(900);

  Serial.println("05 shuban delay over");
  Serial.println("06 Epd Go To sleep");
  epd_drv.EPD4INC_HVDISEN();
  epd_drv.s1d135xx_set_power_state(PL_EPDC_SLEEP);
  epd_drv.EPD_CLK_STOP();
 
     
  epd_drv.EPD_UpdateUser(1, UPDATE_FULL, NULL);
  
  Serial.println("\n04 Epd updat  Over");
  delay(900);
  epd_drv.EPD4INC_HVDISEN();
  epd_drv.s1d135xx_set_power_state(PL_EPDC_SLEEP);
  epd_drv.EPD_CLK_STOP();
  
 
}
//初始化EPD
void EPD4IN_Device::Start_EPD_Show(void)
{

    DrawStartScreen();

}
 
uint16_t EPD4IN_Device::DrawStartScreen(void)
{
  uint16_t returntime;
  Serial.println("----------DrawStartScreen-----------");
  epd_drv.EPD4INC_Port_Reinit();      //SPI初始化

  epd_drv.EPD_CLK_EX();               //其中时钟
  epd_drv.s1d135xx_set_epd_power(1);
  delay(2);
 
 
//通过BUF处理图层
  epd_drv.Buf_Clear();
  epd_drv.Buf_DrawLine(140,0,140,240);
  epd_drv.Buf_DrawLine(0,140,140,140); 
  

  epd_drv.EPD4INC_HVEN();
  delay(2);

  epd_drv.Buf_UpdateFull(1);
//ico_sunny
//int EPD4INC::User_Img_Tran(uint16_t height, uint16_t width, const  uint8_t* p_img, uint16_t mode, const struct pl_area *area, uint8_t isinv)
    
  user_area.top =40 ;
  user_area.left = 160;
  user_area.width = 64;
  user_area.height = 64;

  uint8_t  testicostr[512];

  SearchWeatherIco(testicostr,508);
  epd_drv.User_Img_Tran(64,64,testicostr,S1D13541_LD_IMG_1BPP,&user_area,1);
  user_area.top+=64;
  SearchWeatherIco(testicostr,301);
  epd_drv.User_Img_Tran(64,64,testicostr,S1D13541_LD_IMG_1BPP,&user_area,1);
  user_area.top+=64;
  SearchWeatherIco(testicostr,302);
  epd_drv.User_Img_Tran(64,64,testicostr,S1D13541_LD_IMG_1BPP,&user_area,1);  
  user_area.top+=64;
  SearchWeatherIco(testicostr,317);
  epd_drv.User_Img_Tran(64,64,testicostr,S1D13541_LD_IMG_1BPP,&user_area,1);
  user_area.top+=64;
  SearchWeatherIco(testicostr,901);
  epd_drv.User_Img_Tran(64,64,testicostr,S1D13541_LD_IMG_1BPP,&user_area,1);

  user_area.top = 0;
  user_area.left = 0;
  user_area.width = 120;
  user_area.height = 16;

 // sprintf(buff, "2021年1月21日 星期四",bootcounter++);

 
  Serial.println("Set RTC Buf  ");
  SetRtcBuf(buff);
   
  epd_drv.EPD_SetFount(FONT12);
  epd_drv.DrawUTF( user_area.left , user_area.top, buff, 1);    //日期显示

 
   
  user_area.top = 88;
  user_area.left = 16; 
  
  Serial.println("Set SHT Buf  ");
  SetShtBuf(buff);
  epd_drv.EPD_SetFount(FONT16);
  epd_drv.DrawUTF( user_area.left , user_area.top, buff, 1);    //RTC显示
  epd_drv.DrawTime(10,30, timeStruct.hours, timeStruct.minutes, FONT48_NUM, 1);

  user_area.top = 160;
  user_area.left = 30;   
  sprintf(buff,"Devide ID=%llx",ESP.getEfuseMac());
  epd_drv.EPD_SetFount(FONT16);
  epd_drv.DrawUTF( user_area.left , user_area.top, buff, 1);    //显示设备序列号
 
  user_area.top = 190;
  user_area.left = 30;   
  sprintf(buff,"BAT VOL is=%2.1fV",GetBatVol());
  epd_drv.EPD_SetFount(FONT16);
  epd_drv.DrawUTF( user_area.left , user_area.top, buff, 1);    //显示设备序列号
  epd_drv.EPD_UpdateUser(1, UPDATE_FULL, NULL);
  
  Serial.println("\n04 Epd updat  Over");
  delay(900);
  epd_drv.EPD4INC_HVDISEN();
  epd_drv.s1d135xx_set_power_state(PL_EPDC_SLEEP);
  epd_drv.EPD_CLK_STOP();
  return   SetRtcBuf(buff);
}
//全屏幕刷新
//完成屏幕布局，刷新所有界面 
uint16_t EPD4IN_Device::DrawFullScreen(void)
{
  uint16_t returntime;
  char buff[128];
  Serial.println("----------DrawFullScreen-----------");
  epd_drv.EPD4INC_Port_Reinit();      //SPI初始化
  epd_drv.EPD_CLK_EX();               //其中时钟
  epd_drv.s1d135xx_set_epd_power(1);
  delay(2);
 
 
//通过BUF处理图层
  epd_drv.Buf_Clear();
  epd_drv.Buf_DrawLine(140,0,140,240);
  epd_drv.Buf_DrawLine(0,140,140,140); 
  

  epd_drv.EPD4INC_HVEN();
  delay(2);

  epd_drv.Buf_UpdateFull(1);
 
  user_area.top =40 ;
  user_area.left = 160;
  user_area.width = 64;
  user_area.height = 64;
//天气图标
  uint8_t  testicostr[512];
  SearchWeatherIco(testicostr,epd_rtc_data.Weather_TypeCode);
  epd_drv.User_Img_Tran(64,64,testicostr,S1D13541_LD_IMG_1BPP,&user_area,1);
 //城市和实时温度
  user_area.top = 22;
  user_area.left = 160;
  user_area.width = 32;
  user_area.height = 16;
  memset(buff,0,128);
  sprintf(buff,"%s %s℃",epd_rtc_data.Weather_City,epd_rtc_data.Weather_Tmp_Real);
  epd_drv.EPD_SetFount(FONT16);
  epd_drv.DrawUTF( user_area.left , user_area.top, buff, 1);    //城市名称
//天气  温度范围
  user_area.top = 104;
  user_area.left = 148;
  user_area.width = 32;
  user_area.height = 16;
  memset(buff,0,128);
  sprintf(buff,"%s%s-%s℃",epd_rtc_data.Weather_Type,epd_rtc_data.Weather_Tmp_Low,epd_rtc_data.Weather_Tmp_High);
  epd_drv.EPD_SetFount(FONT16);
  epd_drv.DrawUTF( user_area.left , user_area.top, buff, 1);    //   
 
//年月日 日期显示
  user_area.top = 0;
  user_area.left = 0;
  user_area.width = 120;
  user_area.height = 16;
  Serial.println("Set RTC Buf  ");
  SetRtcBuf(buff);
  epd_drv.EPD_SetFount(FONT12);
  epd_drv.DrawUTF( user_area.left , user_area.top, buff, 1);    //日期显示
//温湿度显示
  user_area.top = 80;
  user_area.left = 16; 
  Serial.println("Set SHT Buf  ");
//  SetShtBuf(buff);
  epd_drv.EPD_SetFount(FONT16);
  epd_drv.DrawUTF( user_area.left , user_area.top, buff, 1);    //RTC显示
  epd_drv.DrawTime(10,30, timeStruct.hours, timeStruct.minutes, FONT48_NUM, 1);

//阴历和节假日
  user_area.top = 100;
  if(epd_rtc_data.Calendar_holiday[0]==NULL)
    user_area.left = 40; 
  else
     user_area.left = 12; 
  
  memset(buff,0,128);
  sprintf(buff,"%s %s",epd_rtc_data.Calendar_lunar,epd_rtc_data.Calendar_holiday);
  epd_drv.EPD_SetFount(FONT16);
  epd_drv.DrawUTF( user_area.left , user_area.top, buff, 1);    //RTC显示
//电池电压
  user_area.top = 0;
  user_area.left = 212;   
  memset(buff,0,128);
  sprintf(buff,"%2.1fV",GetBatVol());
  epd_drv.EPD_SetFount(FONT12);
  epd_drv.DrawUTF( user_area.left , user_area.top, buff, 1);    //电池电压

//softver
//电池电压
  user_area.top = 0;
  user_area.left = 100;   
  memset(buff,0,128);
  sprintf(buff,"%s",softver);
  epd_drv.EPD_SetFount(FONT12);
  epd_drv.DrawUTF( user_area.left , user_area.top, buff, 1);    //电池电压


//刷新时间
  TestRtc();
  epd_drv.DrawTime(10,30, timeStruct.hours, timeStruct.minutes, FONT48_NUM, 1);
  
//TODO  标题
  user_area.top = 144;
  user_area.left = 100;   
  memset(buff,0,128);
  sprintf(buff,"Todo",epd_rtc_data.Todo_Uptime);
  epd_drv.EPD_SetFount(FONT16);
  epd_drv.DrawUTF( user_area.left , user_area.top, buff, 1);    // 
  //TODO  更新时间
  user_area.top = 148;
  user_area.left = 150;   
  memset(buff,0,128);
  sprintf(buff,"%s更新",epd_rtc_data.Todo_Uptime);
  epd_drv.EPD_SetFount(FONT12);
  epd_drv.DrawUTF( user_area.left , user_area.top, buff, 1);    // 
  //todo 内容
  user_area.top = 162;
  user_area.left =0; 
    epd_drv.EPD_SetFount(FONT16);
  for(int todoindx=0;todoindx<epd_rtc_data.Todo_count;todoindx++)
  {
       memset(buff,0,128);
       sprintf(buff,"->%s",epd_rtc_data.Todo_New_List[todoindx]);
       epd_drv.DrawUTF( user_area.left , user_area.top, buff, 1);    // 
       user_area.top +=17;
         
  }


  


  
  ///////////////////////////////////////////////////////////////////////
  







 
  epd_drv.EPD_UpdateUser(1, UPDATE_FULL, NULL);
  
  Serial.println("\n04 Epd updat  Over");
  delay(900);
  epd_drv.EPD4INC_HVDISEN();
  epd_drv.s1d135xx_set_power_state(PL_EPDC_SLEEP);
  epd_drv.EPD_CLK_STOP();
  return   SetRtcBuf(buff);
}
// 局部刷新时间
uint16_t EPD4IN_Device::DrawTimeUpdata(void)
{
   
  Serial.println("----------DrawTimeUpdata-----------");
  epd_drv.EPD4INC_Port_Reinit();      //SPI初始化
  epd_drv.EPD4INC_HVEN();
  delay(2); 
  epd_drv.EPD_CLK_EX();               //其中时钟
  epd_drv.s1d135xx_set_epd_power(1);
  delay(2);
  user_area.top = 30;
  user_area.left = 10;
  user_area.width = 120;
  user_area.height = 48;
  TestRtc();  //更新时间
  epd_drv.DrawTime(10,30, timeStruct.hours, timeStruct.minutes, FONT48_NUM, 1);

 
  epd_drv.EPD_UpdateUser(2, UPDATE_PARTIAL_AREA, &user_area);
  
  Serial.println("\n Epd UPDATE_PARTIAL_AREA updat  Over");
  delay(300);
  epd_drv.EPD4INC_HVDISEN();
  epd_drv.s1d135xx_set_power_state(PL_EPDC_SLEEP);
  epd_drv.EPD_CLK_STOP();
 
}

//void EPD4IN_Device::TestSht(void)
//{
//      Wire.begin();
//
//   if (sht.init()) {
//      Serial.print("init(): success\n");
//  } else {
//      Serial.print("init(): failed\n");
//  } 
//  delay(1);
//  sht.setAccuracy(SHTSensor::SHT_ACCURACY_MEDIUM); // only supported by SHT3x
//  delay(5);
//  if (sht.readSample()) {
//      Serial.print("SHT:\n");
//      Serial.print("  RH: ");
//      Serial.print(sht.getHumidity(), 2);
//      Serial.print("\n");
//      Serial.print("  T:  ");
//      Serial.print(sht.getTemperature(), 2);
//      Serial.print("\n");
//  } else {
//      Serial.print("Error in readSample()\n");
//  }
//   
//}
//
//void EPD4IN_Device::SetShtBuf(char *shtbuf)
//{
//    delay(1);
//    Serial.print("init SHT\n");
//      Wire.begin();
//      delay(5);
//      sht.init();
//      delay(5);
//   if (sht.init()) {
//      Serial.print("init(): success\n");
//  } else {
//      Serial.print("init(): failed\n");
//  } 
//  delay(1);
//  sht.setAccuracy(SHTSensor::SHT_ACCURACY_MEDIUM); // only supported by SHT3x
//    delay(5);
//    sht.readSample();
//    delay(5);
//  if (sht.readSample()) {
//      Serial.print("SHT:\n");
//      Serial.print("  RH: ");
//      Serial.print(sht.getHumidity(), 2);
//      Serial.print("\n");
//      Serial.print("  T:  ");
//      Serial.print(sht.getTemperature(), 2);
//      Serial.print("\n");
//      sprintf(shtbuf, "%2.1f%cRH %2.1f℃",
//                sht.getHumidity(),
//                '%',
//                sht.getTemperature()
//                );
//      
//
//      
//  } else {
//      Serial.print("Error in readSample()\n");
//  }
//   
//}
void  EPD4IN_Device::TestRtc(void)
{
    I2C_BM8563_DateTypeDef dateStruct;
    I2C_BM8563_TimeTypeDef timeStruct;
  Wire1.begin(21, 22);
  rtc.begin();
  // Get RTC
  rtc.getDate(&dateStruct);
  rtc.getTime(&timeStruct);

  // Print RTC
  Serial.printf("%04d/%02d/%02d %02d:%02d:%02d\n",
                dateStruct.year,
                dateStruct.month,
                dateStruct.date,
                timeStruct.hours,
                timeStruct.minutes,
                timeStruct.seconds
               );
}
uint16_t  EPD4IN_Device::SetRtcBuf(char *rtcbuf)
{

  Wire1.begin(21, 22);
  rtc.begin();
  // Get RTC
  rtc.getDate(&dateStruct);
  rtc.getTime(&timeStruct);

  // Print RTC
  Serial.printf("%04d/%02d/%02d %02d:%02d:%02d\n",
                dateStruct.year,
                dateStruct.month,
                dateStruct.date,
                timeStruct.hours,
                timeStruct.minutes,
                timeStruct.seconds
               );

  sprintf(rtcbuf, "%04d/%02d/%02d",
                dateStruct.year,
                dateStruct.month,
                dateStruct.date);

  return timeStruct.seconds;
    
}
uint16_t  EPD4IN_Device::GetRtcSenconds(void)
{
  Wire1.begin(21, 22);
  rtc.begin();
  // Get RTC
  rtc.getTime(&timeStruct);
  return timeStruct.seconds;
}
uint16_t  EPD4IN_Device::GetRtcMinute(void)
{
  Wire1.begin(21, 22);
  rtc.begin();
  // Get RTC
  rtc.getTime(&timeStruct);
  return timeStruct.minutes;
}
uint16_t  EPD4IN_Device::GetRtcHour(void)
{
  Wire1.begin(21, 22);
  rtc.begin();
  // Get RTC
  rtc.getTime(&timeStruct);
  return timeStruct.hours;
}

void  EPD4IN_Device::Rtc_Set(uint16_t rtc_year,uint8_t rtc_month,uint8_t rtc_date,uint8_t rtc_weekDay,uint8_t rtc_hours,uint8_t rtc_minutes,uint8_t rtc_seconds)
{

  Wire1.begin(21, 22);
  rtc.begin();
    // Set RTC Date
  I2C_BM8563_DateTypeDef dateStruct;
  dateStruct.weekDay = rtc_weekDay;
  dateStruct.month = rtc_month;
  dateStruct.date = rtc_date;
  dateStruct.year = rtc_year;
  rtc.setDate(&dateStruct);

  // Set RTC Time
  I2C_BM8563_TimeTypeDef timeStruct;
  timeStruct.hours   = rtc_hours;
  timeStruct.minutes = rtc_minutes;
  timeStruct.seconds = rtc_seconds;
  rtc.setTime(&timeStruct);
  }
float  EPD4IN_Device::GetBatVol(void)
{
  float analog_value = 0;
  pinMode(23, OUTPUT); //BAT EN H
  digitalWrite(23, HIGH);
  analogReadResolution(12);
  pinMode(35,INPUT);
  delay(20);
  analog_value = analogRead(35)/560.1;
  Serial.printf("BAT VOL IS%f:\n",analog_value);
  return analog_value;
}
int EPD4IN_Device::SearchWeatherIco(uint8_t *ico_buf,int weathertype)
{
  //   for(int si=0;si<100;si++)
  //   {
      
  //    if( Weather_ico[si].index==weathertype)
  //    {
  // //    ico_buf=Weather_ico[si];
  //     memcpy(ico_buf,Weather_ico[si].model,512);
  //     return 1;
  //     }
 
  //   }
 
}
//void Get_QR_Img()
//
