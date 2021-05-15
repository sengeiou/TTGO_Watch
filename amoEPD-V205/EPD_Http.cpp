 
#include "EPD_Http.h"
 
//web配置参数
const char* AP_SSID  = "Epd_Config"; //热点名称
String wifi_ssid = "";
String wifi_pass = "";
String scanNetworksID = "";//用于储存扫描到的WiFi
String http_read;

 I2C_BM8563 rtc1(I2C_BM8563_DEFAULT_ADDRESS, Wire1);
 
#define ROOT_HTML  "<!DOCTYPE html><html><head><title>WIFI Config For EPD</title><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"></head><style type=\"text/css\">.input{display: block; margin-top: 10px;}.input span{width: 100px; float: left; float: left; height: 36px; line-height: 36px;}.input input{height: 30px;width: 200px;}.btn{width: 120px; height: 35px; background-color: #000000; border:0px; color:#ffffff; margin-top:15px; margin-left:100px;}</style><body><form method=\"GET\" action=\"connect\"><label class=\"input\"><span>WiFi SSID</span><input type=\"text\" name=\"ssid\"></label><label class=\"input\"><span>WiFi PASS</span><input type=\"text\"  name=\"pass\"></label><input class=\"btn\" type=\"submit\" name=\"submit\" value=\"Submie\"> <p><span> Nearby wifi:</P></form>"
WebServer server(80);

bool addap(void);
void setUpHttpClient();
bool AutoConfig();
int  Auto_Web_Config(void);
void wifi_Config();
void RestoreWifi();
int WebHandle(void);

#define USE_SERIAL Serial

DynamicJsonDocument  docbuffer(2048);   //分配4k字节存储空间
String devicesn = "80000001";
const char *appid = "E9C8587A8BA6E96DFA23924FB6D15762";
const char *host = "http://screenms.yichiot.com/";
const char *query_addr      = "ScreenMSApp/device/realtime/get";
const char *query_reg       = "ScreenMSApp/device/reg";
const char *query_weather   = "ScreenMSApp/device/weather/real/get";
const char *query_todo      = "ScreenMSApp/device/todo/list";
const char *query_Calendar  = "ScreenMSApp/device/calendar/get";

HTTPClient http_client;
String req;
String rsp;

WiFiMulti wifiMulti;


RTC_DATA_ATTR EPD_Device_Struct epd_rtc_data;

bool addap(void)
{
  memset(epd_rtc_data.devsn,0,sizeof(epd_rtc_data.devsn));
  sprintf(epd_rtc_data.devsn,"%llx",ESP.getEfuseMac());
  Serial.printf("\n Devsn=%s",epd_rtc_data.devsn);
  
  
  if( AutoConfig())
    return true;
   else
    return  AutoConfig();


  
}

void setUpHttpClient()
{
  req = (String)host + query_addr;
  Serial.println(req);
  if (http_client.begin(req))
  {
    Serial.println("HTTPclient setUp done!");
  }

}

//用于上电自动连接WiFi
//需要连接WIFI时，需要调用本函数，自动获取系统内存储的WIFI参数。
bool AutoConfig()
{
  WiFi.begin("DX_JS","dingxiao");
  for (int i = 0; i < 5; i++)
  {
    int wstatus = WiFi.status();
    if (wstatus == WL_CONNECTED)
    {
      Serial.println("WIFI SmartConfig Success");
      Serial.printf("SSID:%s", WiFi.SSID().c_str());
      Serial.printf(", PSW:%s\r\n", WiFi.psk().c_str());
      Serial.print("LocalIP:");
      Serial.print(WiFi.localIP());
      Serial.print(" ,GateIP:");
      Serial.println(WiFi.gatewayIP());
      return true;
    }
    else
    {
      Serial.print("WIFI AutoConfig Waiting......");
      Serial.println(wstatus);
      delay(100);
    }
  }
  Serial.println("WIFI AutoConfig Faild!" );
  return false;
}
int Auto_Web_Config(void)
{
  Serial.println("------------Auto_Web_Config()-------------");
  wifi_Config();   //配置WEB和服务器参数
  Serial.println("---------Auto_Web_Config()Return----------");    
  return WebHandle();

  
}



//用于配置WiFi
//先扫描周围WIFI 并存储WIFI列表
//生成一个IP，用于客户端连接
//注册web处理的回调程序
void wifi_Config()
{
  Serial.println("------------wifi_Config()-------------");
  Serial.println("scan start");
  // 扫描附近WiFi
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0) {
    Serial.println("no networks found");
    scanNetworksID = "no networks found";
  } else {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i) {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
      scanNetworksID += "<P>-" + WiFi.SSID(i) + "</P>";
      delay(10);
    }
  }
  Serial.println("");

  WiFi.mode(WIFI_AP);//配置为AP模式
  boolean result = WiFi.softAP(AP_SSID, ""); //开启WIFI热点
  if (result)
  {
    IPAddress myIP = WiFi.softAPIP();
    //打印相关信息
    Serial.println("");
    Serial.print("Soft-AP IP address = ");
    Serial.println(myIP);
    Serial.println(String("MAC address = ")  + WiFi.softAPmacAddress().c_str());
    Serial.println("waiting ...");
  } else {  //开启热点失败
    Serial.println("WiFiAP Failed");
    delay(3000);
    ESP.restart();  //复位esp32
  }

  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }

  //首页
  server.on("/", []() {
    server.send(200, "text/html", ROOT_HTML + scanNetworksID + "</body></html>");
  });

  //连接
  server.on("/connect", []() {

    server.send(200, "text/html", "<html><body><font size=\"10\">successd,wifi connecting...<br />Please close this page manually.</font></body></html>");

    WiFi.softAPdisconnect(true);
    //获取输入的WIFI账户和密码
    wifi_ssid = server.arg("ssid");
    wifi_pass = server.arg("pass");
    server.close();
    WiFi.softAPdisconnect();
    Serial.println("WiFi Connect SSID:" + wifi_ssid + "  PASS:" + wifi_pass);

    //设置为STA模式并连接WIFI
    WiFi.mode(WIFI_STA);
    WiFi.begin(wifi_ssid.c_str(), wifi_pass.c_str());
    uint8_t Connect_time = 0; //用于连接计时，如果长时间连接不成功，复位设备
    while (WiFi.status() != WL_CONNECTED) {  //等待WIFI连接成功
      delay(500);
      Serial.print(".");
      Connect_time ++;
      if (Connect_time > 80) {  //长时间连接不上，复位设备
        Serial.println("Connection timeout, check input is correct or try again later!");
        delay(3000);
        ESP.restart();
      }
    }
    Serial.println("");
    Serial.println("WIFI Config Success");
    Serial.printf("SSID:%s", WiFi.SSID().c_str());
    Serial.print("  LocalIP:");
    Serial.print(WiFi.localIP());
    Serial.println("");

  });
  server.begin();
  Serial.println("---------wifi_Config()Return----------");    
}
//清除WIFI配置信息
void RestoreWifi()
{
  delay(15); 
  esp_wifi_restore();
  delay(15);
}
int wait_time;
int WebHandle(void)
{  
  Serial.println("---------WebHandle()----------"); 

  
  wait_time = 0;
  while (1)
  {
    server.handleClient();    //此函数会自动退出  复位WIFI
    while (WiFi.status() == WL_CONNECTED) {
      Serial.println("---------WebHandle() Sucess----------"); 
      return 1;
    }
 
    wait_time++;
    delay(10);
    if (wait_time > 12000)    //超时退出 120s
    {
      Serial.println("Wait config time out ,restart epd ");
      ESP.restart();  //复位esp32
    }
  }
}


//http 获取RTC时间
int Http_Set_Rtc(void)
{
  int http_code;
  Serial.println("------------Http_Set_Rtc()-------------");
  I2C_BM8563 rtc1(I2C_BM8563_DEFAULT_ADDRESS, Wire1);
  Serial.println("HTTP Start post");
  StaticJsonDocument<200> doc;
  doc["devsn"] = epd_rtc_data.devsn;;
  String ouput;
  // 序列化
  serializeJsonPretty(doc, ouput);
  Serial.println("HTTP  post json string is");
  Serial.println(ouput);
  //执行HTTP POST
  http_code =  Http_Post(query_addr, ouput.c_str());
  if (http_code == 200)
  {
    struct tm *p;
    char s[100];

    DynamicJsonDocument doc(1024);
    // json序列化
    deserializeJson(doc, http_read);
    JsonObject obj = doc.as<JsonObject>();
    JsonObject datas = obj["datas"];

    int timestr = datas["time"];
    Serial.println("RTC GET timestamp is");
    Serial.println(timestr);

    timestr = timestr + 8 * 3600;
    p = gmtime((time_t*)&timestr);
    strftime(s, sizeof(s), "%Y-%m-%d %H:%M:%S", p);


    Serial.println(s);
    Wire1.begin(21, 22);
    rtc1.begin();
    // Set RTC Date
    I2C_BM8563_DateTypeDef dateStruct;
    dateStruct.weekDay = p->tm_wday + 1;
    dateStruct.month = p->tm_mon + 1;
    dateStruct.date = p->tm_mday;
    dateStruct.year = p->tm_year + 1900;
    rtc1.setDate(&dateStruct);

    // Set RTC Time
    I2C_BM8563_TimeTypeDef timeStruct;
    timeStruct.hours   = p->tm_hour;
    timeStruct.minutes =  p->tm_min;
    timeStruct.seconds =  p->tm_sec;
    rtc1.setTime(&timeStruct);

  }
  Serial.println(http_code);
  Serial.println("---------Http_Set_Rtc()Return----------");  
}
//////////////////////Device Regedit//////////////////
//设备注册，获取接口token

int Http_Get_Token(void)
{
  int http_code;
  Serial.println("------------Http_Get_Token()-------------");
  Serial.println("HTTP Start post");
  StaticJsonDocument<200> doc;

 
  doc["devsn"] =  epd_rtc_data.devsn;


  
  doc["appid"] = "E9C8587A8BA6E96DFA23924FB6D15762";
  doc["type"] = "screenms";
  doc["softver"] = 100;
  doc["hardver"] = 100;
  doc["confver"] = 100;


  String ouput;
  // 序列化
  serializeJsonPretty(doc, ouput);
  Serial.println("HTTP  post json string is");
  Serial.println(ouput);
  //执行HTTP POST
  http_code =  Http_Post(query_reg, ouput.c_str());
  if (http_code == 200)
  {
    DynamicJsonDocument doc(1024);
    // json序列化
    deserializeJson(doc, http_read);
    JsonObject obj = doc.as<JsonObject>();
    bool tokenResult = obj["result"];
    if (tokenResult) {
      JsonObject datas = obj["datas"];
      const char* tokenbuf = datas["token"];
      memset(epd_rtc_data.token_str, 0, sizeof(epd_rtc_data.Weather_City));
      ///   String weatherdatas=datas["city"];
      ///   sprintf(epd_rtc_data.Weather_City,"%s",weatherdatas);

      sprintf(epd_rtc_data.token_str, "%s", tokenbuf);

      Serial.println(tokenbuf);
      Serial.println("Http Get Token is---------");

      Serial.println(epd_rtc_data.token_str);
    }
  }
  Serial.println(http_code);
  Serial.println("---------Http_Get_Token()Return----------");  
}
//////////////////////Device Get RealWeather//////////////////
//
int Http_Get_RealWeather(void)
{
  int http_code;
  Serial.println("------------Http_Get_RealWeather()-------------");
  Serial.println("HTTP Start post");
  StaticJsonDocument<200> doc;
  doc["devsn"] = epd_rtc_data.devsn;;
  doc["token"] = epd_rtc_data.token_str;
  String ouput;
  // 序列化
  serializeJsonPretty(doc, ouput);
  Serial.println("HTTP  post json string is");
  Serial.println(ouput);
  //执行HTTP POST
  http_code =  Http_Post(query_weather, ouput.c_str());
  if (http_code == 200)
  {
    DynamicJsonDocument doc(1024);
    // json序列化
    deserializeJson(doc, http_read);
    JsonObject obj = doc.as<JsonObject>();
    epd_rtc_data.weather_result = obj["result"];
    if (epd_rtc_data.weather_result)
    {
      JsonObject datas = obj["datas"];
      memset(epd_rtc_data.Weather_City, 0, sizeof(epd_rtc_data.Weather_City));
      memset(epd_rtc_data.Weather_Tmp_High, 0, sizeof(epd_rtc_data.Weather_Tmp_High));
      memset(epd_rtc_data.Weather_Tmp_Low, 0, sizeof(epd_rtc_data.Weather_Tmp_Low));
      memset(epd_rtc_data.Weather_Tmp_Real, 0, sizeof(epd_rtc_data.Weather_Tmp_Real));
      memset(epd_rtc_data.Weather_Type, 0, sizeof(epd_rtc_data.Weather_Type));
      memset(epd_rtc_data.Weather_hum, 0, sizeof(epd_rtc_data.Weather_hum));
      memset(epd_rtc_data.Weather_winddir, 0, sizeof(epd_rtc_data.Weather_winddir));
      memset(epd_rtc_data.Weather_windpwr, 0, sizeof(epd_rtc_data.Weather_windpwr));
      memset(epd_rtc_data.Weather_uptime, 0, sizeof(epd_rtc_data.Weather_uptime));
      memset(epd_rtc_data.Weather_Type, 0, sizeof(epd_rtc_data.Weather_Type));
  


      String weatherdatas = datas["city"];
      sprintf(epd_rtc_data.Weather_City, "%s", weatherdatas);
      String weatherdatas1 = datas["high"];
      sprintf(epd_rtc_data.Weather_Tmp_High, "%s", weatherdatas1);
      String weatherdatas2 = datas["low"];
      sprintf(epd_rtc_data.Weather_Tmp_Low, "%s", weatherdatas2);
      String weatherdatas3 = datas["tempe"];
      sprintf(epd_rtc_data.Weather_Tmp_Real, "%s", weatherdatas3);
      String weatherdatas4 = datas["humidity"];
      sprintf(epd_rtc_data.Weather_hum, "%s", weatherdatas4);
      String weatherdatas5 = datas["winddir"];
      sprintf(epd_rtc_data.Weather_winddir, "%s", weatherdatas5);
      String weatherdatas6 = datas["windpwr"];
      sprintf(epd_rtc_data.Weather_windpwr, "%s", weatherdatas6);
      String weatherdatas7 = datas["utime"];
      sprintf(epd_rtc_data.Weather_uptime, "%s", weatherdatas7);
      String weatherdatas8 = datas["type"];
      sprintf(epd_rtc_data.Weather_Type, "%s", weatherdatas8);

     epd_rtc_data.Weather_TypeCode = datas["typecode"];
     Serial.printf("今日天气 %s code %d\n",epd_rtc_data.Weather_Type,epd_rtc_data.Weather_TypeCode);


      Serial.printf("%s %s更新 温度:%s-%s℃ 湿度:%s% 风向:%s 风力:%s\n", epd_rtc_data.Weather_City, epd_rtc_data.Weather_uptime, epd_rtc_data.Weather_Tmp_Low, epd_rtc_data.Weather_Tmp_High, epd_rtc_data.Weather_hum, epd_rtc_data.Weather_winddir, epd_rtc_data.Weather_windpwr);
      //  JsonObject datas = obj["now"];
    }
  }
  
  Serial.println(http_code);
  Serial.println("---------Http_Get_RealWeather()Return----------");    
}
//////////////////////Device Get Calendar//////////////////
//
int Http_Get_Calendar(void)
{
  int http_code;
  Serial.println("------------Http_Get_Calendar()-------------");
  Serial.println("HTTP Start post");
  StaticJsonDocument<200> doc;
  doc["devsn"] = epd_rtc_data.devsn;;
  doc["token"] = epd_rtc_data.token_str;

  String ouput;
  // 序列化
  serializeJsonPretty(doc, ouput);
  Serial.println("HTTP  post json string is");
  Serial.println(ouput);
  //执行HTTP POST
  http_code =  Http_Post(query_Calendar, ouput.c_str());
  if (http_code == 200)
  {
    DynamicJsonDocument doc(2048);
    // json序列化
    deserializeJson(doc, http_read);
    JsonObject obj = doc.as<JsonObject>();
    epd_rtc_data.Calendar_result  = obj["result"];
    if (epd_rtc_data.Calendar_result)
    {
      Serial.println("Json to char...");
      JsonObject datas = obj["datas"];
      JsonObject info = datas["info"];
      memset(epd_rtc_data.Calendar_animalsYear, 0, sizeof(epd_rtc_data.Calendar_animalsYear));
      memset(epd_rtc_data.Calendar_lunarYear, 0, sizeof(epd_rtc_data.Calendar_lunarYear));
      memset(epd_rtc_data.Calendar_suit, 0, sizeof(epd_rtc_data.Calendar_suit));
      memset(epd_rtc_data.Calendar_avoid, 0, sizeof(epd_rtc_data.Calendar_avoid));
      memset(epd_rtc_data.Calendar_lunar, 0, sizeof(epd_rtc_data.Calendar_lunar));
      memset(epd_rtc_data.Calendar_holiday, 0, sizeof(epd_rtc_data.Calendar_holiday));
      Serial.println("Json to char...");
      const char* CalendarInfo0 = info["animalsYear"];
      sprintf(epd_rtc_data.Calendar_animalsYear, "%s", CalendarInfo0);
      Serial.println(epd_rtc_data.Calendar_animalsYear);

      const char*  CalendarInfo1 = info["lunarYear"];
      sprintf(epd_rtc_data.Calendar_lunarYear, "%s", CalendarInfo1);
      Serial.println(epd_rtc_data.Calendar_lunarYear);

      const char* CalendarInfo2 = info["suit"];
      sprintf(epd_rtc_data.Calendar_suit, "%s", CalendarInfo2);
      Serial.println(epd_rtc_data.Calendar_suit);

      const char*  CalendarInfo3 = info["avoid"];
      sprintf(epd_rtc_data.Calendar_avoid, "%s", CalendarInfo3);
      Serial.println(epd_rtc_data.Calendar_avoid);

      const char* CalendarInfo4 = info["lunar"];
      sprintf(epd_rtc_data.Calendar_lunar, "%s", CalendarInfo4);
      Serial.println(epd_rtc_data.Calendar_lunar);

      const char* CalendarInfo5 = info["holiday"];
      sprintf(epd_rtc_data.Calendar_holiday, "%s", CalendarInfo5);


      Serial.println(epd_rtc_data.Calendar_holiday);
    }
  }
  Serial.println(http_code);
  Serial.println("---------Http_Get_Calendar()Return----------");  
}
      I2C_BM8563_DateTypeDef dateStruct1;
      I2C_BM8563_TimeTypeDef timeStruct1;
uint16_t  NewGetRtcMinute(void)
{
  Wire1.begin(21, 22);
  rtc1.begin();
  // Get RTC
  rtc1.getTime(&timeStruct1);
  return timeStruct1.minutes;
}
uint16_t  NewGetRtcHour(void)
{
  Wire1.begin(21, 22);
  rtc1.begin();
  // Get RTC
  rtc1.getTime(&timeStruct1);
  return timeStruct1.hours;
}

//////////////////////Device Get Todo List//////////////////
//
int Http_Get_TodoList(void)
{
  int http_code;
  Serial.println("------------Http_Get_TodoList()-------------");
  Serial.println("HTTP Start post");
  StaticJsonDocument<200> doc;
  doc["devsn"] = epd_rtc_data.devsn;;
  doc["token"] = epd_rtc_data.token_str;

  String ouput;
  // 序列化
  serializeJsonPretty(doc, ouput);
  Serial.println("HTTP  post json string is");
  Serial.println(ouput);
  //执行HTTP POST
  http_code =  Http_Post(query_todo, ouput.c_str());
  if (http_code == 200)
  {
    DynamicJsonDocument doc(2048);
    // json序列化
    deserializeJson(doc, http_read);
    JsonObject obj = doc.as<JsonObject>();
    epd_rtc_data.Todo_result = obj["result"];
    if (epd_rtc_data.Todo_result)
    {
      Serial.println("Json to char...");
      JsonObject datas = obj["datas"];

      epd_rtc_data.Todo_count=datas["count"];
      if(epd_rtc_data.Todo_count>0)   //todo 非空白
      {
        JsonArray  todolistarry = datas["list"];
        Serial.println("Get JSON List");
        for(int listi=0;listi<epd_rtc_data.Todo_count;listi++)    //逐一提取list 列表内容
        {
           Serial.printf("todo list %d\n",listi);
           const char *todonews=todolistarry[listi]["msg"];
           memset(epd_rtc_data.Todo_New_List[listi], 0, sizeof(epd_rtc_data.Todo_New_List[listi]));
           sprintf(epd_rtc_data.Todo_New_List[listi], "%s", todonews);
           Serial.println(epd_rtc_data.Todo_New_List[listi]);

           //更新时间
           memset(epd_rtc_data.Todo_Uptime,0,sizeof(epd_rtc_data.Todo_Uptime));

           sprintf(epd_rtc_data.Todo_Uptime,"%d:%d",NewGetRtcHour(),NewGetRtcMinute());
            
              
        }
        
        
        
      }
 
 

    }
  }
  Serial.println(http_code);
  Serial.println("---------Http_Get_TodoList()Return----------");  
}



//////////////////////////////////////////////////
//http post
//webq 传入的字符串
//sendjson 需要发送的json字符串
//revjson 从服务器获取得到的json字符串数据
//返回：http_code
int Http_Post(String webp, String sendjson)
{
  Serial.println("------------Http_Post()-------------");
  req = (String)host + webp;    //生成url
  Serial.println("HTTP Start post URL is");
  Serial.println(req);
 

  if (http_client.begin(req))
  {
    Serial.println("HTTPclient setUp done!");
  }
  // 添加http头
  http_client.addHeader("Content-Type", "application/json");
  // 发送请求
  int http_code = http_client.POST((uint8_t*)sendjson.c_str(), sendjson.length());
  if (http_code == 200)
  {
    http_read = http_client.getString();
    Serial.println(http_read);
  }
  Serial.println("---------Http_Post()Return----------");
  return http_code;
}
//////////////////////////////////////////////////

//////////////////////////////////////////////////


int GetNews(const char **news_str)
{
  int i;
  HTTPClient http;
  String  errjson = "err_code";
  USE_SERIAL.print("[HTTP] begin...\n");
  http.begin("http://free-api.ledcfg.com/base/info/searchnews?sn=amobbs&mode=0"); //HTTP
  USE_SERIAL.print("[HTTP] GET...\n");
  // start connection and send HTTP header
  int httpCode = http.GET();
  // httpCode will be negative on error
  if (httpCode > 0) {
    // HTTP header has been send and Server response header has been handled
    USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);
    // file found at server
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      USE_SERIAL.println(payload);
      DeserializationError error = deserializeJson(docbuffer, payload);   //反序列化json
      JsonObject root = docbuffer.as<JsonObject>();
      USE_SERIAL.printf("deserial ok \r\n");
      if (root != NULL)
      {
        USE_SERIAL.printf("root is ok \r\n");
        int error_code = root[errjson];
        USE_SERIAL.printf("root get err code is %d\r\n", error_code);
        if (error_code == 0)   //无错误 输出
        {
          i = root["result"].size();
          USE_SERIAL.printf("result size is%d\r\n", i);

          for (int j = 0; j < i; j++)
          {
            news_str[j] = root["result"][j];
            USE_SERIAL.printf("%s\r\n", news_str[j]);
          }
        }
      }
    }
  } else {
    USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());

  }

  http.end();

  return i;
}
 
