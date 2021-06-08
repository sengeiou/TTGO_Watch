/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-esp8266-input-data-html-form/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

#include <Arduino.h>
#include <ArduinoJson.h>
#ifdef ESP32
  #include <WiFi.h>
  #include <AsyncTCP.h>
#else
  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>

#include <FS.h>
#include <SPIFFS.h>

AsyncWebServer server(80);

// REPLACE WITH YOUR NETWORK CREDENTIALS
const char* ssid = "wuyiyi";
const char* password = "dingxiao";

//*********SSID and Pass for AP**************//
const char* ssidAPConfig = "dx_esp32";
const char* passAPConfig = "dingxiao";

const char* PARAM_INPUT_1 = "ssid1";   //@-html文件中input属性的name值
const char* PARAM_INPUT_2 = "passkey1";
const char* PARAM_INPUT_3 = "ssid2";
const char* PARAM_INPUT_4 = "passkey2";

//@-新闻数据结构体
typedef struct {
    String inputMessage1;
    String inputParam1;

    String inputMessage2;
    String inputParam2;

    String inputMessage3;
    String inputParam3;

    String inputMessage4;
    String inputParam4;
} SPIFFS_JSONData_t;
SPIFFS_JSONData_t SPIFFS_JSONData;

// HTML web page to handle 3 input fields (input1, input2, input3)
// const char index_html[] PROGMEM = R"rawliteral(
// <!DOCTYPE HTML><html><head>
//   <title>ESP Input Form</title>
//   <meta name="viewport" content="width=device-width, initial-scale=1">
//   </head><body>
//   <form action="/get">
//     input1: <input type="text" name="input1">
//     <input type="submit" value="Submit">
//   </form><br>
//   <form action="/get">
//     input2: <input type="text" name="input2">
//     <input type="submit" value="Submit">
//   </form><br>
//   <form action="/get">
//     input3: <input type="text" name="input3">
//     <input type="submit" value="Submit">
//   </form>
// </body></html>)rawliteral";

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "dx Not found");
}

void setup() {

  Serial.begin(115200);

  SPIFFS.begin(true);
  delay(100); 

  //@-读取config.json
  Load_Config();

  //@-配置成wifi模式
  // WiFi.mode(WIFI_STA);
  // WiFi.begin(ssid, password);

  //@-配置成wifi路由模式
  WiFi.mode(WIFI_AP);
  Serial.println(WiFi.softAP(ssidAPConfig,passAPConfig) ? "soft-AP setup": "Failed to connect");
  delay(100);
  Serial.println(WiFi.softAPConfig( IPAddress(192,168,1,4),IPAddress(192,168,1,254), IPAddress(255,255,255,0))? "Configuring Soft AP" : "Error in Configuration");      
  Serial.println(WiFi.softAPIP());


  //@-WIFI连接信息
  // if (WiFi.waitForConnectResult() != WL_CONNECTED) {
  //   Serial.println("WiFi Failed!");
  //   return;
  // }
  // Serial.println();
  // Serial.print("IP Address: ");
  // Serial.println(WiFi.localIP());

  // Send web page with input fields to client  ----HTML文件为内存数组
  // server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
  //   request->send_P(200, "text/html", index_html);
  // });

   //@-HTML文件为SPIFFS文件系统
   server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/Test.html", "text/html");
  });

  //@-从SPIFFS文件系统中获得css文件
  // server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
  //   request->send(SPIFFS, "/style.css", "text/css");
  // });

  //@-从SPIFFS文件系统中获得图片
    server.on("/logo", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/logo.png", "image/png");
  });

  // Send a GET request to <ESP_IP>/get?input1=<inputMessage>
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {

    // GET input1 value on <ESP_IP>/get?input1=<inputMessage>
    // if (request->hasParam(PARAM_INPUT_1)) {
    //   inputMessage1 = request->getParam(PARAM_INPUT_1)->value();
    //   inputParam1 = PARAM_INPUT_1;
    // }
    // // GET input2 value on <ESP_IP>/get?input2=<inputMessage>
    // else if (request->hasParam(PARAM_INPUT_2)) {
    //   inputMessage2 = request->getParam(PARAM_INPUT_2)->value();
    //   inputParam2 = PARAM_INPUT_2;
    // }
    // // GET input3 value on <ESP_IP>/get?input3=<inputMessage>
    // else if (request->hasParam(PARAM_INPUT_3)) {
    //   inputMessage3 = request->getParam(PARAM_INPUT_3)->value();
    //   inputParam3 = PARAM_INPUT_3;
    // }
    // else {
    //   inputMessage = "No message sent";
    //   inputParam = "none";
    // }

    SPIFFS_JSONData.inputMessage1 = request->getParam(PARAM_INPUT_1)->value();
    SPIFFS_JSONData.inputParam1 = PARAM_INPUT_1;
    SPIFFS_JSONData.inputMessage2 = request->getParam(PARAM_INPUT_2)->value();
    SPIFFS_JSONData.inputParam2 = PARAM_INPUT_2;
    SPIFFS_JSONData.inputMessage3 = request->getParam(PARAM_INPUT_3)->value();
    SPIFFS_JSONData.inputParam3 = PARAM_INPUT_3;
    SPIFFS_JSONData.inputMessage4 = request->getParam(PARAM_INPUT_4)->value();
    SPIFFS_JSONData.inputParam4 = PARAM_INPUT_4;

    Serial.println(SPIFFS_JSONData.inputMessage1);
    Serial.println(SPIFFS_JSONData.inputMessage2);
    Serial.println(SPIFFS_JSONData.inputMessage3);
    Serial.println(SPIFFS_JSONData.inputMessage4);

    //@-SPIFFS写入参数
    Save_Set_Data();

    //@-发送配置成功页面
    request->send(200, "text/html", "HTTP GET request sent to your ESP on input field (" 
                                     + SPIFFS_JSONData.inputParam1 + ") with value: " + SPIFFS_JSONData.inputMessage1 +
                                     "<br><a href=\"/\">Return to Home Page</a>");
  });
  server.onNotFound(notFound);
  server.begin();
}

//@-保存参数到SPIFFS文件系统中的json文件
void Save_Set_Data()
{
  StaticJsonDocument<500> doc;

  doc["name"] = "dx";

  doc["ssid1"] = SPIFFS_JSONData.inputMessage1;
  doc["pass1"] = SPIFFS_JSONData.inputMessage2;
  doc["ssid2"] = SPIFFS_JSONData.inputMessage3;
  doc["pass2"] = SPIFFS_JSONData.inputMessage4;

  File file_config = SPIFFS.open("/config.json", FILE_WRITE);
 
  if (!file_config) {
    Serial.println("There was an error opening the file for writing");
    return;
  }
  // if (file.print("TEST")) {
  //   Serial.println("File was written");
  // } else {
  //   Serial.println("File write failed");
  // }

  serializeJson(doc, file_config);
 
  // file.close();

}


//@-加载SPIFFS文件系统中的json文件
void Load_Config()
{
  File file = SPIFFS.open("/config.json", FILE_READ);
  if (!file)
  {
    Serial.println("No Config Exist");
  } 
  else
  {
    size_t size = file.size();

    Serial.print("--------------json file size:");
    Serial.println(size);
    if(size == 0)
    {
      Serial.println("Config file empty !");
    }
    else
    {
      char buff_dx[256];
      file.readBytes(buff_dx, size);
      DynamicJsonDocument doc(128);
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

        Serial.print("ssid1:");
        Serial.println(root["ssid1"].as<String>());
        Serial.print("pass1:");
        Serial.println(root["pass1"].as<String>());
        Serial.print("ssid2:");
        Serial.println(root["ssid2"].as<String>());
        Serial.print("pass2:");
        Serial.println(root["pass2"].as<String>());
      }

    }
  }

}

void loop() {
  
}
