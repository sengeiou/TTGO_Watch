#include <FS.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <TFT_eSPI.h> 
#include <SPI.h>
#include <ArduinoJson.h>
// JPEG decoder library
#include <JPEGDecoder.h>

// Return the minimum of two values a and b
#define minimum(a,b)     (((a) < (b)) ? (a) : (b))

TFT_eSPI tft = TFT_eSPI(); 

String serverName_dxjson = "http://www.dx1023.com/dxjson/";

//@-DXJSON数据结构体
typedef struct {
  char pdf_title[128];
  char pdf_image[512];
} DXJSONData_t;
DXJSONData_t DXJSONData;

//@-获取信息网站JSON数据
void WIFI_Get_JsonInfo(String serverName, int Data_Mode, String Http_source)
{
  char temp[256];
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

      if(http_payload_size > 100)
      {
        //@3-从网站提供的API接口中获取信息，并将数据json化
        DynamicJsonDocument doc(6144);  //3072
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

          //@-DXJSON数据
          if(Data_Mode == 1)
          {
            strcpy(temp, root["result"]["pdf_name"]);
            sprintf(DXJSONData.pdf_title, "%s", temp);
            strcpy(temp, root["result"]["pdf_image"]);
            sprintf(DXJSONData.pdf_image, "%s", temp);

            Serial.println(DXJSONData.pdf_title);
            Serial.println(DXJSONData.pdf_image);
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

void setup()
{
    Serial.begin(115200);
    if (!SPIFFS.begin(true)) {
      Serial.println("SPIFFS initialisation failed!");
      return;
    }

    tft.init();
    tft.setRotation(0);
    tft.fillScreen(TFT_RED);

    // Set device as a Wi-Fi Station
    WiFi.mode(WIFI_STA);
    // Connect to Wi-Fi
    Serial.print("Connecting to ");
    // WiFi.begin("wuyiyi", "dingxiao");
    WiFi.begin("DX_JS", "dingxiao");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    if(WiFi.status() == WL_CONNECTED)
    {
        Serial.println("");
        Serial.println("WiFi connected.");
    }

    //@-获取json数据
    WIFI_Get_JsonInfo(serverName_dxjson, 1, "DX_JSON");

    // downloadAndSaveFile("/test_download.jpg","http://www.dx1023.com/media/images/test_dddxx.jpg");
    downloadAndSaveFile("/test_download.jpg",String(DXJSONData.pdf_image));
    // bitmapDemo();
    ReadImageFile();

    drawJpeg("/test_download.jpg", 0, 0);
}


void jpegRender(int xpos, int ypos) {

  // retrieve infomration about the image
  uint16_t  *pImg;
  uint16_t mcu_w = JpegDec.MCUWidth;
  uint16_t mcu_h = JpegDec.MCUHeight;
  uint32_t max_x = JpegDec.width;
  uint32_t max_y = JpegDec.height;

  uint32_t min_w = minimum(mcu_w, max_x % mcu_w);
  uint32_t min_h = minimum(mcu_h, max_y % mcu_h);

  // save the current image block size
  uint32_t win_w = mcu_w;
  uint32_t win_h = mcu_h;
  max_x += xpos;
  max_y += ypos;

  // read each MCU block until there are no more
  while ( JpegDec.readSwappedBytes()) { // Swap byte order so the SPI buffer can be used

    // save a pointer to the image block
    pImg = JpegDec.pImage;

    // calculate where the image block should be drawn on the screen
    int mcu_x = JpegDec.MCUx * mcu_w + xpos;  // Calculate coordinates of top left corner of current MCU
    int mcu_y = JpegDec.MCUy * mcu_h + ypos;

    // check if the image block size needs to be changed for the right edge
    if (mcu_x + mcu_w <= max_x) win_w = mcu_w;
    else win_w = min_w;

    // check if the image block size needs to be changed for the bottom edge
    if (mcu_y + mcu_h <= max_y) win_h = mcu_h;
    else win_h = min_h;

    // copy pixels into a contiguous block
    if (win_w != mcu_w)
    {
      uint16_t *cImg;
      int p = 0;
      cImg = pImg + win_w;
      for (int h = 1; h < win_h; h++)
      {
        p += mcu_w;
        for (int w = 0; w < win_w; w++)
        {
          *cImg = *(pImg + w + p);
          cImg++;
        }
      }
    }

    // draw image MCU block only if it will fit on the screen
    if ( ( mcu_x + win_w) <= tft.width() && ( mcu_y + win_h) <= tft.height())
    {
      tft.pushRect(mcu_x, mcu_y, win_w, win_h, pImg);
    }

    else if ( ( mcu_y + win_h) >= tft.height()) JpegDec.abort();
  }
}

void drawJpeg(const char *filename, int xpos, int ypos) {

  // Open the named file (the Jpeg decoder library will close it after rendering image)
//   fs::File jpegFile = SPIFFS.open( filename, "r");    // File handle reference for SPIFFS
  //  File jpegFile = SD.open( filename, FILE_READ);  // or, file handle reference for SD library

  File jpegFile = SPIFFS.open(filename, FILE_READ);

  if ( !jpegFile ) {
    Serial.print("ERROR: File \""); Serial.print(filename); Serial.println ("\" not found!");
    return;
  }

  // Use one of the three following methods to initialise the decoder:
  //boolean decoded = JpegDec.decodeFsFile(jpegFile); // Pass a SPIFFS file handle to the decoder,
  //boolean decoded = JpegDec.decodeSdFile(jpegFile); // or pass the SD file handle to the decoder,
  boolean decoded = JpegDec.decodeFsFile(filename);  // or pass the filename (leading / distinguishes SPIFFS files)
  // Note: the filename can be a String or character array type
  if (decoded)  jpegRender(xpos, ypos);
  else Serial.println("Jpeg file format not supported!");
}

void ReadImageFile()
{
  File file = SPIFFS.open("/test_download.jpg", FILE_READ);
  if (!file)
  {
    Serial.println("No Config-> white define");
  } 
  else
  {
    size_t size = file.size();
    Serial.print("--------------json file size:");
    Serial.println(size);
    if(size == 0)
    {
      Serial.println("Config file empty-> white define");
    }
  }
}

void downloadAndSaveFile(String fileName, String  url){
  
  HTTPClient http;

  Serial.println("[HTTP] begin...\n");
  Serial.println(fileName);
  Serial.println(url);
  http.begin(url);
  
  Serial.printf("[HTTP] GET...\n", url.c_str());
  // start connection and send HTTP header
  int httpCode = http.GET();
  if(httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);
      Serial.printf("[FILE] open file for writing %d\n", fileName.c_str());
      
      File file = SPIFFS.open(fileName, FILE_WRITE);

      // file found at server
      if(httpCode == HTTP_CODE_OK) {

          // get lenght of document (is -1 when Server sends no Content-Length header)
          int len = http.getSize();

          // create buffer for read
          uint8_t buff[128] = { 0 };

          // get tcp stream
          WiFiClient * stream = http.getStreamPtr();

          // read all data from server
          while(http.connected() && (len > 0 || len == -1)) {
              // get available data size
              size_t size = stream->available();
              if(size) {
                  // read up to 128 byte
                  int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
                  // write it to Serial
                  //Serial.write(buff, c);
                  file.write(buff, c);
                  if(len > 0) {
                      len -= c;
                  }
              }
              delay(1);
          }

          Serial.println();
          Serial.println("[HTTP] connection closed or file end.\n");
          Serial.println("[FILE] closing file\n");
          file.close();
          
      }
      
  }
  http.end();
}

void loop()
{


}