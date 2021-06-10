
#include <SPIFFS.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <I2C_BM8563.h>
#include <WEMOS_SHT3X.h>
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>

#include <HTTPClient.h>


// #include "EPD_Http.h"     ----20210606-dx
#include "EPD4IN_Device.h"
#include "EPD4IN_driver.h"
#include "driver/rtc_io.h"

//@-图片数据
#include "DX_EPD_Test.h"


//20210520-该版本EPD无光感传感器
// #define XDOT  240
// #define YDOT  400

#define ESP32

//@-IO定义
#define Button0_PIN   0  
#define Button1_PIN  27  
#define Button2_PIN  26
#define BAT_EN_PIN   23
#define BAT_V_PIN    35
#define BAT_AD_Solution 12

#define uS_TO_S_FACTOR 1000000ULL  /* Conversion factor for micro seconds to seconds */

//@-GUI界面配置---
//---------------------------------------------
#define Area1_Box_X      120
#define Area1_Box_High   105
#define Area2_Box_X      60
#define Area2_Box_High   Area1_Box_High + 50




//@-创建EPD驱动
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

//@-wifi连接信息
const char* ssid     = "wuyiyi";
const char* password = "dingxiao";
int wifi_connect_tick = 0;
int wifi_connect_flag = 0;  //@-0:没有连接  1:连接成功  2:连接超时


//@-网络授时
// const char* ntpServer = "pool.ntp.org";   ----->ok
// const char* ntpServer = "cn.pool.ntp.org";  ---->OK
// const char* ntpServer = "cn.ntp.org.cn";
const char* ntpServer = "time.pool.aliyun.com";

//@-设置时区参数
const long  gmtOffset_sec = 7 * 3600;
const int   daylightOffset_sec = 3600;



//@-RTC数据
I2C_BM8563_DateTypeDef dx_dateStruct;
I2C_BM8563_TimeTypeDef dx_timeStruct;

//@-键盘数据
int Button0_State = 0;          //@-按键0状态
int Button1_State = 0;          //@-按键1状态
int Button2_State = 0;          //@-按键2状态
bool Key_Flag = false;

//@-电池电压
float BAT_V = 0;

//@-ESP32唤醒
esp_sleep_wakeup_cause_t wakeup_reason;

//@-RTC数据存储区
RTC_DATA_ATTR int bootCount = 0;

//@-管脚mask
uint64_t mask;

//---------------------------JSON---------------------------------------
//Your Domain name with URL path or IP address with path
// String serverName = "http://wufazhuce.com/one/3200";
// String serverName = "https://v2.jinrishici.com/info";
// String serverName = "http://sentence.iciba.com/index.php?c=dailysentence&m=getdetail&title=2021-05-31";
// String serverName = "http://c.m.163.com/nc/article/headline/T1348647853363/0-40.html";   //获取40条头条
// String serverName = "http://c.m.163.com/nc/article/headline/T1348647853363/0-10.html";   //获取10条头条
// String serverName = "http://arduinojson.org/example.json";
// String serverName = "https://v1.hitokoto.cn/";
// String serverName = "http://sentence.iciba.com/index.php?c=dailysentence&m=getdetail&title=2021-05-31";  //@-每日一句
// String serverName = "http://v.juhe.cn/toutiao/index? &type=top &page=1 &page_size=2 &key=71afabc411187aef339731d24ac43b97";

String serverName_sinaNews = "http://interface.sina.cn/dfz/outside/wap/news/list.d.html?col=56261&show_num=3";  //@-新浪综合新闻5条
String serverName_covid = "https://lab.isaaclin.cn/nCoV/api/overall";
String serverName_weather = "http://apis.juhe.cn/simpleWeather/query?&city=杭州&key=2b636957c5b1b630bf13194d76d86801";
String serverName_huangli = "http://v.juhe.cn/calendar/day?key=9774f2f31c8349cbab916eaf11d849db"; //date=2021-6-9

//@-新闻数据结构体
typedef struct {
  char news_title[256];
  // char news_author_name[64];
} NewsData_t;
NewsData_t NewsData[3];

//@-Covid-19数据结构体
typedef struct {
  int currentConfirmedCount;  //@-国内当前确诊数
  int currentConfirmedIncr;   //@-国内当前新增确诊数
  long confirmedCount;        //@-国内总确诊数
  int  confirmedIncr;         //@-国内新增总确诊数
  int suspectedCount;         //@-国内疑似数
  int suspectedIncr;          //@-国内新增疑似数
  long curedCount;            //@-国内治愈数
  int  curedIncr;             //@-国内新增治愈术
  int deadCount;              //@-国内死亡数
  int deadIncr;               //@-国内新增死亡数
  int seriousCount;           //@-国内重症数
  int seriousIncr;            //@-国内新增重症数

  long g_currentConfirmedCount; //@-全球当前确诊数
  long g_currentConfirmedIncr;  //@-全球当前新增确诊数
  long g_confirmedCount;        //@-全球总确诊数
  long g_confirmedIncr;         //@-全球新增总确诊数
  long g_curedCount;            //@-全球治愈数
  long g_curedIncr;             //@-全球新增治愈术
  long g_deadCount;             //@-全球死亡数
  long g_deadIncr;              //@-全球新增死亡数

} Covid19Data_t;
Covid19Data_t Covid19Data;

//@-聚合天气数据
typedef struct {
  char weather_city[32];                //@-温度信息城市
  char weather_temperature[72];         //@-全天天气温度范围
  char weather_info[48];                //@-全天天气情况
  char weather_info_id[16];             //@-全天天气情况图标id
  char weather_humidity[16];            //@-当前天气湿度
  char weather_direct[48];              //@-当前风向
  char weather_power[32];               //@-当前风力
  char weather_aqi[32];                 //@-当前AQI指数

  char weather_futureDay1_temperature[72];        //@-未来1天天气温度范围
  char weather_futureDay1_info[48];               //@-未来1天天气情况

  char weather_futureDay2_temperature[72];        //@-未来2天天气温度范围
  char weather_futureDay2_info[48];               //@-未来2天天气情况

  char weather_futureDay3_temperature[72];        //@-未来3天天气温度范围
  char weather_futureDay3_info[48];               //@-未来3天天气情况
  
} Juhe_WeatherData_t;
Juhe_WeatherData_t Juhe_WeatherData;

//@-聚合黄历数据数据
typedef struct {
  char lunarYear[32];                //@-农历年信息 
  char lunar[40];                    //@-农历日信息 
} Juhe_HuangliData_t;
Juhe_HuangliData_t Juhe_HuangliData;


//@-WEB服务器
AsyncWebServer server_dx(80);


//@-天气id对应图标
const uint8_t* weather_index_img_id_dx = gImage_weather_00;





// Replaces placeholder with LED state value
String processor(const String& var){
  Serial.println(var);
  // ledState = "ON";
  // if(var == "STATE"){
  //   if(digitalRead(ledPin)){
  //     ledState = "ON";
  //   }
  //   else{
  //     ledState = "OFF";
  //   }
  //   Serial.print(ledState);
    // return ledState;
  // }
  return String();
}

//@-配置
void setup()
{
  char temp_str[256];

  //@-初始化串口
  Serial.begin(115200);

  //Increment boot number and print it every reboot
  ++bootCount;
  Serial.println("Boot number: " + String(bootCount));

  //ESP32启动方式
  wakeup_reason = esp_sleep_get_wakeup_cause();
  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
  }

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

  //@-文件系统启动
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  //@-配置I2C总线
  Wire.begin(21, 22);

  //@-获得数据
  delay(5);
  sht30.get();  //获得温湿度数据
  delay(5);
  ReadRTC();

  //@-配置wifi连接-每5min检测wifi连接-并获取json数据
  if((dx_timeStruct.minutes == 5)||(dx_timeStruct.minutes == 15)||(dx_timeStruct.minutes == 20)||
     (dx_timeStruct.minutes == 25)||(dx_timeStruct.minutes == 30)||(dx_timeStruct.minutes == 35)||
     (dx_timeStruct.minutes == 40)||(dx_timeStruct.minutes == 45)||(dx_timeStruct.minutes == 50)||
     (dx_timeStruct.minutes == 55)||(dx_timeStruct.minutes == 0) ||(bootCount == 1))
  {
    //@-连接wifi
    WIFI_Connect();

    // if(WiFi.status()== WL_CONNECTED)
    // {
    //     // Print ESP32 Local IP Address
    //     Serial.println(WiFi.localIP());

    //     // Route for root / web page
    //     server_dx.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    //       request->send(SPIFFS, "/index.html", String(), false, processor);
    //     });
        
    //     // Route to load style.css file
    //     server_dx.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    //       request->send(SPIFFS, "/style.css", "text/css");
    //     });
          // // Start server_dx
          // server_dx.begin();
    // }

    //@-每5min获取Sina综合新闻json数据
    WIFI_Get_JsonInfo(serverName_sinaNews, 1);

    //@-每1hour获取Covid数据
    if(dx_timeStruct.minutes == 0)
    WIFI_Get_JsonInfo(serverName_covid, 2);

    //@-工作时间每天2次获取天气数据
    if((((dx_timeStruct.hours == 7)||(dx_timeStruct.hours == 11))&&(dx_timeStruct.minutes == 0))||(bootCount == 1))
    WIFI_Get_JsonInfo(serverName_weather, 3);

    //@-获得黄历数据
    if(((dx_timeStruct.hours == 1)&&(dx_timeStruct.minutes == 0))||(bootCount == 1))
    {
      sprintf(temp_str, "&date=%d-%d-%d", dx_dateStruct.year, dx_dateStruct.month, dx_dateStruct.date);
      String huangliData = serverName_huangli + String(temp_str);
      Serial.println(huangliData);
      WIFI_Get_JsonInfo(huangliData, 4);
    }

  }

  //@-读取AD
  BAT_V = analogRead(BAT_V_PIN)/560.1;

  //@-读取RTC数据
  delay(2);
  ReadRTC();
  
  //@-显示内容
  if((wakeup_reason == 0))
  EPD_ShowMain();

  // else if(wakeup_reason == ESP_SLEEP_WAKEUP_TIMER)
  EPD_ShowArea();

  //保证屏幕RST引脚高电平
  rtc_gpio_pullup_en(GPIO_NUM_4);
  delay(2);
  rtc_gpio_pulldown_dis(GPIO_NUM_4);   

  //@-唤醒设置 
  mask|=  1ull << Button2_PIN;
//mask|=  1ull << 27;
  //@-配置外部按键唤醒
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_0,0);
  esp_sleep_enable_ext1_wakeup(mask,ESP_EXT1_WAKEUP_ALL_LOW);
  //@-配置定时器唤醒
  esp_sleep_enable_timer_wakeup(50 * uS_TO_S_FACTOR);
  Serial.println("----------Sleep Now-----------");
  esp_deep_sleep_start();
}

//@-获取信息网站JSON数据
void WIFI_Get_JsonInfo(String serverName, int Data_Mode)
{
  char temp[256];
  int httpResponseCode;
  int http_payload_size;

  //@-判断WIFI连接状态
  if(WiFi.status()== WL_CONNECTED)
  {
    //@-创建HTTP链接
    HTTPClient http;

    // String serverPath = serverName + "?temperature=24.37";

    //@-获得指定网站的JSON数据
    http.begin(serverName.c_str());

    //@-发送HTTP Get
    httpResponseCode = http.GET();

    //@-成功连接网站
    if (httpResponseCode>0) 
    {
      //@-获得GET返回值
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      String payload = http.getString();

      //@1-从html页面中获取信息
      // String word3 = getValue(payload, '\n', 77);
      // Serial.println(word3);

      //@2-打印获得的数据长度
      http_payload_size = payload.length();
      Serial.print("HTTP payload Size:");
      Serial.println(http_payload_size);
      // Serial.println(payload);

      if(http_payload_size > 100)
      {
        //@3-从网站提供的API接口中获取信息，并将数据json化
        DynamicJsonDocument doc(3072);
        // StaticJsonDocument<2048> doc;

        //@-序列化JSON数据
        DeserializationError error = deserializeJson(doc, payload);
        if (error) 
        {
          Serial.println("JSON parsing failed!");
        } 
        else 
        {
          // get the JsonObject in the JsonDocument
          JsonObject root = doc.as<JsonObject>(); 

          //@-Sina综合新闻数据
          if(Data_Mode == 1)
          {
            strcpy(temp, root["result"]["data"]["list"][0]["title"]);
            sprintf(NewsData[0].news_title, "1.%s     ", temp);
            // Serial.println(strlen(NewsData[0].news_title));
            strcpy(temp, root["result"]["data"]["list"][1]["title"]);
            sprintf(NewsData[1].news_title, "2.%s     ", temp);
            // Serial.println(strlen(NewsData[1].news_title));
            strcpy(temp, root["result"]["data"]["list"][2]["title"]);
            sprintf(NewsData[2].news_title, "3.%s     ", temp);
            // Serial.println(strlen(NewsData[2].news_title));
          }
          //@-Covid-19数据
          else if(Data_Mode == 2)
          {
            Covid19Data.currentConfirmedCount = root["results"][0]["currentConfirmedCount"];
            Covid19Data.currentConfirmedIncr = root["results"][0]["currentConfirmedIncr"];
            Covid19Data.confirmedCount = root["results"][0]["confirmedCount"];
            Covid19Data.confirmedIncr = root["results"][0]["confirmedIncr"];
            Covid19Data.suspectedCount = root["results"][0]["suspectedCount"];
            Covid19Data.suspectedIncr = root["results"][0]["suspectedIncr"];
            Covid19Data.curedCount = root["results"][0]["curedCount"];
            Covid19Data.curedIncr = root["results"][0]["curedIncr"];
            Covid19Data.deadCount = root["results"][0]["deadCount"];
            Covid19Data.deadIncr = root["results"][0]["deadIncr"];
            Covid19Data.seriousCount = root["results"][0]["seriousCount"];
            Covid19Data.seriousIncr = root["results"][0]["seriousIncr"];

            Covid19Data.g_currentConfirmedCount = root["results"][0]["globalStatistics"]["currentConfirmedCount"];
            Covid19Data.g_currentConfirmedIncr = root["results"][0]["globalStatistics"]["currentConfirmedIncr"];
            Covid19Data.g_confirmedCount = root["results"][0]["globalStatistics"]["confirmedCount"];
            Covid19Data.g_confirmedIncr = root["results"][0]["globalStatistics"]["confirmedIncr"];
            Covid19Data.g_curedCount = root["results"][0]["globalStatistics"]["curedCount"];
            Covid19Data.g_curedIncr = root["results"][0]["globalStatistics"]["curedIncr"];
            Covid19Data.g_deadCount = root["results"][0]["globalStatistics"]["deadCount"];
            Covid19Data.g_deadIncr = root["results"][0]["globalStatistics"]["deadIncr"];
          }
          //@-天气数据
          else if(Data_Mode == 3)
          {
            strcpy(temp, root["result"]["city"]);
            sprintf(Juhe_WeatherData.weather_city, "%s", temp);
            strcpy(temp, root["result"]["realtime"]["temperature"]);
            sprintf(Juhe_WeatherData.weather_temperature, "%s", temp);
            strcpy(temp, root["result"]["future"][0]["weather"]);
            sprintf(Juhe_WeatherData.weather_info, "%s", temp);
            strcpy(temp, root["result"]["realtime"]["wid"]);
            sprintf(Juhe_WeatherData.weather_info_id, "%s", temp);
            strcpy(temp, root["result"]["realtime"]["humidity"]);
            sprintf(Juhe_WeatherData.weather_humidity, "%s", temp);
            strcpy(temp, root["result"]["realtime"]["direct"]);
            sprintf(Juhe_WeatherData.weather_direct, "%s", temp);
            strcpy(temp, root["result"]["realtime"]["power"]);
            sprintf(Juhe_WeatherData.weather_power, "%s", temp);
            strcpy(temp, root["result"]["realtime"]["aqi"]);
            sprintf(Juhe_WeatherData.weather_aqi, "%s", temp);

            strcpy(temp, root["result"]["future"][1]["temperature"]);
            sprintf(Juhe_WeatherData.weather_futureDay1_temperature, "%s", temp);
            strcpy(temp, root["result"]["future"][1]["weather"]);
            sprintf(Juhe_WeatherData.weather_futureDay1_info, "%s", temp);

            strcpy(temp, root["result"]["future"][2]["temperature"]);
            sprintf(Juhe_WeatherData.weather_futureDay2_temperature, "%s", temp);
            strcpy(temp, root["result"]["future"][2]["weather"]);
            sprintf(Juhe_WeatherData.weather_futureDay2_info, "%s", temp);

            strcpy(temp, root["result"]["future"][3]["temperature"]);
            sprintf(Juhe_WeatherData.weather_futureDay3_temperature, "%s", temp);
            strcpy(temp, root["result"]["future"][3]["weather"]);
            sprintf(Juhe_WeatherData.weather_futureDay3_info, "%s", temp);

            // Serial.print(Juhe_WeatherData.weather_city);
            // Serial.print(Juhe_WeatherData.weather_futureDay3_info); 
            // Serial.println(Juhe_WeatherData.weather_aqi);
          }
          //@-黄历信息
          else if(Data_Mode == 4)
          {
            strcpy(temp, root["result"]["data"]["lunarYear"]);
            sprintf(Juhe_HuangliData.lunarYear, "%s", temp);
            strcpy(temp, root["result"]["data"]["lunar"]);
            sprintf(Juhe_HuangliData.lunar, "%s", temp);
          }
        }
      }
    }
    else 
    {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
      //@-释放http资源
      http.end();
  }
  else 
  {
    //@-断开wifi链接
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    Serial.println("WiFi Disconnected");
  }
}

//@-非全局刷新
void EPD_ShowArea()
{
  int temp_data = 0; 

  Serial.println("----------DrawTimeUpdata-----------");
  epd_drv_dx.EPD4INC_Port_Reinit();      //SPI初始化
  epd_drv_dx.EPD4INC_HVEN();
  delay(2); 
  epd_drv_dx.EPD_CLK_EX();               //其中时钟
  epd_drv_dx.s1d135xx_set_epd_power(1);
  delay(2);


 //@1---------------------------------------------------------------------------------
  //@-显示日期-年-月
  epd_drv_dx.EPD_SetFount(FONT16);
  sprintf(buff_dx,"%d年 %2d月", dx_dateStruct.year, dx_dateStruct.month);
  epd_drv_dx.DrawUTF( 15 , 3, buff_dx, 1); 
  //@-显示日期
  epd_drv_dx.EPD_SetFount(FONT48_NUM);
  epd_drv_dx.DrawNum_DX( 20 , 17, dx_dateStruct.date, 1); 
  //@-显示星期
  epd_drv_dx.EPD_SetFount(FONT16);
  switch(dx_dateStruct.weekDay)
  {
    case 1: sprintf(buff_dx,"周一"); break;
    case 2: sprintf(buff_dx,"周二"); break;
    case 3: sprintf(buff_dx,"周三"); break;
    case 4: sprintf(buff_dx,"周四"); break;
    case 5: sprintf(buff_dx,"周五"); break;
    case 6: sprintf(buff_dx,"周六"); break;
    case 7: sprintf(buff_dx,"周日"); break;
    default: break;
  }
  epd_drv_dx.DrawUTF( 70, 45, buff_dx, 1); 

  //@-电量-图标
  user_area_dx.left = 205;    //x
  user_area_dx.top = 1;       //y
  user_area_dx.width = 16;
  user_area_dx.height = 16;
  epd_drv_dx.User_Img_Tran(user_area_dx.width, user_area_dx.height, gImage_battery, S1D13541_LD_IMG_1BPP,&user_area_dx,1);
  //@-电量
  temp_data = ((BAT_V-3.1)/0.012);
  epd_drv_dx.EPD_SetFount(FONT12);
  sprintf(buff_dx,"%2d", temp_data);
  epd_drv_dx.DrawUTF( 223 , 3, buff_dx, 1); 

  //@-显示时间
  epd_drv_dx.DrawTime(121, 17, dx_timeStruct.hours, dx_timeStruct.minutes, FONT48_NUM, 1);

  //@2---------------------------------------------------------------------------------


  //@-局部刷新
  user_area_dx.left = 0;   //x
  user_area_dx.top = 0;  //y
  user_area_dx.width = 239;
  user_area_dx.height = 105;

  //@-全部刷新
  // user_area_dx.top = 0;
  // user_area_dx.left = 0;
  // user_area_dx.width = 200;
  // user_area_dx.height = 400;


  // //@-显示json数据-12font能显示20个字
  // epd_drv_dx.EPD_SetFount(FONT16);
  // // sprintf(buff_dx,"1-%c", Hitokoto.hitokoto);
  // epd_drv_dx.DrawUTF( 0 , 260, NewsData[0].news_title, 1); 
  // epd_drv_dx.DrawUTF( 0 , 260+17, NewsData[1].news_title, 1); 
  // epd_drv_dx.DrawUTF( 0 , 260+34, NewsData[2].news_title, 1); 
  // // epd_drv_dx.DrawUTF( 0 , 260+51, NewsData[3].news_title, 1); 
  // // epd_drv_dx.DrawUTF( 0 , 260+68, NewsData[4].news_title, 1); 


  epd_drv_dx.EPD_UpdateUser(2, UPDATE_PARTIAL_AREA, &user_area_dx);

  //   epd_drv_dx.EPD_UpdateUser(1, UPDATE_FULL, NULL);

  delay(300);
  epd_drv_dx.EPD4INC_HVDISEN();
  epd_drv_dx.s1d135xx_set_power_state(PL_EPDC_SLEEP);
  epd_drv_dx.EPD_CLK_STOP();
}

//@-天气id图标
void weather_info_id_show(int id)
{
  switch(id)
  {
    case 0: weather_index_img_id_dx = gImage_weather_00; break;
    case 1: weather_index_img_id_dx = gImage_weather_01; break;
    case 2: weather_index_img_id_dx = gImage_weather_02; break;
    case 3: weather_index_img_id_dx = gImage_weather_03; break;
    case 4: weather_index_img_id_dx = gImage_weather_04; break;
    case 5: weather_index_img_id_dx = gImage_weather_05; break;
    case 6: weather_index_img_id_dx = gImage_weather_06; break;
    case 7: weather_index_img_id_dx = gImage_weather_07; break;
    case 8: weather_index_img_id_dx = gImage_weather_08; break;
    case 9: weather_index_img_id_dx = gImage_weather_09; break;

    case 10: weather_index_img_id_dx = gImage_weather_10; break;
    case 11: weather_index_img_id_dx = gImage_weather_11; break;
    case 12: weather_index_img_id_dx = gImage_weather_12; break;
    case 13: weather_index_img_id_dx = gImage_weather_13; break;
    case 14: weather_index_img_id_dx = gImage_weather_14; break;
    case 15: weather_index_img_id_dx = gImage_weather_15; break;
    case 16: weather_index_img_id_dx = gImage_weather_16; break;
    case 17: weather_index_img_id_dx = gImage_weather_17; break;
    case 18: weather_index_img_id_dx = gImage_weather_18; break;
    case 19: weather_index_img_id_dx = gImage_weather_19; break;

    case 20: weather_index_img_id_dx = gImage_weather_20; break;
    case 21: weather_index_img_id_dx = gImage_weather_21; break;
    case 22: weather_index_img_id_dx = gImage_weather_22; break;
    case 23: weather_index_img_id_dx = gImage_weather_23; break;
    case 24: weather_index_img_id_dx = gImage_weather_24; break;
    case 25: weather_index_img_id_dx = gImage_weather_25; break;
    case 26: weather_index_img_id_dx = gImage_weather_26; break;
    case 27: weather_index_img_id_dx = gImage_weather_27; break;
    case 28: weather_index_img_id_dx = gImage_weather_28; break;
    case 29: weather_index_img_id_dx = gImage_weather_29; break;

    case 30: weather_index_img_id_dx = gImage_weather_30; break;
    case 31: weather_index_img_id_dx = gImage_weather_31; break;
    case 53: weather_index_img_id_dx = gImage_weather_53; break;
    default: weather_index_img_id_dx = gImage_weather_00; break;
  }
}

//@-全刷新显示
void EPD_ShowMain()
{
  int temp_data = 0;

  Serial.println("----------Start_Mian_EPD()-----------");
  
  epd_drv_dx.EPD4INC_Port_init();
  epd_drv_dx.EPD4INC_Port_Reinit();      //SPI初始化
  epd_drv_dx.EPD_CLK_EX();

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
  //@-区域1分割线
  epd_drv_dx.Buf_DrawLine(0,Area1_Box_High,239,Area1_Box_High);   //@-横线 
  epd_drv_dx.Buf_DrawLine(Area1_Box_X ,0,Area1_Box_X ,Area1_Box_High);   //@-竖线
  //@-区域2分割线
  epd_drv_dx.Buf_DrawLine(0,Area2_Box_High,239,Area2_Box_High);   //@-横线 
  epd_drv_dx.Buf_DrawLine(Area2_Box_X ,Area1_Box_High,Area2_Box_X ,Area2_Box_High);   //@-竖线

  epd_drv_dx.EPD4INC_HVEN();
  delay(2);

  //@-全部显示缓存更新
  epd_drv_dx.Buf_UpdateFull(1);


  //@1---------------------------------------------------------------------------------
  //@-显示日期-年-月
  epd_drv_dx.EPD_SetFount(FONT16);
  sprintf(buff_dx,"%d年 %2d月", dx_dateStruct.year, dx_dateStruct.month);
  epd_drv_dx.DrawUTF( 15 , 3, buff_dx, 1); 
  //@-显示日期
  epd_drv_dx.EPD_SetFount(FONT48_NUM);
  epd_drv_dx.DrawNum_DX( 20 , 17, dx_dateStruct.date, 1); 
  //@-显示星期
  epd_drv_dx.EPD_SetFount(FONT16);
  switch(dx_dateStruct.weekDay)
  {
    case 1: sprintf(buff_dx,"周一"); break;
    case 2: sprintf(buff_dx,"周二"); break;
    case 3: sprintf(buff_dx,"周三"); break;
    case 4: sprintf(buff_dx,"周四"); break;
    case 5: sprintf(buff_dx,"周五"); break;
    case 6: sprintf(buff_dx,"周六"); break;
    case 7: sprintf(buff_dx,"周日"); break;
    default: break;
  }
  epd_drv_dx.DrawUTF( 70, 45, buff_dx, 1); 
  //@-显示黄历
  epd_drv_dx.EPD_SetFount(FONT16);
  sprintf(buff_dx,"%s", Juhe_HuangliData.lunarYear);
  // sprintf(buff_dx,"辛丑年");
  epd_drv_dx.DrawUTF( 35 , 65, buff_dx, 1); 
  sprintf(buff_dx,"%s", Juhe_HuangliData.lunar);
  // sprintf(buff_dx,"四月二九");
  epd_drv_dx.DrawUTF( 25 , 81, buff_dx, 1); 

  //@-地点-图标
  user_area_dx.left = 125;    //x
  user_area_dx.top = 1;       //y
  user_area_dx.width = 16;
  user_area_dx.height = 16;
  epd_drv_dx.User_Img_Tran(user_area_dx.width, user_area_dx.height, gImage_location, S1D13541_LD_IMG_1BPP,&user_area_dx,1);
  //@-地点-城市名
  epd_drv_dx.EPD_SetFount(FONT16);
  sprintf(buff_dx,"%s", Juhe_WeatherData.weather_city);
  // sprintf(buff_dx,"杭州");
  epd_drv_dx.DrawUTF( 142 , 3, buff_dx, 1); 
  //@-电量-图标
  user_area_dx.left = 205;    //x
  user_area_dx.top = 1;       //y
  user_area_dx.width = 16;
  user_area_dx.height = 16;
  epd_drv_dx.User_Img_Tran(user_area_dx.width, user_area_dx.height, gImage_battery, S1D13541_LD_IMG_1BPP,&user_area_dx,1);
  //@-电量
  temp_data = ((BAT_V-3.1)/0.012);
  epd_drv_dx.EPD_SetFount(FONT12);
  sprintf(buff_dx,"%2d", temp_data);
  epd_drv_dx.DrawUTF( 223 , 3, buff_dx, 1); 

  //@-显示时间
  epd_drv_dx.DrawTime(121, 17, dx_timeStruct.hours, dx_timeStruct.minutes, FONT48_NUM, 1);

  //@-天气概述-最大8个汉字
  epd_drv_dx.EPD_SetFount(FONT12);
  sprintf(buff_dx,"%s", Juhe_WeatherData.weather_info);
  // sprintf(buff_dx,"大暴雨到特大暴雨");
  epd_drv_dx.DrawUTF( 154 ,67, buff_dx, 1); 
  sprintf(buff_dx,"%s℃-%s%s", Juhe_WeatherData.weather_temperature, Juhe_WeatherData.weather_direct, Juhe_WeatherData.weather_power);
  epd_drv_dx.DrawUTF( 154 , 82, buff_dx, 1); 
  //@-显示天气图标
  const uint8_t* p_img;
  sprintf(buff_dx,"%s", Juhe_WeatherData.weather_info_id);
  int info_id = String(buff_dx).toInt();
  weather_info_id_show(info_id);
  user_area_dx.left = 121;    //x
  user_area_dx.top = 65;       //y
  user_area_dx.width = 32;
  user_area_dx.height = 32;
  epd_drv_dx.User_Img_Tran(user_area_dx.width, user_area_dx.height, weather_index_img_id_dx, S1D13541_LD_IMG_1BPP,&user_area_dx,1);
  
  //@2---------------------------------------------------------------------------------






  // //@-测试数据-------------
  // user_area_dx.top = 0;
  // // user_area_dx.left = 100;   
  // user_area_dx.left = 0; 
  // user_area_dx.width = 240;
  // user_area_dx.height = 127;

  // // S1D13541_LD_IMG_1BPP  ---->  单色
  // // S1D13541_LD_IMG_2BPP  ---->  4灰
  // // S1D13541_LD_IMG_4BPP  ---->  16灰  -->jpg照片
  // epd_drv_dx.User_Img_Tran(user_area_dx.width, user_area_dx.height, gImage_EPD_Logo_41W53H,S1D13541_LD_IMG_1BPP,&user_area_dx,1);

  // //刷新背景
  // // epd_drv.EPD_Update_Full(12000, S1D13541_LD_IMG_1BPP, gImage_InitPage);
  // //写入ID
  // // sprintf(buff_dx,"世界人民大团结万岁 =%d",good);
  // // epd_drv_dx.EPD_SetFount(FONT16);
  // // epd_drv_dx.DrawUTF( 35 , 180, buff_dx, 1);    //显示设备序列号

  // //@-设置显示数据刷新区域
  // user_area_dx.top = 180;
  // user_area_dx.left = 35;
  // user_area_dx.width = 200;
  // user_area_dx.height = 50;
  
  // epd_drv_dx.EPD_SetFount(FONT16);
  // sprintf(buff_dx,"温度:%0.1f  湿度:%0.1f", sht30.cTemp, sht30.humidity);
  // epd_drv_dx.DrawUTF( 35 , 180, buff_dx, 1);    
  // sprintf(buff_dx,"时间:%2d:%2d V:%0.1f", dx_timeStruct.hours, dx_timeStruct.minutes, BAT_V);
  // epd_drv_dx.DrawUTF( 35 , 200, buff_dx, 1);  


  // //@-显示json数据-12font能显示20个字
  // // epd_drv_dx.EPD_SetFount(FONT16);
  // // sprintf(buff_dx,"1-%c", Hitokoto.hitokoto);
  // epd_drv_dx.DrawUTF( 0 , 260, NewsData[0].news_title, 1); 
  // epd_drv_dx.DrawUTF( 0 , 260+17, NewsData[1].news_title, 1); 
  // epd_drv_dx.DrawUTF( 0 , 260+34, NewsData[2].news_title, 1); 
  // // epd_drv_dx.DrawUTF( 0 , 260+51, NewsData[3].news_title, 1); 
  // // epd_drv_dx.DrawUTF( 0 , 260+68, NewsData[4].news_title, 1); 


  epd_drv_dx.EPD4INC_HVEN();
  delay(10);
  epd_drv_dx.EPD_UpdateUser(1, UPDATE_FULL, NULL);
 
  delay(900);
 
  epd_drv_dx.EPD4INC_HVDISEN();
  epd_drv_dx.s1d135xx_set_power_state(PL_EPDC_SLEEP);
  epd_drv_dx.EPD_CLK_STOP();
}


//@-连接wifi
void WIFI_Connect()
{
  // Connect to Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    wifi_connect_tick = wifi_connect_tick + 1;
    if(wifi_connect_tick > 8)
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


  //@-wifi连接成功-每天23:01-23:03进行网络对时
  if((wifi_connect_flag == 1) && (dx_timeStruct.hours == 23) && 
    ((dx_timeStruct.minutes == 5)||(dx_timeStruct.minutes == 15)||(dx_timeStruct.minutes == 20)))
  {
    // Init and get the time
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    printLocalTime();
  }

  // //disconnect WiFi as it's no longer needed
  // WiFi.disconnect(true);
  // WiFi.mode(WIFI_OFF);
}

//@-打印实时时间
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

//@-设置RTC
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


//@-读取RTC
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


//@-主循环
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
  }
  
  delay(10);
}
