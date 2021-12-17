/**
 * --------------ESP32_DesktipDev是一款智能桌面显示设备------------
 * 1-显示实时时间
 * 2-显示当地天气数据
 * 3-显示农历信息
 * 4-显示疫情信息
 * 5-显示股票信息
 * 6-显示国内新闻
 * 7-具有番茄钟功能
 * 8-具有闹钟功能
 * 9-具备OTA远程软件更新功能
 **/


/**
 * 
 * 头文件
 * 
**/
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <ArduinoJson.h>

#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>
#include <HTTPClient.h>

#include <TFT_eSPI.h> 


#define USE_BLE 0
#if USE_BLE
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#endif


/**
 * 
 * 宏定义
 * 
**/
#define Software_Version "20211217 V0.1"

#define ESP32


//@-IO定义
#define uS_TO_S_FACTOR 1000000ULL  /* Conversion factor for micro seconds to seconds */

#define SPIFFS_Save_Def          0
#define SPIFFS_Save_Data         1
#define SPIFFS_Save_Wifi_Switch  2

#define WIFI_Index_1             0
#define WIFI_Index_2             1



/**
 * 
 * 全局变量
 * 
**/

//@-创建LCD驱动-使用1.54寸TFT，驱动为SetupDx_ST7789.h
TFT_eSPI tft = TFT_eSPI(); 

//@-WIFI连接信息
const String ssid1     = "wuyiyi";
const String password1 = "dingxiao";
const String ssid2 = "DX_JS";  
const String password2 = "dingxiao";


String ssid;
String password;
int wifi_connect_tick = 0;
int wifi_connect_time = 60;  //默认wifi连接时间

char buff_dx[128];


//@-网络授时
// const char* ntpServer = "pool.ntp.org";   ----->ok
// const char* ntpServer = "cn.pool.ntp.org";  ---->OK
// const char* ntpServer = "time.pool.aliyun.com";   ---->OK
const char* ntpServer = "ntp.aliyun.com";

//@-设置时区参数
const long  gmtOffset_sec = 7 * 3600;
const int   daylightOffset_sec = 3600;

int Dev_SystemTime_Year = 2021;
int Dev_SystemTime_Month = 12;
int Dev_SystemTime_Day = 17;
int Dev_SystemTime_WeekOfDay = 1;
int Dev_SystemTime_Hour = 10;
int Dev_SystemTime_Minute = 23;
int Dev_SystemTime_Second = 0;

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
// const uint8_t* weather_index_img_id_dx = gImage_weather_00;

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

  //@-初始化串口
  Serial.begin(115200);

  Serial.println((String)"Memory available in PSRAM : " +ESP.getFreePsram());

  //@-设备启动次数
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

  #if USE_BLE
  //@-查询是否进入蓝牙BLE模式
  if(wakeup_reason == ESP_SLEEP_WAKEUP_EXT1)
  {
    Serial.println("BLE Mode Start----------------->");
    Dev_BLEInit();
  }
  #endif

  //@-TFT GUI初始化
  tft.begin();
  tft.setRotation(0);  // portrait
  tft.fillScreen(TFT_BLACK);

  //@-连接WIFI
  WIFI_Connect();

  //@-每5min获取Sina综合新闻json数据
  WIFI_Get_JsonInfo(serverName_sinaNews, 1, "新浪新闻");

  //@-显示信息
  tft.loadFont("STKAITI18");
  // tft.setCursor(40,40);
  // tft.println(WiFi.localIP().toString());
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.setCursor(0,0);
  // tft.println(NewsData[0].news_title);
  String myNewString(NewsData[0].news_title);
  tft.drawString(myNewString,0,0,18);
  tft.setTextColor(TFT_RED, TFT_BLACK);
  tft.setCursor(0,32);
  tft.println(NewsData[1].news_title);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setCursor(0,64);
  tft.println(NewsData[2].news_title);
  // Remove font parameters from memory to recover RAM
  tft.unloadFont();

  //@-断开WIFI连接
  WIFI_Disconnect();

}

//@-断开WIFI连接
void WIFI_Disconnect()
{
  //@-断开wifi链接
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  Serial.println("WiFi Disconnected");
}

//@-连接wifi
void WIFI_Connect()
{
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  // Connect to Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(ssid1);
  WiFi.begin(ssid1.c_str(), password1.c_str());
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
    //@-网络对时
    WIFI_Get_InternetTime();
  }
}

//@-获取网络对时
void WIFI_Get_InternetTime()
{
  struct tm timeinfo;

  // Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

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

  // Serial.println("----------dx--------------");
  // Serial.println(timeinfo.tm_year);
  // Serial.println(timeinfo.tm_mon);
  // Serial.println(timeinfo.tm_mday);
  // Serial.println(timeinfo.tm_wday);
  // Serial.println(timeinfo.tm_hour);
  // Serial.println(timeinfo.tm_min);
  // Serial.println(timeinfo.tm_sec);


  Dev_SystemTime_Year = timeinfo.tm_year+1900;
  Dev_SystemTime_Month = timeinfo.tm_mon+1;
  Dev_SystemTime_Day = timeinfo.tm_mday;
  Dev_SystemTime_WeekOfDay = timeinfo.tm_wday;
  Dev_SystemTime_Hour = timeinfo.tm_hour+1;
  Dev_SystemTime_Minute = timeinfo.tm_min;
  Dev_SystemTime_Second = timeinfo.tm_sec;

  Serial.println("----------dx--------------");
  Serial.println(Dev_SystemTime_Year);
  Serial.println(Dev_SystemTime_Month);
  Serial.println(Dev_SystemTime_Day);
  Serial.println(Dev_SystemTime_WeekOfDay);
  Serial.println(Dev_SystemTime_Hour);
  Serial.println(Dev_SystemTime_Minute);
  Serial.println(Dev_SystemTime_Second);
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
            sprintf(NewsData[0].news_title, "1.%5s", temp);
            Serial.println(NewsData[0].news_title);
            strcpy(temp, root["result"]["data"]["list"][1]["title"]);
            sprintf(NewsData[1].news_title, "2.%5s", temp);
            Serial.println(NewsData[1].news_title);
            strcpy(temp, root["result"]["data"]["list"][2]["title"]);
            sprintf(NewsData[2].news_title, "3.%5s", temp);
            Serial.println(NewsData[2].news_title);
            strcpy(temp, root["result"]["data"]["list"][3]["title"]);
            sprintf(NewsData[3].news_title, "4.%13s                   ", temp);
            strcpy(temp, root["result"]["data"]["list"][4]["title"]);
            sprintf(NewsData[4].news_title, "5.%13s                   ", temp);
            strcpy(temp, root["result"]["data"]["list"][5]["title"]);
            sprintf(NewsData[5].news_title, "6.%13s                   ", temp);
            strcpy(temp, root["result"]["data"]["list"][6]["title"]);
            sprintf(NewsData[6].news_title, "7.%13s                   ", temp);
            strcpy(temp, root["result"]["data"]["list"][7]["title"]);
            sprintf(NewsData[7].news_title, "8.%13s                   ", temp);
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
            Covid19Data.confirmedNum = root["result"]["confirmedNum"];
            Covid19Data.confirmedIncr = root["result"]["confirmedIncr"];
            Covid19Data.externalConfirmedNum = root["result"]["externalConfirmedNum"];
            Covid19Data.externalConfirmedIncr = root["result"]["externalConfirmedIncr"];
            Covid19Data.asymptomaticNum = root["result"]["asymptomaticNum"];
            Covid19Data.asymptomaticIncr = root["result"]["asymptomaticIncr"];
            Covid19Data.deadNum = root["result"]["deadNum"];
            Covid19Data.deadIncr = root["result"]["deadIncr"];

            //@-获得图片数据
            // strcpy(temp, root["result"]);
            // sprintf(Covid19Data.dx_string, "%s", temp);
            // String dx_s = String(Covid19Data.dx_string);
            // int s_size = dx_s.length();
            // String dx_s1 = dx_s.substring(15,s_size-3); 

            // // Split the string into substrings
            // StringCount = 0;
            // while (dx_s1.length() > 0)
            // {
            //   int index = dx_s1.indexOf(',');
            //   if (index == -1) // No space found
            //   {
            //     json_gImage_play_W56H59[StringCount++] = dx_s1.toInt();
            //     break;
            //   }
            //   else
            //   {
            //     json_gImage_play_W56H59[StringCount++] = dx_s1.substring(0, index).toInt();
            //     dx_s1 = dx_s1.substring(index+1);
            //   }
            // }

            // Serial.println(json_gImage_play_W56H59[0]);
            // Serial.println(StringCount);
            // Serial.println(json_gImage_play_W56H59[StringCount-1]);

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


 
}
