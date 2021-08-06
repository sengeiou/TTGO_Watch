
#include <SPIFFS.h>
#include <WiFi.h>
#include <WiFiMulti.h>
// #define ARDUINOJSON_DECODE_UNICODE 1
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

#define USE_BLE 0

#if USE_BLE
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#endif

//const char* ssid = "8879";
//const char* password = "blackbug381";

//20210520-该版本EPD无光感传感器
// #define XDOT  240
// #define YDOT  400

#define ESP32

#define Software_Version "20210618 V1.02"

//@-IO定义
#define Button0_PIN   0  
#define Button1_PIN  27  
#define Button2_PIN  26
#define Charge_PIN   18
#define BAT_EN_PIN   23
#define BAT_V_PIN    35
#define BAT_AD_Solution 12

#define uS_TO_S_FACTOR 1000000ULL  /* Conversion factor for micro seconds to seconds */

//@-GUI界面配置---
//---------------------------------------------
#define Area1_Box_X      115
#define Area1_Box_High   105
#define Area2_Box_X      45
#define Area2_Box_High   Area1_Box_High + 50   //155
// #define Area3_Box_X      120
#define Area3_Box_High   Area2_Box_High + 50   //205
#define Area4_Box_X      120
#define Area4_Box_High   Area3_Box_High + 35   //240
// #define Area5_Box_X      120
#define Area5_Box_High   Area4_Box_High + 142  //382


#define SPIFFS_Save_Def          0
#define SPIFFS_Save_Data         1
#define SPIFFS_Save_Wifi_Switch  2

#define WIFI_Index_1             0
#define WIFI_Index_2             1



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
const String ssid1     = "wuyiyi";
const String password1 = "dingxiao";
const String ssid2 = "DX_JS";  
const String password2 = "dingxiao";

// const char *ssid3 = "DX_JS";  
// const char *password3 = "dingxiao";

String ssid;
String password;
int wifi_connect_tick = 0;
int wifi_connect_time = 60;  //默认wifi连接时间


//@-网络授时
// const char* ntpServer = "pool.ntp.org";   ----->ok
// const char* ntpServer = "cn.pool.ntp.org";  ---->OK
// const char* ntpServer = "time.pool.aliyun.com";   ---->OK
const char* ntpServer = "ntp.aliyun.com";

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
int Charge_State = 0;           //@-充电接入状态
bool Key_Flag = false;
int Config_Set_Tick = 0;        //@-进入设备配置计数器

//@-电池电压
float BAT_V = 0;

//@-ESP32唤醒
esp_sleep_wakeup_cause_t wakeup_reason;

//@-RTC数据存储区
RTC_DATA_ATTR int bootCount = 0;
// RTC_DATA_ATTR int Dev_Wifi_Index = 0;

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

String serverName_sinaNews = "http://interface.sina.cn/dfz/outside/wap/news/list.d.html?col=56261&show_num=8";  //@-新浪综合新闻7条
// String serverName_covid = "http://route.showapi.com/2217-2?showapi_appid=672306&showapi_sign=7c49550af6554658a9005f3014bc6f2b";
String serverName_covid = "https://lab.isaaclin.cn/nCoV/api/overall";
// String serverName_covid2 = "http://81.68.90.103/nCoV/api/overall";
String serverName_weather = "http://apis.juhe.cn/simpleWeather/query?&key=2b636957c5b1b630bf13194d76d86801";
String serverName_huangli = "http://v.juhe.cn/calendar/day?key=9774f2f31c8349cbab916eaf11d849db"; //date=2021-6-9
String serverName_stock_sh = "http://web.juhe.cn:8080/finance/stock/hs?gid=&type=0&key=4dd25417ff9fb7cbab4791e60899d9a8"; //上证指数
String serverName_stock_sz = "http://web.juhe.cn:8080/finance/stock/hs?gid=&type=1&key=4dd25417ff9fb7cbab4791e60899d9a8"; //深圳指数

String serverName_weatherAQI = "http://route.showapi.com/104-29?showapi_appid=672306&showapi_sign=7c49550af6554658a9005f3014bc6f2b&city=杭州";  //空气质量-万维易源
String serverName_huangli1 = "http://route.showapi.com/856-2?showapi_appid=672306&showapi_sign=7c49550af6554658a9005f3014bc6f2b";

//@-数据不是很准确
// https://coronavirus-tracker-api.herokuapp.com/v2/locations/67  中国
// https://coronavirus-tracker-api.herokuapp.com/v2/locations?&id=67 中国河北省

// 

//@-万维易源数据PM2.5空气质量指数
// String serverName_stock_sz = "http://route.showapi.com/104-29?showapi_appid=672306&showapi_sign=7c49550af6554658a9005f3014bc6f2b&city=杭州";
//@-万维易源数据疫情数据
// String serverName_covid = http://route.showapi.com/2217-2?showapi_appid=672306&showapi_sign=7c49550af6554658a9005f3014bc6f2b
//@-万维易源数据黄历数据
// String serverName_covid = "http://route.showapi.com/856-2?showapi_appid=672306&showapi_sign=7c49550af6554658a9005f3014bc6f2b&ymd=20210618";

// D:\Work\Django\blog\home\dx\sites\dx1023.com\django_blog\blog
String serverName_covid1 = "http://www.dx1023.com/dxjson/";



// struct SpiRamAllocator {
//   void* allocate(size_t size) {
//     return heap_caps_malloc(size, MALLOC_CAP_SPIRAM);
//   }

//   void deallocate(void* pointer) {
//     heap_caps_free(pointer);
//   }

//   void* reallocate(void* ptr, size_t new_size) {
//     return heap_caps_realloc(ptr, new_size, MALLOC_CAP_SPIRAM);
//   }
// };

// using SpiRamJsonDocument = BasicJsonDocument<SpiRamAllocator>;



//@-新闻数据结构体
typedef struct {
  char news_title[256];
  // char news_author_name[64];
} NewsData_t;
NewsData_t NewsData[8];

//@-Covid-19数据结构体
typedef struct {
  //@-对应serverName_covid1的json格式数据
  #if 0
  // int currentConfirmedCount;  //@-国内当前确诊数  
  // int currentConfirmedIncr;   //@-国内当前新增确诊数 
  // long confirmedCount;        //@-国内累计确诊数
  // int  confirmedIncr;         //@-国内新增总确诊数
  // int suspectedCount;         //@-国内疑似数
  // int suspectedIncr;          //@-国内新增疑似数
  // long curedCount;            //@-国内治愈数
  // int  curedIncr;             //@-国内新增治愈术
  // int deadCount;              //@-国内死亡数       
  // int deadIncr;               //@-国内新增死亡数    
  // int seriousCount;           //@-国内重症数
  // int seriousIncr;            //@-国内新增重症数

  // long g_currentConfirmedCount; //@-全球当前确诊数
  // long g_currentConfirmedIncr;  //@-全球当前新增确诊数
  // long g_confirmedCount;        //@-全球累计确诊数
  // long g_confirmedIncr;         //@-全球新增总确诊数
  // long g_curedCount;            //@-全球治愈数
  // long g_curedIncr;             //@-全球新增治愈术
  // long g_deadCount;             //@-全球死亡数
  // long g_deadIncr;              //@-全球新增死亡数
  #endif

  //@-对应serverName_covid的json格式数据
  int confirmedNum;            //@-现存确诊
  int confirmedIncr;           //@-现存确诊新增
  int externalConfirmedNum;    //@-境外输入
  int externalConfirmedIncr;   //@-境外输入新增
  int asymptomaticNum;         //@-无症状
  int asymptomaticIncr;        //@-无症状新增
  int deadNum;                 //@-累计死亡
  int deadIncr;                //@-累计死亡新增

  char dx_string[3500];  

} Covid19Data_t;
Covid19Data_t Covid19Data;

unsigned char json_gImage_play_W56H59[1000];
int StringCount = 0;

// unsigned char json_gImage_play_W56H59[3480];

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
  char weather_futureDay1_info[64];               //@-未来1天天气情况
  char weather_futureDay1_date[64];               //@-未来1天日期

  char weather_futureDay2_temperature[72];        //@-未来2天天气温度范围
  char weather_futureDay2_info[64];               //@-未来2天天气情况
  char weather_futureDay2_date[64];               //@-未来2天日期

  char weather_futureDay3_temperature[72];        //@-未来3天天气温度范围
  char weather_futureDay3_info[64];               //@-未来3天天气情况
  char weather_futureDay3_date[64];               //@-未来3天日期
  
} Juhe_WeatherData_t;
Juhe_WeatherData_t Juhe_WeatherData;

//@-聚合黄历数据
typedef struct {
  char lunarYear[32];                //@-农历年信息 
  char lunar[40];                    //@-农历日信息 
} Juhe_HuangliData_t;
Juhe_HuangliData_t Juhe_HuangliData;

//@-聚合股票数据
typedef struct {
  char  sh_stock_value[72];                //@-上证指数
  char  sh_stock_per[40];                  //@-上证指数涨跌百分比
  char  sz_stock_value[72];                //@-深圳指数
  char  sz_stock_per[40];                  //@-上证指数涨跌百分比
} Juhe_StockData_t;
Juhe_StockData_t Juhe_StockData;

//@-万维数源黄历数据
typedef struct {
  char  nongli[72];                  //@-农历信息
  char  ganzhi[72];                  //@-干子信息
  char  qixiang[72];                 //@-气象信息
  char  jieri[72];                   //@-节假日
  char  jieqi24[72];                 //@-24节气
} WanWei_HuangliData_t;
WanWei_HuangliData_t WanWei_HuangliData;

//@-万维数源天气质量数据
typedef struct {
  char  quality[32];                  //@-天气资料描述
  char  pm2_5[32];                    //@-pm2.5信息
  char  pm10[32];                     //@-pm10信息
  char  so2[32];                      //@-二氧化硫一小时平均，μg/m³
  char  o3[32];                       //@-臭氧1小时平均，μg/m³
  char  co[32];                       //@-一氧化碳1小时平均，mg/m3
  char  no2[32];                      //@-二氧化氮1小时平均，μg/m3
} WanWei_WeatherData_t;
WanWei_WeatherData_t WanWei_WeatherData;

//@-SPIFFS JSON数据结构体
typedef struct {

    String Json_name;
    String Json_location;

    String Json_SSID1;
    String Json_Pass1;

    String Json_SSID2;
    String Json_Pass2;

    String Json_App_KEY1;

    int Json_Wifi_Index;

    int Json_First_Run;   //@-设备第一次运行标志 1:是  0:不是

} SPIFFS_JSONData_t;
SPIFFS_JSONData_t SPIFFS_JSONData_Read;
SPIFFS_JSONData_t SPIFFS_JSONData_Save;

//@-天气id对应图标
const uint8_t* weather_index_img_id_dx = gImage_weather_00;

//@-设备运行模式
int EPD_Dev_RunMode = 0;   //0:正常模式  1:配置模式  2:蓝牙BLE模式

//@-WEB服务器
AsyncWebServer server_dx(80);
const char* PARAM_INPUT_1 = "ssid1";   //@-html文件中input属性的name值
const char* PARAM_INPUT_2 = "passkey1";
const char* PARAM_INPUT_3 = "ssid2";
const char* PARAM_INPUT_4 = "passkey2";
const char* PARAM_INPUT_5 = "city";
//*********SSID and Pass for AP**************//
const char* ssidAPConfig = "dx_epd";
const char* passAPConfig = "dingxiao";

int Dev_ConfigSuc_Flag = 0;  //@-设备参数配置成功标志
int Dev_ConfigSecond = 8;    //@-设备参数配置成功秒倒计时
//------------------------------------------
//@-内容显示标志
int Display_Weather_Flag = 0;
int Display_Covid_Flag =   0;
int Display_Stock_Flag =   0;
int Display_News_Flag =    0;
int Display_Lunar_Flag =   0;
int Display_WanWeiLunar_Flag =   0;
int Display_WanWeiWeather_Flag =   0;

//-------------------------------------------------------
#if USE_BLE
//@-BLE相关
// See the following for generating UUIDs:
// https://www.uuidgenerator.net/
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint32_t value = 0;
uint8_t txValue = 0;
char  BLE_Msg[256];                      //BLE接收缓存区
char  BLE_SendMsg[256];                  //BLE发送缓存区
#endif




//@-加载SPIFFS文件系统中的json文件
void Load_Config()
{
  File file = SPIFFS.open("/config.json", FILE_READ);
  if (!file)
  {
    Serial.println("No Config-> white define");
    Save_Set_Data(SPIFFS_Save_Def, 0, 0);
  } 
  else
  {
    size_t size = file.size();

    Serial.print("--------------json file size:");
    Serial.println(size);
    if(size == 0)
    {
      Serial.println("Config file empty-> white define");
      Save_Set_Data(SPIFFS_Save_Def, 0, 0);
    }
    else
    {
      char buff_dx[1024];
      file.readBytes(buff_dx, size);
      DynamicJsonDocument doc(1024);
      //@-序列化JSON数据
      DeserializationError error = deserializeJson(doc, buff_dx);
      if (error) 
      {
        Serial.println("JSON parsing failed!");
      } 
      else
      {
        // get the JsonObject in the JsonDocument
        JsonObject root = doc.as<JsonObject>(); 

        // Serial.print("ssid1:");
        // Serial.println(root["Json_SSID1"].as<String>());
        // Serial.print("pass1:");
        // Serial.println(root["Json_Pass1"].as<String>());
        // Serial.print("ssid2:");
        // Serial.println(root["Json_SSID2"].as<String>());
        // Serial.print("pass2:");
        // Serial.println(root["Json_Pass2"].as<String>());

        //@-读取配置参数
        SPIFFS_JSONData_Read.Json_name = root["Json_name"].as<String>();
        SPIFFS_JSONData_Read.Json_location = root["Json_location"].as<String>();
        SPIFFS_JSONData_Read.Json_SSID1 = root["Json_SSID1"].as<String>();
        SPIFFS_JSONData_Read.Json_Pass1 = root["Json_Pass1"].as<String>();
        SPIFFS_JSONData_Read.Json_SSID2 = root["Json_SSID2"].as<String>();
        SPIFFS_JSONData_Read.Json_Pass2 = root["Json_Pass2"].as<String>();
        SPIFFS_JSONData_Read.Json_App_KEY1 = root["Json_App_KEY1"].as<String>();
        SPIFFS_JSONData_Read.Json_Wifi_Index = root["Json_Wifi_Index"];
        SPIFFS_JSONData_Read.Json_First_Run = root["Json_First_Run"];

        //@-设备名
        Serial.print("Json_name:");
        Serial.println(SPIFFS_JSONData_Read.Json_name);
        //@-地点
        Serial.print("Json_location:");
        Serial.println(SPIFFS_JSONData_Read.Json_location);

        //@-wifi1信息
        Serial.print("Json_SSID1:");
        Serial.println(SPIFFS_JSONData_Read.Json_SSID1);
        Serial.print("Json_Pass1:");
        Serial.println(SPIFFS_JSONData_Read.Json_Pass1);

        //@-wifi2信息
        Serial.print("Json_SSID2:");
        Serial.println(SPIFFS_JSONData_Read.Json_SSID2);
        Serial.print("Json_Pass2:");
        Serial.println(SPIFFS_JSONData_Read.Json_Pass2);

        //@-Json_App_KEY1
        Serial.print("Json_App_KEY1:");
        Serial.println(SPIFFS_JSONData_Read.Json_App_KEY1);

        //@-wifi连接index
        Serial.print("wifi index:");
        Serial.println(SPIFFS_JSONData_Read.Json_Wifi_Index);
        //@-设备第一次运行标志
        Serial.print("first run:");
        Serial.println(SPIFFS_JSONData_Read.Json_First_Run);
      }

    }
  }

}

//@-保存参数到SPIFFS文件系统中的json文件
void Save_Set_Data(int mode, int wifi_index, int first_run)  //@-mode  0:初始化模式   1:用户模式
{
  // StaticJsonDocument<500> doc;
  DynamicJsonDocument doc(1024);

  //@-初始化模式
  if(mode == SPIFFS_Save_Def)
  {
    doc["Json_name"] = "丁霄";
    doc["Json_location"] = "杭州";

    doc["Json_SSID1"] = "wifi1";
    doc["Json_Pass1"] = "pass1";

    doc["Json_SSID2"] = "wifi2";
    doc["Json_Pass2"] = "pass2";

    doc["Json_App_KEY1"] = "key1";

    doc["Json_Wifi_Index"] = wifi_index;

    doc["Json_First_Run"] = first_run;  //调试

  }
  else if (mode == SPIFFS_Save_Data)
  {
    doc["Json_name"] = SPIFFS_JSONData_Save.Json_name;
    doc["Json_location"] = SPIFFS_JSONData_Save.Json_location;

    doc["Json_SSID1"] = SPIFFS_JSONData_Save.Json_SSID1;
    doc["Json_Pass1"] = SPIFFS_JSONData_Save.Json_Pass1;

    doc["Json_SSID2"] = SPIFFS_JSONData_Save.Json_SSID2;
    doc["Json_Pass2"] = SPIFFS_JSONData_Save.Json_Pass2;

    doc["Json_App_KEY1"] = SPIFFS_JSONData_Save.Json_App_KEY1;

    doc["Json_Wifi_Index"] = wifi_index;

    doc["Json_First_Run"] = first_run;
  }
  else if(mode == SPIFFS_Save_Wifi_Switch)
  {
    doc["Json_name"] = SPIFFS_JSONData_Read.Json_name;
    doc["Json_location"] = SPIFFS_JSONData_Read.Json_location;

    doc["Json_SSID1"] = SPIFFS_JSONData_Read.Json_SSID1;
    doc["Json_Pass1"] = SPIFFS_JSONData_Read.Json_Pass1;

    doc["Json_SSID2"] = SPIFFS_JSONData_Read.Json_SSID2;
    doc["Json_Pass2"] = SPIFFS_JSONData_Read.Json_Pass2;

    doc["Json_App_KEY1"] = SPIFFS_JSONData_Read.Json_App_KEY1;

    doc["Json_Wifi_Index"] = wifi_index;

    doc["Json_First_Run"] = SPIFFS_JSONData_Read.Json_First_Run;
  }

  File file_config = SPIFFS.open("/config.json", FILE_WRITE);
 
  if (!file_config) {
    Serial.println("There was an error opening the file for writing");
    return;
  }

  serializeJson(doc, file_config);
}

//@-http 服务器没有找到页面
void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "dx Not found");
}

//@-配置
void setup()
{
  char temp_str[256];

  psramInit();

  //@-初始化串口
  Serial.begin(115200);

  Serial.println((String)"Memory available in PSRAM : " +ESP.getFreePsram());

  if(bootCount > 600)
  bootCount = 0;
  //Increment boot number and print it every reboot
  bootCount = bootCount + 1;
  Serial.println("Boot number: " + String(bootCount));

  //@-文件系统启动
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  //@-加载config文件
  Load_Config();

  //@-选择wifi
  // if(SPIFFS_JSONData_Read.Json_Wifi_Index == WIFI_Index_1)
  // {
  //   // ssid = ssid1;
  //   // password = password1;
  //   ssid = SPIFFS_JSONData_Read.Json_SSID1;
  //   password = SPIFFS_JSONData_Read.Json_Pass1;
  // }
  // else if(SPIFFS_JSONData_Read.Json_Wifi_Index == WIFI_Index_2)
  // {
  //   // ssid = ssid2;
  //   // password = password2;
  //   ssid = SPIFFS_JSONData_Read.Json_SSID2;
  //   password = SPIFFS_JSONData_Read.Json_Pass2;
  // }
  ssid = SPIFFS_JSONData_Read.Json_SSID1;
  password = SPIFFS_JSONData_Read.Json_Pass1;

  //ESP32启动方式
  wakeup_reason = esp_sleep_get_wakeup_cause();
  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); wifi_connect_time = 8;break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
  }

  // chipid=ESP.getEfuseMac();

  //@-注册按键
  pinMode(Button0_PIN, INPUT|PULLUP);
  pinMode(Button1_PIN, INPUT|PULLUP);
  pinMode(Button2_PIN, INPUT|PULLUP);
  pinMode(Charge_PIN, INPUT|PULLUP);

  //@-查询是否需要进入配置模式
  Config_Set_Tick = 0;
  if(wakeup_reason == ESP_SLEEP_WAKEUP_EXT0)
  {
      Serial.print("Config");
      while (1)
      {
        //@-读取键盘值
        Button0_State = digitalRead(Button0_PIN);
        if((Button0_State == LOW))
        {
          Config_Set_Tick = Config_Set_Tick + 1;
          Serial.print(".");
          if(Config_Set_Tick > 8)
          {
            EPD_Dev_RunMode = 1;
            break;
          }
        }
        delay(500);
      }
  }

  #if USE_BLE
  //@-查询是否进入蓝牙BLE模式
  if(wakeup_reason == ESP_SLEEP_WAKEUP_EXT1)
  {
    Serial.println("BLE Mode Start----------------->");
    Dev_BLEInit();
  }
  #endif

  //@-读取USB充电状态
  Charge_State = digitalRead(Charge_PIN);
  Serial.print("USB Charge:");
  Serial.println(Charge_State);

  //@-AD采样检测
  pinMode(BAT_EN_PIN, OUTPUT);
  digitalWrite(BAT_EN_PIN, HIGH);
  analogReadResolution(BAT_AD_Solution);
  pinMode(BAT_V_PIN,INPUT);

  //@-配置I2C总线
  Wire.begin(21, 22);

  //@-获得数据
  delay(5);
  sht30.get();  //获得温湿度数据
  delay(5);
  ReadRTC();

  //@-正常运行模式
  if(EPD_Dev_RunMode == 0)
  {
    #if 1
    //@-配置wifi连接-每5min检测wifi连接-并获取json数据
    if((dx_timeStruct.minutes == 5)||(dx_timeStruct.minutes == 10)||(dx_timeStruct.minutes == 15)||
      (dx_timeStruct.minutes == 20)||(dx_timeStruct.minutes == 25)||(dx_timeStruct.minutes == 30)||
      (dx_timeStruct.minutes == 35)||(dx_timeStruct.minutes == 40)||(dx_timeStruct.minutes == 45)||
      (dx_timeStruct.minutes == 50)||(dx_timeStruct.minutes == 55)||(dx_timeStruct.minutes == 0) ||
      (bootCount == 1))
    {
      //@-连接wifi
      WIFI_Connect();

      //@-每5min获取Sina综合新闻json数据
      WIFI_Get_JsonInfo(serverName_sinaNews, 1, "新浪新闻");

      //@-获取Covid数据-每天7，15点获取1次
      if((((dx_timeStruct.hours == 7)||(dx_timeStruct.hours == 15))&&(dx_timeStruct.minutes == 0))||(bootCount == 1))
      {
        WIFI_Get_JsonInfo(serverName_covid1, 2, "Covid-19");
      }

      #if 0
      //@-工作时间每天4次获取天气数据-聚合天气
      if((((dx_timeStruct.hours == 7)||(dx_timeStruct.hours == 10)||(dx_timeStruct.hours == 13)||(dx_timeStruct.hours == 16))&&(dx_timeStruct.minutes == 0))||(bootCount == 1))
      {
        
        sprintf(temp_str, "&city=%s", SPIFFS_JSONData_Read.Json_location);
        String serverName_weather1 = serverName_weather + String(temp_str);
        // Serial.println(serverName_weather1);
        WIFI_Get_JsonInfo(serverName_weather1, 3, "聚合天气");
      }

      // //@-工作时间每天4次获取天气数据-万维气象
      // if((((dx_timeStruct.hours == 7)||(dx_timeStruct.hours == 10)||(dx_timeStruct.hours == 13)||(dx_timeStruct.hours == 16))&&(dx_timeStruct.minutes == 0))||(bootCount == 1))
      // {
      //   WIFI_Get_JsonInfo(serverName_weatherAQI, 7, "万维气象");
      // }

      //@-获得黄历数据-每天凌晨1点获取1次
      if(((dx_timeStruct.hours == 0)&&(dx_timeStruct.minutes == 1))||(bootCount == 1))
      {
        sprintf(temp_str, "&date=%d-%d-%d", dx_dateStruct.year, dx_dateStruct.month, dx_dateStruct.date);
        String huangliData = serverName_huangli + String(temp_str);
        WIFI_Get_JsonInfo(huangliData, 4, "聚合黄历");
      }

      // //@-获得黄历数据-每天凌晨1点获取0:1次
      // if(((dx_timeStruct.hours == 0)&&(dx_timeStruct.minutes == 1))||(bootCount == 1))
      // {
      //   sprintf(temp_str, "&ymd=%04d%02d%02d", dx_dateStruct.year, dx_dateStruct.month, dx_dateStruct.date);
      //   String huangliData = serverName_huangli1 + String(temp_str);
      //   WIFI_Get_JsonInfo(huangliData, 8, "万维农历");
      // }

      //@-获得上证指数-工作日的9/10/11/13/14/15:30获取6次
      if(((dx_dateStruct.weekDay == 1)||(dx_dateStruct.weekDay == 2)||(dx_dateStruct.weekDay == 3)||(dx_dateStruct.weekDay == 4)||(dx_dateStruct.weekDay == 5))||(bootCount == 1))
      {
        //@-交易时间刷新
        if((((dx_timeStruct.hours == 9)||(dx_timeStruct.hours == 10)||(dx_timeStruct.hours == 11)||(dx_timeStruct.hours == 13)||(dx_timeStruct.hours == 14)||(dx_timeStruct.hours == 15))&&(dx_timeStruct.minutes == 35))||(bootCount == 1))
        WIFI_Get_JsonInfo(serverName_stock_sh, 5, "聚合股票-上海");
      }
      // //@-获得深圳指数-工作日的9/10/11/13/14/15:30获取6次
      if(((dx_dateStruct.weekDay == 1)||(dx_dateStruct.weekDay == 2)||(dx_dateStruct.weekDay == 3)||(dx_dateStruct.weekDay == 4)||(dx_dateStruct.weekDay == 5))||(bootCount == 1))
      {
        //@-交易时间刷新
        if((((dx_timeStruct.hours == 9)||(dx_timeStruct.hours == 10)||(dx_timeStruct.hours == 11)||(dx_timeStruct.hours == 13)||(dx_timeStruct.hours == 14)||(dx_timeStruct.hours == 15))&&(dx_timeStruct.minutes == 35))||(bootCount == 1))
        WIFI_Get_JsonInfo(serverName_stock_sz, 6, "聚合股票-深圳");
      }
      #endif

      //@-断开wifi链接
      WiFi.disconnect(true);
      WiFi.mode(WIFI_OFF);
      Serial.println("WiFi Disconnected");
    }
    #endif

    //@-读取AD
    BAT_V = analogRead(BAT_V_PIN)/560.1;

    //@-读取RTC数据
    delay(2);
    ReadRTC();

    //@-显示内容
    if((wakeup_reason == 0)||(bootCount == 1))
    {
      EPD_ShowMain();
      // EPD_ShowConfigSuc();
      // EPD_ShowCofigSuc_Second(6);
    }
    if((wakeup_reason == ESP_SLEEP_WAKEUP_TIMER) && (bootCount != 1))
    {
      EPD_ShowArea();
    }

    //保证屏幕RST引脚高电平
    rtc_gpio_pullup_en(GPIO_NUM_4);
    delay(2);
    rtc_gpio_pulldown_dis(GPIO_NUM_4);   

    //@-唤醒设置 
    mask|=  1ull << Button2_PIN;
  //mask|=  1ull << 27;
    //@-配置外部按键唤醒
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_0,0);  //KEY0唤醒RTC_IO
    esp_sleep_enable_ext1_wakeup(mask,ESP_EXT1_WAKEUP_ALL_LOW);   //KEY2唤醒RTC_CNTL
    //@-配置定时器唤醒
    esp_sleep_enable_timer_wakeup(50 * uS_TO_S_FACTOR);
    Serial.println("----------Sleep Now-----------");
    esp_deep_sleep_start();
  }
  //@-配置模式
  else if(EPD_Dev_RunMode == 1)
  {
    //@-配置成wifi路由模式
    WiFi.mode(WIFI_AP);
    Serial.println(WiFi.softAP(ssidAPConfig,passAPConfig) ? "soft-AP setup": "Failed to connect");
    delay(100);
    Serial.println(WiFi.softAPConfig( IPAddress(192,168,1,4),IPAddress(192,168,1,254), IPAddress(255,255,255,0))? "Configuring Soft AP" : "Error in Configuration");      
    Serial.println(WiFi.softAPIP());

    //@-HTML文件为SPIFFS文件系统
    server_dx.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
    {
      request->send(SPIFFS, "/Test.html", "text/html");
    });

    //@-api设置页面
    server_dx.on("/set", HTTP_GET, [](AsyncWebServerRequest *request)
    {
      request->send(SPIFFS, "/Set.html", "text/html");
    });

    //@-从SPIFFS文件系统中获得图片
    server_dx.on("/logo", HTTP_GET, [](AsyncWebServerRequest *request)
    {
      request->send(SPIFFS, "/logo.png", "image/png");
    });

    //@-挂载http get 操作
    server_dx.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) 
    {
      //@-获得参数
      SPIFFS_JSONData_Save.Json_SSID1 = request->getParam(PARAM_INPUT_1)->value();
      SPIFFS_JSONData_Save.Json_Pass1 = request->getParam(PARAM_INPUT_2)->value();
      SPIFFS_JSONData_Save.Json_SSID2 = request->getParam(PARAM_INPUT_3)->value();
      SPIFFS_JSONData_Save.Json_Pass2 = request->getParam(PARAM_INPUT_4)->value();
      SPIFFS_JSONData_Save.Json_location = request->getParam(PARAM_INPUT_5)->value();

      Serial.println(SPIFFS_JSONData_Save.Json_location);
      Serial.println(SPIFFS_JSONData_Save.Json_SSID1);
      Serial.println(SPIFFS_JSONData_Save.Json_Pass1);
      Serial.println(SPIFFS_JSONData_Save.Json_SSID2);
      Serial.println(SPIFFS_JSONData_Save.Json_Pass2);

      //@-debug
      SPIFFS_JSONData_Save.Json_name = "丁霄";
      // SPIFFS_JSONData_Save.Json_location = "杭州";
      SPIFFS_JSONData_Save.Json_App_KEY1 = "key1";

      //@-SPIFFS写入参数
      Save_Set_Data(SPIFFS_Save_Data, SPIFFS_JSONData_Read.Json_Wifi_Index, SPIFFS_JSONData_Read.Json_First_Run);

      //@-发送配置成功页面
      request->send(200, "text/html", "HTTP GET request sent to your ESP on input field (" 
      + SPIFFS_JSONData_Save.Json_SSID1 + ") with pass: " + SPIFFS_JSONData_Save.Json_Pass1 +
      "<br><a href=\"/\">Return to Home Page</a>");

      // delay(5000);
      // ESP.restart();
      Dev_ConfigSuc_Flag = 1;
    });

    //@-挂载http get_api 操作
    server_dx.on("/get_api", HTTP_GET, [] (AsyncWebServerRequest *request) 
    {
      //@-获得参数
      String API_KEY1 = request->getParam("api_key1")->value();
      Serial.println(API_KEY1);
      //@-API Key 配置成功返回主页
      request->send(SPIFFS, "/Test.html", "text/html");
    });

    //@-配置服务器没有找到页面配置
    server_dx.onNotFound(notFound);

    //@-启动配置服务器
    server_dx.begin();

    //@-显示配置信息
    EPD_ShowConfig();
  }
  #if USE_BLE
  //@-蓝牙BLE模式
  else if(EPD_Dev_RunMode == 2)
  {

  }
  #endif
}

//@-获取信息网站JSON数据
void WIFI_Get_JsonInfo(String serverName, int Data_Mode, String Http_source)
{
  char temp[3500];
  float temp_float;
  int httpResponseCode;
  int http_payload_size;
  //@-创建HTTP链接
  HTTPClient http;

  //@-判断WIFI连接状态
  if(WiFi.status()== WL_CONNECTED)
  {
    //@-获得指定网站的JSON数据
    http.begin(serverName.c_str());

    //@-发送HTTP Get
    httpResponseCode = http.GET();

    //@-成功连接网站
    if (httpResponseCode>0) 
    {
      //@-获得GET返回值
      // Serial.print("HTTP Response code: ");
      // Serial.println(httpResponseCode);
      String payload = http.getString();

      //@1-从html页面中获取信息
      // String word3 = getValue(payload, '\n', 77);
      // Serial.println(word3);

      //@2-打印获得的数据长度
      http_payload_size = payload.length();
      Serial.print(Http_source);
      Serial.print(" HTTP payload Size:");
      Serial.println(http_payload_size);
      // Serial.println(payload);

      // size_t capacity = ESP.getMaxAllocHeap();  // ESP32
      // Serial.print(" ESP.getMaxAllocHeap:");
      // Serial.println(capacity);

      if(http_payload_size > 100)
      {
        // @3-从网站提供的API接口中获取信息，并将数据json化
        DynamicJsonDocument doc(6144);  //6144//3072
        // SpiRamJsonDocument  doc(14000);
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
            sprintf(NewsData[0].news_title, "1.%s                   ", temp);
            strcpy(temp, root["result"]["data"]["list"][1]["title"]);
            sprintf(NewsData[1].news_title, "2.%s                   ", temp);
            strcpy(temp, root["result"]["data"]["list"][2]["title"]);
            sprintf(NewsData[2].news_title, "3.%s                   ", temp);
            strcpy(temp, root["result"]["data"]["list"][3]["title"]);
            sprintf(NewsData[3].news_title, "4.%s                   ", temp);
            strcpy(temp, root["result"]["data"]["list"][4]["title"]);
            sprintf(NewsData[4].news_title, "5.%s                   ", temp);
            strcpy(temp, root["result"]["data"]["list"][5]["title"]);
            sprintf(NewsData[5].news_title, "6.%s                   ", temp);
            strcpy(temp, root["result"]["data"]["list"][6]["title"]);
            sprintf(NewsData[6].news_title, "7.%s                   ", temp);
            strcpy(temp, root["result"]["data"]["list"][7]["title"]);
            sprintf(NewsData[7].news_title, "8.%s                   ", temp);
            //@-显示标志置位
            Display_News_Flag = 1;

          }
          //@-Covid-19数据
          else if(Data_Mode == 2)
          {
            //@-对应serverName_covid1的json格式数据
            // Covid19Data.currentConfirmedCount = root["results"][0]["currentConfirmedCount"];
            // Covid19Data.currentConfirmedIncr = root["results"][0]["currentConfirmedIncr"];
            // Covid19Data.confirmedCount = root["results"][0]["confirmedCount"];
            // Covid19Data.confirmedIncr = root["results"][0]["confirmedIncr"];
            // Covid19Data.suspectedCount = root["results"][0]["suspectedCount"];
            // Covid19Data.suspectedIncr = root["results"][0]["suspectedIncr"];
            // Covid19Data.curedCount = root["results"][0]["curedCount"];
            // Covid19Data.curedIncr = root["results"][0]["curedIncr"];
            // Covid19Data.deadCount = root["results"][0]["deadCount"];
            // Covid19Data.deadIncr = root["results"][0]["deadIncr"];
            // Covid19Data.seriousCount = root["results"][0]["seriousCount"];
            // Covid19Data.seriousIncr = root["results"][0]["seriousIncr"];

            // Covid19Data.g_currentConfirmedCount = root["results"][0]["globalStatistics"]["currentConfirmedCount"];
            // Covid19Data.g_currentConfirmedIncr = root["results"][0]["globalStatistics"]["currentConfirmedIncr"];
            // Covid19Data.g_confirmedCount = root["results"][0]["globalStatistics"]["confirmedCount"];
            // Covid19Data.g_confirmedIncr = root["results"][0]["globalStatistics"]["confirmedIncr"];
            // Covid19Data.g_curedCount = root["results"][0]["globalStatistics"]["curedCount"];
            // Covid19Data.g_curedIncr = root["results"][0]["globalStatistics"]["curedIncr"];
            // Covid19Data.g_deadCount = root["results"][0]["globalStatistics"]["deadCount"];
            // Covid19Data.g_deadIncr = root["results"][0]["globalStatistics"]["deadIncr"];

            // https://lab.isaaclin.cn/nCoV/api/overall
            // Covid19Data.confirmedNum =  root["results"][0]["currentConfirmedCount"];
            // Covid19Data.confirmedIncr = root["results"][0]["currentConfirmedIncr"];
            // Covid19Data.externalConfirmedNum = root["results"][0]["suspectedCount"];
            // Covid19Data.externalConfirmedIncr = root["results"][0]["suspectedIncr"];
            // Covid19Data.asymptomaticNum = root["results"][0]["seriousCount"];
            // Covid19Data.asymptomaticIncr = root["results"][0]["seriousIncr"];
            // Covid19Data.deadNum = root["results"][0]["deadCount"];
            // Covid19Data.deadIncr = root["results"][0]["deadIncr"];

            //@-对应serverName_covid的dx1023站点json格式数据
            // Covid19Data.confirmedNum = root["result"]["confirmedNum"];
            // Covid19Data.confirmedIncr = root["result"]["confirmedIncr"];
            // Covid19Data.externalConfirmedNum = root["result"]["externalConfirmedNum"];
            // Covid19Data.externalConfirmedIncr = root["result"]["externalConfirmedIncr"];
            // Covid19Data.asymptomaticNum = root["result"]["asymptomaticNum"];
            // Covid19Data.asymptomaticIncr = root["result"]["asymptomaticIncr"];
            // Covid19Data.deadNum = root["result"]["deadNum"];
            // Covid19Data.deadIncr = root["result"]["deadIncr"];

            //@-获得图片数据
            strcpy(temp, root["result"]);
            sprintf(Covid19Data.dx_string, "%s", temp);
            String dx_s = String(Covid19Data.dx_string);
            int s_size = dx_s.length();
            String dx_s1 = dx_s.substring(15,s_size-3); 

            // Split the string into substrings
            StringCount = 0;
            while (dx_s1.length() > 0)
            {
              int index = dx_s1.indexOf(',');
              if (index == -1) // No space found
              {
                json_gImage_play_W56H59[StringCount++] = dx_s1.toInt();
                break;
              }
              else
              {
                json_gImage_play_W56H59[StringCount++] = dx_s1.substring(0, index).toInt();
                dx_s1 = dx_s1.substring(index+1);
              }
            }

            Serial.println(json_gImage_play_W56H59[0]);
            Serial.println(StringCount);
            Serial.println(json_gImage_play_W56H59[StringCount-1]);

            // for(int i=0; i < 3480; i++)
            // json_gImage_play_W56H59[i] = root["result"]["gray_img"][i];
            
            //@-显示标志置位
            Display_Covid_Flag = 1;
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
            strcpy(temp, root["result"]["future"][1]["date"]);
            sprintf(Juhe_WeatherData.weather_futureDay1_date, "%s", temp);

            strcpy(temp, root["result"]["future"][2]["temperature"]);
            sprintf(Juhe_WeatherData.weather_futureDay2_temperature, "%s", temp);
            strcpy(temp, root["result"]["future"][2]["weather"]);
            sprintf(Juhe_WeatherData.weather_futureDay2_info, "%s", temp);
            strcpy(temp, root["result"]["future"][2]["date"]);
            sprintf(Juhe_WeatherData.weather_futureDay2_date, "%s", temp);

            strcpy(temp, root["result"]["future"][3]["temperature"]);
            sprintf(Juhe_WeatherData.weather_futureDay3_temperature, "%s", temp);
            strcpy(temp, root["result"]["future"][3]["weather"]);
            sprintf(Juhe_WeatherData.weather_futureDay3_info, "%s", temp);
            strcpy(temp, root["result"]["future"][3]["date"]);
            sprintf(Juhe_WeatherData.weather_futureDay3_date, "%s", temp);

            //@-显示标志置位
            Display_Weather_Flag = 1;

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

            //@-显示标志置位
            Display_Lunar_Flag = 1;
          }
          //@-上证指数
          else if(Data_Mode == 5)
          {
            strcpy(temp, root["result"]["nowpri"]);
            temp_float = String(temp).toFloat();
            sprintf(Juhe_StockData.sh_stock_value, "%0.2f", temp_float);
            strcpy(temp, root["result"]["increPer"]);
            temp_float = String(temp).toFloat();
            sprintf(Juhe_StockData.sh_stock_per, "%0.2f", temp_float);

            //@-显示标志置位
            Display_Stock_Flag = 1;
          }
          //@-深圳指数
          else if(Data_Mode == 6)
          {
            strcpy(temp, root["result"]["nowpri"]);
            temp_float = String(temp).toFloat();
            sprintf(Juhe_StockData.sz_stock_value, "%0.2f", temp_float);
            strcpy(temp, root["result"]["increPer"]);
            temp_float = String(temp).toFloat();
            sprintf(Juhe_StockData.sz_stock_per, "%0.2f", temp_float);

            //@-显示标志置位
            Display_Stock_Flag = 1;
          }
          //@-万维气象
          else if(Data_Mode == 7)
          {
            strcpy(temp, root["showapi_res_body"]["pm"]["quality"]);
            sprintf(WanWei_WeatherData.quality, "%s", temp);
            strcpy(temp, root["showapi_res_body"]["pm"]["pm2_5"]);
            sprintf(WanWei_WeatherData.pm2_5, "%s", temp);
            strcpy(temp, root["showapi_res_body"]["pm"]["pm10"]);
            sprintf(WanWei_WeatherData.pm10, "%s", temp);
            strcpy(temp, root["showapi_res_body"]["pm"]["so2"]);
            sprintf(WanWei_WeatherData.so2, "%s", temp);
            strcpy(temp, root["showapi_res_body"]["pm"]["o3"]);
            sprintf(WanWei_WeatherData.o3, "%s", temp);
            strcpy(temp, root["showapi_res_body"]["pm"]["co"]);
            sprintf(WanWei_WeatherData.co, "%s", temp);
            strcpy(temp, root["showapi_res_body"]["pm"]["no2"]);
            sprintf(WanWei_WeatherData.no2, "%s", temp);
            Serial.println(WanWei_WeatherData.quality);
            Display_WanWeiWeather_Flag = 1;
          }
          //@-万维农历
          else if(Data_Mode == 8)
          {
            strcpy(temp, root["showapi_res_body"]["nongli"]);
            sprintf(WanWei_HuangliData.nongli, "%s", temp);
            strcpy(temp, root["showapi_res_body"]["ganzhi"]);
            sprintf(WanWei_HuangliData.ganzhi, "%s", temp);
            strcpy(temp, root["showapi_res_body"]["jieri"]);
            sprintf(WanWei_HuangliData.jieri, "%s", temp);
            strcpy(temp, root["showapi_res_body"]["qixiang"]);
            sprintf(WanWei_HuangliData.qixiang, "%s", temp);
            strcpy(temp, root["showapi_res_body"]["jieqi24"]);
            sprintf(WanWei_HuangliData.jieqi24, "%s", temp);
            Serial.println(WanWei_HuangliData.jieqi24);
            Display_WanWeiLunar_Flag = 1;
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
}

//@-EPD绘制时间-年月日-实时时间
void EPD_Paint_YearTime()
{
  int temp_data = 0; 

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
    case 0: sprintf(buff_dx,"周日"); break;
    case 1: sprintf(buff_dx,"周一"); break;
    case 2: sprintf(buff_dx,"周二"); break;
    case 3: sprintf(buff_dx,"周三"); break;
    case 4: sprintf(buff_dx,"周四"); break;
    case 5: sprintf(buff_dx,"周五"); break;
    case 6: sprintf(buff_dx,"周六"); break;
    default: break;
  }
  epd_drv_dx.DrawUTF( 70, 45, buff_dx, 1); 

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
  if(Charge_State == 0)
  epd_drv_dx.User_Img_Tran(user_area_dx.width, user_area_dx.height, gImage_battery, S1D13541_LD_IMG_1BPP,&user_area_dx,1);
  else if(Charge_State == 1)
  epd_drv_dx.User_Img_Tran(user_area_dx.width, user_area_dx.height, gImage_battery_charge, S1D13541_LD_IMG_1BPP,&user_area_dx,1);
  //@-电量
  temp_data = ((BAT_V-3.1)/0.012);
  epd_drv_dx.EPD_SetFount(FONT12);
  sprintf(buff_dx,"%2d", temp_data);
  epd_drv_dx.DrawUTF( 223 , 3, buff_dx, 1); 

  //@-显示时间
  epd_drv_dx.DrawTime(121, 17, dx_timeStruct.hours, dx_timeStruct.minutes, FONT48_NUM, 1);
}
//@-EPD绘制农历时间
void EPD_Paint_Lunar()
{
  //@-显示黄历
  epd_drv_dx.EPD_SetFount(FONT16);
  sprintf(buff_dx,"%s", Juhe_HuangliData.lunarYear);
  // sprintf(buff_dx,"辛丑年");
  epd_drv_dx.DrawUTF( 35 , 65, buff_dx, 1); 
  sprintf(buff_dx,"%s", Juhe_HuangliData.lunar);
  // sprintf(buff_dx,"五月初九");
  epd_drv_dx.DrawUTF( 25 , 81, buff_dx, 1); 
}
//@-EPD绘制天气概述
void EPD_Paint_WeatherSummer()
{
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
}
//@-EPD绘制未来天气
void EPD_Paint_WeatherFuture()
{
  String temp_str;
  char temp_char[32];
  char temp_char1[32];
  int next_day1 = 0;
  int next_day2 = 0;
  int next_day3 = 0;

  //@-显示api图标
  user_area_dx.left = 5;        //x
  user_area_dx.top = 110;       //y
  user_area_dx.width = 16;
  user_area_dx.height = 16;
  epd_drv_dx.User_Img_Tran(user_area_dx.width, user_area_dx.height, gImage_aqi, S1D13541_LD_IMG_1BPP,&user_area_dx,1);
  //@-显示api数值
  epd_drv_dx.EPD_SetFount(FONT12);
  sprintf(buff_dx,"%s", Juhe_WeatherData.weather_aqi);
  epd_drv_dx.DrawUTF( 23 ,113, buff_dx, 1); 
  //@-显示湿度图标
  user_area_dx.left = 5;        //x
  user_area_dx.top = 130;       //y
  user_area_dx.width = 16;
  user_area_dx.height = 16;
  epd_drv_dx.User_Img_Tran(user_area_dx.width, user_area_dx.height, gImage_shidu, S1D13541_LD_IMG_1BPP,&user_area_dx,1);
  //@-显示湿度数值
  epd_drv_dx.EPD_SetFount(FONT12);
  sprintf(buff_dx,"%s", Juhe_WeatherData.weather_humidity);
  epd_drv_dx.DrawUTF( 23 ,133, buff_dx, 1); 

  //@-显示后三天天气数据
  next_day1 = dx_dateStruct.weekDay + 1;
  next_day2 = dx_dateStruct.weekDay + 2;
  next_day3 = dx_dateStruct.weekDay + 3;
  if(next_day1 > 6) 
  next_day1 = next_day1 - 7;
  if(next_day2 > 6) 
  next_day2 = next_day2 - 7;
  if(next_day3 > 6) 
  next_day3 = next_day3 - 7;

  epd_drv_dx.EPD_SetFount(FONT12);
  switch(next_day1)
  {
    case 0: sprintf(temp_char,"周日"); break;
    case 1: sprintf(temp_char,"周一"); break;
    case 2: sprintf(temp_char,"周二"); break;
    case 3: sprintf(temp_char,"周三"); break;
    case 4: sprintf(temp_char,"周四"); break;
    case 5: sprintf(temp_char,"周五"); break;
    case 6: sprintf(temp_char,"周六"); break;
    default: break;
  }
  sprintf(temp_char1,"%s", Juhe_WeatherData.weather_futureDay1_date);
  temp_str = String(temp_char1).substring(5);
  sprintf(buff_dx,"%s%s %s%s", temp_str.c_str(), temp_char, Juhe_WeatherData.weather_futureDay1_info, Juhe_WeatherData.weather_futureDay1_temperature);
  epd_drv_dx.DrawUTF( 48 ,110, buff_dx, 1); 

  switch(next_day2)
  {
    case 0: sprintf(temp_char,"周日"); break;
    case 1: sprintf(temp_char,"周一"); break;
    case 2: sprintf(temp_char,"周二"); break;
    case 3: sprintf(temp_char,"周三"); break;
    case 4: sprintf(temp_char,"周四"); break;
    case 5: sprintf(temp_char,"周五"); break;
    case 6: sprintf(temp_char,"周六"); break;
    default: break;
  }
  sprintf(temp_char1,"%s", Juhe_WeatherData.weather_futureDay2_date);
  temp_str = String(temp_char1).substring(5);
  sprintf(buff_dx,"%s%s %s%s", temp_str.c_str(), temp_char, Juhe_WeatherData.weather_futureDay2_info, Juhe_WeatherData.weather_futureDay2_temperature);
  epd_drv_dx.DrawUTF( 48 ,125, buff_dx, 1); 

  switch(next_day3)
  {
    case 0: sprintf(temp_char,"周日"); break;
    case 1: sprintf(temp_char,"周一"); break;
    case 2: sprintf(temp_char,"周二"); break;
    case 3: sprintf(temp_char,"周三"); break;
    case 4: sprintf(temp_char,"周四"); break;
    case 5: sprintf(temp_char,"周五"); break;
    case 6: sprintf(temp_char,"周六"); break;
    default: break;
  }
  sprintf(temp_char1,"%s", Juhe_WeatherData.weather_futureDay3_date);
  temp_str = String(temp_char1).substring(5);
  sprintf(buff_dx,"%s%s %s%s", temp_str.c_str(), temp_char, Juhe_WeatherData.weather_futureDay3_info, Juhe_WeatherData.weather_futureDay3_temperature);
  epd_drv_dx.DrawUTF( 48 ,140, buff_dx, 1); 
}
//@-EPD绘制Covid19信息
void EPD_Paint_Covid()
{
  //@-显示国内疫情-现存确诊人数
  epd_drv_dx.EPD_SetFount(FONT12);
  sprintf(buff_dx,"较昨日%d", Covid19Data.confirmedIncr);
  epd_drv_dx.DrawUTF( 5 ,160, buff_dx, 1); 
  sprintf(buff_dx,"%d", Covid19Data.confirmedNum);
  epd_drv_dx.DrawUTF( 8 ,175, buff_dx, 1); 
  sprintf(buff_dx,"现存确诊");
  epd_drv_dx.DrawUTF( 5 ,190, buff_dx, 1); 
  //@-显示国内疫情-境外输入人数
  sprintf(buff_dx,"较昨日%d", Covid19Data.externalConfirmedIncr);
  epd_drv_dx.DrawUTF( 67 ,160, buff_dx, 1); 
  sprintf(buff_dx,"%d", Covid19Data.externalConfirmedNum);
  epd_drv_dx.DrawUTF( 70 ,175, buff_dx, 1); 
  sprintf(buff_dx,"境外输入");
  epd_drv_dx.DrawUTF( 67 ,190, buff_dx, 1); 
  //@-显示国内疫情-无症状人数
  sprintf(buff_dx,"较昨日%d", Covid19Data.asymptomaticIncr);
  epd_drv_dx.DrawUTF( 124 ,160, buff_dx, 1); 
  sprintf(buff_dx,"%d", Covid19Data.asymptomaticNum);
  epd_drv_dx.DrawUTF( 127 ,175, buff_dx, 1); 
  sprintf(buff_dx,"现无症状");
  epd_drv_dx.DrawUTF( 124 ,190, buff_dx, 1); 
  //@-显示国内疫情-死亡人数
  sprintf(buff_dx,"较昨日%d", Covid19Data.deadIncr);
  epd_drv_dx.DrawUTF( 181 ,160, buff_dx, 1); 
  sprintf(buff_dx,"%d", Covid19Data.deadNum);
  epd_drv_dx.DrawUTF( 184 ,175, buff_dx, 1); 
  sprintf(buff_dx,"累计死亡");
  epd_drv_dx.DrawUTF( 181 ,190, buff_dx, 1); 
}
//@-EPD绘制股票信息
void EPD_Paint_Stock()
{
  //@-显示上证指数
  epd_drv_dx.EPD_SetFount(FONT12);
  sprintf(buff_dx,"上证指数%s", Juhe_StockData.sh_stock_value);
  epd_drv_dx.DrawUTF( 5 ,210, buff_dx, 1); 
  String stock_str = String(Juhe_StockData.sh_stock_per);
  float stock_float = stock_str.toFloat();
  if(stock_float < 0)
  sprintf(buff_dx,"跌 %s%%", Juhe_StockData.sh_stock_per);
  else
  sprintf(buff_dx,"涨 %s%%", Juhe_StockData.sh_stock_per);
  epd_drv_dx.DrawUTF( 5 ,225, buff_dx, 1); 
  //@-显示深圳指数
  sprintf(buff_dx,"深圳指数%s", Juhe_StockData.sz_stock_value);
  epd_drv_dx.DrawUTF( 125 ,210, buff_dx, 1); 
  stock_str = String(Juhe_StockData.sz_stock_per);
  stock_float = stock_str.toFloat();
  if(stock_float < 0)
  sprintf(buff_dx,"跌 %s%%", Juhe_StockData.sz_stock_per);
  else
  sprintf(buff_dx,"涨 %s%%", Juhe_StockData.sz_stock_per);
  epd_drv_dx.DrawUTF( 125 ,225, buff_dx, 1); 
}
//@-EPD绘制新闻信息
void EPD_Paint_News()
{
  // epd_drv_dx.EPD_SetFount(FONT16);
  // epd_drv_dx.DrawUTF( 0 , 245, NewsData[0].news_title, 1); 
  // epd_drv_dx.DrawUTF( 0 , 245+17, NewsData[1].news_title, 1); 
  // epd_drv_dx.DrawUTF( 0 , 245+34, NewsData[2].news_title, 1); 
  // epd_drv_dx.DrawUTF( 0 , 245+51, NewsData[3].news_title, 1); 
  // epd_drv_dx.DrawUTF( 0 , 245+68, NewsData[4].news_title, 1); 
  // epd_drv_dx.DrawUTF( 0 , 245+85, NewsData[5].news_title, 1); 
  // epd_drv_dx.DrawUTF( 0 , 245+102, NewsData[6].news_title, 1); 
  // epd_drv_dx.DrawUTF( 0 , 245+119, NewsData[7].news_title, 1); 

  //@-显示网络图片
  user_area_dx.left = 20;    //x
  user_area_dx.top = 250;       //y
  user_area_dx.width = 56;
  user_area_dx.height = 59;
  epd_drv_dx.User_Img_Tran(user_area_dx.width, user_area_dx.height, json_gImage_play_W56H59, S1D13541_LD_IMG_1BPP,&user_area_dx,1);
 
}
//@-EPD绘制底部信息
void EPD_Paint_BottomInfo()
{
  //@-wifi-图标
  user_area_dx.left = 2;    //x
  user_area_dx.top = 383;       //y
  user_area_dx.width = 16;
  user_area_dx.height = 16;
  epd_drv_dx.User_Img_Tran(user_area_dx.width, user_area_dx.height, gImage_wifi, S1D13541_LD_IMG_1BPP,&user_area_dx,1);
  //@-显示wifi ssid
  epd_drv_dx.EPD_SetFount(FONT12);
  sprintf(buff_dx,"%s", ssid);
  epd_drv_dx.DrawUTF( 20 ,385, buff_dx, 1); 
  //@-显示软件版本号
  sprintf(buff_dx,"%s", Software_Version);
  epd_drv_dx.DrawUTF( 150 ,385, buff_dx, 1); 
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

//@-非全局刷新
void EPD_ShowArea()
{
  int temp_data = 0; 
  String temp_str;
  char temp_char[32];
  char temp_char1[32];
  int next_day1 = 0;
  int next_day2 = 0;
  int next_day3 = 0;

  Serial.println("----------DrawTimeUpdata-----------");
  epd_drv_dx.EPD4INC_Port_Reinit();      //SPI初始化
  epd_drv_dx.EPD4INC_HVEN();
  delay(2); 
  epd_drv_dx.EPD_CLK_EX();               //其中时钟
  epd_drv_dx.s1d135xx_set_epd_power(1);
  delay(2);

  //@1-------------------------------------实时时间--------------------------------------------
  EPD_Paint_YearTime();
  //@-局部刷新
  user_area_dx.left = 0;   //x
  user_area_dx.top = 0;    //y
  user_area_dx.width = 239;
  user_area_dx.height = 65;
  
  //@2-------------------------------------农历天气--------------------------------------------
  if(Display_Lunar_Flag == 1)
  {
    Serial.println("----------Area-EPD_Display_Lunar-----------");
    Display_Lunar_Flag = 0;
    EPD_Paint_Lunar();
    //@-局部刷新
    user_area_dx.left = 0;   //x
    user_area_dx.top =  0;    //66;    //y
    user_area_dx.width = 115;
    user_area_dx.height = 105;
  }

  if(Display_Weather_Flag == 1)
  {
    Serial.println("----------Area-EPD_Display_WeatherSummer__WeatherFuture-----------");
    Display_Weather_Flag = 0;
    EPD_Paint_WeatherSummer();
    EPD_Paint_WeatherFuture();
    //@-局部刷新
    user_area_dx.left = 0;   //x
    user_area_dx.top =  0;//106;    //y
    user_area_dx.width = 239;
    user_area_dx.height = 155; 
  }

  //@3-------------------------------------Covid--------------------------------------------
  if(Display_Covid_Flag == 1)
  {
    Serial.println("----------Area-EPD_Display_Covid-----------");
    Display_Covid_Flag = 0;
    EPD_Paint_Covid();
    //@-局部刷新
    user_area_dx.left = 0;   //x
    user_area_dx.top =  0;//156;    //y
    user_area_dx.width = 239;
    user_area_dx.height = 205;  
  }

  //@4-------------------------------------股票--------------------------------------------
  if(Display_Stock_Flag == 1)
  {
    Serial.println("----------Area-EPD_Display_Stock-----------");
    Display_Stock_Flag = 0;
    EPD_Paint_Stock();
    //@-局部刷新
    user_area_dx.left = 0;   //x
    user_area_dx.top =  0;//206;    //y
    user_area_dx.width = 239;
    user_area_dx.height = 240;
  }

  //@5-------------------------------------新闻--------------------------------------------
  if(Display_News_Flag == 1)
  {
    Serial.println("----------Area-EPD_Display_News-----------");
    Display_News_Flag = 0;
    EPD_Paint_News();
    //@-局部刷新
    user_area_dx.left = 0;   //x
    user_area_dx.top = 0;//240;    //y
    user_area_dx.width = 239;
    user_area_dx.height = 382;
  }

  epd_drv_dx.EPD_UpdateUser(2, UPDATE_PARTIAL_AREA, &user_area_dx);

  //   epd_drv_dx.EPD_UpdateUser(1, UPDATE_FULL, NULL);

  delay(500);
  epd_drv_dx.EPD4INC_HVDISEN();
  epd_drv_dx.s1d135xx_set_power_state(PL_EPDC_SLEEP);
  epd_drv_dx.EPD_CLK_STOP();
}

//@-全刷新显示
void EPD_ShowMain()
{
  int temp_data = 0;
  String temp_str;
  char temp_char[32];
  char temp_char1[32];
  int next_day1 = 0;
  int next_day2 = 0;
  int next_day3 = 0;

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
  //@-区域3分割线
  epd_drv_dx.Buf_DrawLine(0,Area3_Box_High,239,Area3_Box_High);   //@-横线 
  // epd_drv_dx.Buf_DrawLine(Area3_Box_X ,Area2_Box_High,Area3_Box_X ,Area3_Box_High);   //@-竖线
  //@-区域4分割线
  epd_drv_dx.Buf_DrawLine(0,Area4_Box_High,239,Area4_Box_High);   //@-横线 
  epd_drv_dx.Buf_DrawLine(Area4_Box_X ,Area3_Box_High,Area4_Box_X ,Area4_Box_High);   //@-竖线
  //@-区域5分割线
  epd_drv_dx.Buf_DrawLine(0,Area5_Box_High,239,Area5_Box_High);   //@-横线 

  epd_drv_dx.EPD4INC_HVEN();
  delay(2);

  //@-全部显示缓存更新
  epd_drv_dx.Buf_UpdateFull(1);

  //@1---------------------------------------------------------------------------------
  EPD_Paint_YearTime();

  EPD_Paint_Lunar();

  EPD_Paint_WeatherSummer();

  //@2---------------------------------------------------------------------------------
  EPD_Paint_WeatherFuture();

  //@3---------------------------------------------------------------------------------
  EPD_Paint_Covid();

  //@4---------------------------------------------------------------------------------
  EPD_Paint_Stock();

  //@5---------------------------------------------------------------------------------
  EPD_Paint_News();

  //@6---------------------------------------------------------------------------------
  EPD_Paint_BottomInfo();

  Display_Weather_Flag = 0;
  Display_Covid_Flag =   0;
  Display_Stock_Flag =   0;
  Display_News_Flag =    0;
  Display_Lunar_Flag =   0;

  epd_drv_dx.EPD4INC_HVEN();
  delay(10);
  epd_drv_dx.EPD_UpdateUser(1, UPDATE_FULL, NULL);
 
  delay(900);
 
  epd_drv_dx.EPD4INC_HVDISEN();
  epd_drv_dx.s1d135xx_set_power_state(PL_EPDC_SLEEP);
  epd_drv_dx.EPD_CLK_STOP();
}

//@-全刷新显示配置信息
void EPD_ShowConfig()
{
 Serial.println("----------Start_Config_EPD()-----------");
  
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
  // //@-区域1分割线
  // epd_drv_dx.Buf_DrawLine(0,Area1_Box_High,239,Area1_Box_High);   //@-横线 
  // epd_drv_dx.Buf_DrawLine(Area1_Box_X ,0,Area1_Box_X ,Area1_Box_High);   //@-竖线
  
  epd_drv_dx.EPD4INC_HVEN();
  delay(2);

  //@-全部显示缓存更新
  epd_drv_dx.Buf_UpdateFull(1);

  //@1---------------------------------------------------------------------------------
  //@-显示
  epd_drv_dx.EPD_SetFount(FONT16);
  sprintf(buff_dx,"wifi连接设备");
  epd_drv_dx.DrawUTF( 125 , 160, buff_dx, 1); 
  sprintf(buff_dx,"ssid:dx_epd");
  epd_drv_dx.DrawUTF( 125 , 177, buff_dx, 1); 
  sprintf(buff_dx,"pass:dingxiao");
  epd_drv_dx.DrawUTF( 125 , 194, buff_dx, 1); 

  //@-二维码
  user_area_dx.left = 135;    //x
  user_area_dx.top = 220;       //y
  user_area_dx.width = 80;
  user_area_dx.height = 80;
  //@-图片的尺寸需要8的倍数
  epd_drv_dx.User_Img_Tran(user_area_dx.width, user_area_dx.height, gImage_QRCode_W80H80, S1D13541_LD_IMG_1BPP,&user_area_dx,1);

  //@-漫画
  user_area_dx.left = 2;    //x
  user_area_dx.top = 80;       //y
  user_area_dx.width = 120;
  user_area_dx.height = 232;
  //@-图片的尺寸需要8的倍数
  epd_drv_dx.User_Img_Tran(user_area_dx.width, user_area_dx.height, gImage_play_W120H232, S1D13541_LD_IMG_1BPP,&user_area_dx,1);
  
  epd_drv_dx.EPD4INC_HVEN();
  delay(10);
  epd_drv_dx.EPD_UpdateUser(1, UPDATE_FULL, NULL);
 
  delay(900);
 
  epd_drv_dx.EPD4INC_HVDISEN();
  epd_drv_dx.s1d135xx_set_power_state(PL_EPDC_SLEEP);
  epd_drv_dx.EPD_CLK_STOP();
  
}

#if 0
//@-全刷新显示配置成功信息
void EPD_ShowConfigSuc()
{
  Serial.println("----------Config_Success()-----------");
  epd_drv_dx.EPD4INC_Port_Reinit();      //SPI初始化

  //通过BUF处理图层
  epd_drv_dx.Buf_Clear();

  epd_drv_dx.EPD4INC_HVEN();
  delay(2); 
  epd_drv_dx.EPD_CLK_EX();               //其中时钟
  epd_drv_dx.s1d135xx_set_epd_power(1);
  delay(2);


  //@-中国人
  user_area_dx.left = 31;    //x
  user_area_dx.top = 100;       //y
  user_area_dx.width = 176;
  user_area_dx.height = 192;
  //@-图片的尺寸需要8的倍数
  epd_drv_dx.User_Img_Tran(user_area_dx.width, user_area_dx.height, gImage_chinese_W176H192, S1D13541_LD_IMG_1BPP,&user_area_dx,1);
  
  //@-显示
  epd_drv_dx.EPD_SetFount(FONT16);
  sprintf(buff_dx,"设备重启中...");
  epd_drv_dx.DrawUTF( 80 , 295, buff_dx, 1); 


  //@-局部刷新
  user_area_dx.left = 0;   //x
  user_area_dx.top = 0;    //y
  user_area_dx.width = 239;
  user_area_dx.height = 350;
  
  epd_drv_dx.EPD_UpdateUser(2, UPDATE_PARTIAL_AREA, &user_area_dx);

  delay(500);
  epd_drv_dx.EPD4INC_HVDISEN();
  epd_drv_dx.s1d135xx_set_power_state(PL_EPDC_SLEEP);
  epd_drv_dx.EPD_CLK_STOP();
  
}
#endif

//@-非全局刷新
void EPD_ShowCofigSuc_Second(int second)
{
  Serial.println("----------SecondUpdata-----------");
  epd_drv_dx.EPD4INC_Port_Reinit();      //SPI初始化
  epd_drv_dx.EPD4INC_HVEN();
  delay(2); 
  epd_drv_dx.EPD_CLK_EX();               //其中时钟
  epd_drv_dx.s1d135xx_set_epd_power(1);
  delay(2);

  //@1-------------------------------------秒时间倒计时--------------------------------------------
  //@-显示
  epd_drv_dx.EPD_SetFount(FONT16);
  sprintf(buff_dx,"设备重启中...");
  epd_drv_dx.DrawUTF( 80 , 315, buff_dx, 1); 

  sprintf(buff_dx,"%d秒", second);
  epd_drv_dx.DrawUTF( 108 , 332, buff_dx, 1); 
  //@-局部刷新
  user_area_dx.left = 70;   //x
  user_area_dx.top = 312;    //y
  user_area_dx.width = 239;
  user_area_dx.height = 50;
  
  epd_drv_dx.EPD_UpdateUser(2, UPDATE_PARTIAL_AREA, &user_area_dx);

  delay(500);
  epd_drv_dx.EPD4INC_HVDISEN();
  epd_drv_dx.s1d135xx_set_power_state(PL_EPDC_SLEEP);
  epd_drv_dx.EPD_CLK_STOP();
}

//@-连接wifi
void WIFI_Connect()
{
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  // Connect to Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid.c_str(), password.c_str());
  // WiFi.begin("liang12", "12345678");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    wifi_connect_tick = wifi_connect_tick + 1;
    if(wifi_connect_tick > wifi_connect_time)
    {
      wifi_connect_tick = 0;

      bootCount = 0;

      //@-切换wifi信号源
      if(SPIFFS_JSONData_Read.Json_Wifi_Index == WIFI_Index_1)
      Save_Set_Data(SPIFFS_Save_Wifi_Switch, WIFI_Index_2, 0);
      else if(SPIFFS_JSONData_Read.Json_Wifi_Index == WIFI_Index_2)
      Save_Set_Data(SPIFFS_Save_Wifi_Switch, WIFI_Index_1, 0);

      break;
    }
  }
  if(WiFi.status() == WL_CONNECTED)
  {
    Serial.println("");
    Serial.println("WiFi connected.");
  }

  //@-wifi连接成功-每天23:01-23:03进行网络对时
  if(((WiFi.status() == WL_CONNECTED) && (dx_timeStruct.hours == 23) && 
    ((dx_timeStruct.minutes == 5)||(dx_timeStruct.minutes == 15)||(dx_timeStruct.minutes == 20)))
    ||(bootCount == 1))
  {
    // Init and get the time
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    printLocalTime();
  }
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

#if USE_BLE
//@-BLE设备服务回调
class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

//@-BLE设备接收回调
class MyCallbacks: public BLECharacteristicCallbacks {
  
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string rxValue = pCharacteristic->getValue();
                            
    //  Serial.println("DXDXDX");

      if( rxValue.length() < 256)
      {
          memset(BLE_Msg, 0, sizeof(BLE_Msg));
          for (int i = 0; i < rxValue.length(); i++)
          {
            BLE_Msg[i] = rxValue[i];
            // BLE_Msg_Input.setCharAt(i,rxValue[i]);
          }
      }
     }
};

void Dev_BLEInit()
{
  // Create the BLE Device
  BLEDevice::init("DX_EPD_BLE");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );
  pCharacteristic->setCallbacks(new MyCallbacks()); 

  // https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.descriptor.gatt.client_characteristic_configuration.xml
  // Create a BLE Descriptor
  pCharacteristic->addDescriptor(new BLE2902());

  // Start the service
  pService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
  BLEDevice::startAdvertising();
  Serial.println("Waiting a client connection to notify...");
}
#endif


//@-主循环
void loop()
{
  //@-键盘扫描
  Button_Check();

  //@-读取AD
  BAT_V = analogRead(BAT_V_PIN)/560.1;

  //@-判断是否配置成功
  if(Dev_ConfigSuc_Flag == 1)
  {
    if(Dev_ConfigSecond <= 0)
    {
      ESP.restart();
    }

    EPD_ShowCofigSuc_Second(Dev_ConfigSecond);
    Dev_ConfigSecond = Dev_ConfigSecond - 1;
    delay(500);
  }
  else if(Dev_ConfigSuc_Flag == 0)
  {
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
}
