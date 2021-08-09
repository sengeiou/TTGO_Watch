
#define ARDUINOJSON_DECODE_UNICODE 1

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>   //V5.13.5
 
//@-浏览器安全码
// const char* root_ca= \
// "-----BEGIN CERTIFICATE-----\n" \
// "MIIGvjCCBaagAwIBAgIRAKL7IEo7D+v9u0G4ItYCJYwwDQYJKoZIhvcNAQELBQAw\n" \
// "gY8xCzAJBgNVBAYTAkdCMRswGQYDVQQIExJHcmVhdGVyIE1hbmNoZXN0ZXIxEDAO\n" \
// "BgNVBAcTB1NhbGZvcmQxGDAWBgNVBAoTD1NlY3RpZ28gTGltaXRlZDE3MDUGA1UE\n" \
// "AxMuU2VjdGlnbyBSU0EgRG9tYWluIFZhbGlkYXRpb24gU2VjdXJlIFNlcnZlciBD\n" \
// "QTAeFw0yMDAzMTcwMDAwMDBaFw0yMjA2MTkwMDAwMDBaMB8xHTAbBgNVBAMMFCou\n" \
// "b3BlbndlYXRoZXJtYXAub3JnMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKC\n" \
// "AQEA2DMTq6QbiQ6N/PK6u6dv8J1w5/w/GLm1d7J3daL80/15qRlsxUEpM78/OWmE\n" \
// "s60kKSfyOVyxOHrVoXMfEhIxATdYQtRtN2JQEFYDkRauvVgr5eXQO2EJZXBZUb2C\n" \
// "0dLFMD2WtrQGl7059kCOBlA/vX2+uTIQwFx/qZyVKkhzgdthtoDQ5jDzx7scDM0U\n" \
// "9c/be/aWNPzoJV1HK37luC0nHUyT0zDpXMt82DgoCRix9z9RzDNkyjsPW2qP/pOE\n" \
// "RpXk0z49jOFqtUxTtR9HfbKoeQ/RobxD2fG5P1cfunZ2lU3lyl5PeKbmMlSdSlci\n" \
// "4OuileGdauTqgU254X7bB/9iTQIDAQABo4IDgjCCA34wHwYDVR0jBBgwFoAUjYxe\n" \
// "xFStiuF36Zv5mwXhuAGNYeEwHQYDVR0OBBYEFP2HTXuP9/WVxbHQk4RHPpXCLktU\n" \
// "MA4GA1UdDwEB/wQEAwIFoDAMBgNVHRMBAf8EAjAAMB0GA1UdJQQWMBQGCCsGAQUF\n" \
// "BwMBBggrBgEFBQcDAjBJBgNVHSAEQjBAMDQGCysGAQQBsjEBAgIHMCUwIwYIKwYB\n" \
// "BQUHAgEWF2h0dHBzOi8vc2VjdGlnby5jb20vQ1BTMAgGBmeBDAECATCBhAYIKwYB\n" \
// "BQUHAQEEeDB2ME8GCCsGAQUFBzAChkNodHRwOi8vY3J0LnNlY3RpZ28uY29tL1Nl\n" \
// "Y3RpZ29SU0FEb21haW5WYWxpZGF0aW9uU2VjdXJlU2VydmVyQ0EuY3J0MCMGCCsG\n" \
// "AQUFBzABhhdodHRwOi8vb2NzcC5zZWN0aWdvLmNvbTAzBgNVHREELDAqghQqLm9w\n" \
// "ZW53ZWF0aGVybWFwLm9yZ4ISb3BlbndlYXRoZXJtYXAub3JnMIIB9gYKKwYBBAHW\n" \
// "eQIEAgSCAeYEggHiAeAAdwBGpVXrdfqRIDC1oolp9PN9ESxBdL79SbiFq/L8cP5t\n" \
// "RwAAAXDobGj8AAAEAwBIMEYCIQDuoxRU3qxvOhsXh/vQPwAzBQfmu0b76RYKY27r\n" \
// "3IjeuwIhAKhiaG0C9WMqsBNviTNJHl8iUZppSoDbreFWKU3ju715AHYA36Veq2iC\n" \
// "Tx9sre64X04+WurNohKkal6OOxLAIERcKnMAAAFw6GxoyAAABAMARzBFAiEAiPLZ\n" \
// "oR9BVGbeBKcZWWCWe5khT1jrbwqFFs1qqciHhmUCICNPG3dRIueExiu3HF6tUiNb\n" \
// "rlGF/mf9Efr3JkAkqGsZAHUAQcjKsd8iRkoQxqE6CUKHXk4xixsD6+tLx2jwkGKW\n" \
// "BvYAAAFw6Gxo7AAABAMARjBEAiAzzodBqseRU0wn7ukh37SvTOjmv8vpayKuZ4AE\n" \
// "ut06BAIgArnrQObBVZU87a6ubmSWGHPiEi8cyPYdqZkMVycT3TgAdgBvU3asMfAx\n" \
// "GdiZAKRRFf93FRwR2QLBACkGjbIImjfZEwAAAXDobGnaAAAEAwBHMEUCIGo9M7aa\n" \
// "TjzbYPbR16+gwPnAGNiZI0ujRTDXRUJsW+D8AiEAgexT/9i23R7/XZfh5sL1Q9E/\n" \
// "pE40zy1wXC1O3BHvz2MwDQYJKoZIhvcNAQELBQADggEBANJ4pa0tYp5QOtGy1RxM\n" \
// "hcX2WydaU89WwySUB41pxbXBvaRLQyFBzC/COjPyN6zR52irYeBr0uFLLmwkaZfg\n" \
// "eavkaExosslVP9g1js4j7wAKR5CdlEJfgw4eTxu8LAx5WUhm66HaMQol2neSyky2\n" \
// "XPZt4KvZC9Fk/0x28JpXbMpckpH1/VpWPz3ulQw1/9TgV0+saRpFaKVXoZT5IObo\n" \
// "j6cAp85OGBmRNJFypFFZRvy85aPJCP8IIyNoC9MoZIQ2VEuXQMTrIDU14Y46BTDq\n" \
// "HaolM6WQZl42iGBzqJcOF2PGzcZ5YUahZW1GMxwB3NCyugR93FMCwtM4Wip6Ja5Q\n" \
// "5fs=\n" \
// "-----END CERTIFICATE-----\n";

HTTPClient http;
HTTPClient http1;
//@-json数据格式化
// DynamicJsonBuffer
StaticJsonBuffer<1500> jsonBuffer;   //@-arduinojson使用老版本库5.13.5，新的版本库使用6.18.0
String App_Html_Add = "https://api.openweathermap.org/data/2.5/weather?q=hangzhou&appid=8874f49040c9bad35a788cbdb34f5444";  //-当天数据
String App_Html_Add1 = "https://api.openweathermap.org/data/2.5/forecast?q=hangzhou&appid=8874f49040c9bad35a788cbdb34f5444";     //-5 Day / 3 Hour Forecast
String App_Html_Add2 = "http://wthrcdn.etouch.cn/weather_mini?citykey=101210101";

// String App_Html_Add = "https://v0.yiketianqi.com/api?version=v9&appid=61739339&appsecret=RnWxRr51";  --中国网站

// https://v0.yiketianqi.com/api?version=v9&appid=61739339&appsecret=RnWxRr51

// String App_Html_Add = "https://lab.isaaclin.cn/nCoV/api/overall";

// https://api.openweathermap.org/data/2.5/forecast?q=hangzhou&appid=8874f49040c9bad35a788cbdb34f5444  -- 5 day weather forecast

// http://api.openweathermap.org/data/2.5/find?q=London&appid=8874f49040c9bad35a788cbdb34f5444  -- 未来5天的天气，但不支持中国城市





//@-获取天气任务
void GetweatherTask(void *pvParameters) 
{
  if((http.connected() == true) || (http1.connected() == true))
  {
    weather_use_http_id = weather_use_http_last_id;
    Serial.println("http wait close..."); 
    vTaskDelay(500);
    weather_begin_flag = false;
    vTaskDelay(100);
    vTaskDelete(NULL);
  }
  else
  {
    Serial.print("weather start http id:"); 
    Serial.println(weather_use_http_id); 

    while((Getweather_tick < 5) && (weather_json_flag == false))
    {
        //@-天气数据获取次数
        Getweather_tick = Getweather_tick + 1;

        //@-判断wifi连接
        if((WiFi.status() == WL_CONNECTED)) 
        { 
          // http.begin("http://www.dx1023.com/video"); //Specify the URL

          //@-连接openweathermap网址API
          // http.begin("https://api.openweathermap.org/data/2.5/weather?q=hangzhou&appid=8874f49040c9bad35a788cbdb34f5444",root_ca);
          
          // {"coord":{"lon":120.16,"lat":30.29},
          //  "weather":[{"id":803,"main":"Clouds","description":"broken clouds","icon":"04d"}],
          //  "base":"stations",
          //  "main":{"temp":296.94,"feels_like":298.52,"temp_min":294.26,"temp_max":298.15,"pressure":1010,"humidity":64},
          //  "visibility":10000,
          //  "wind":{"speed":0.89,"deg":90,"gust":4.47},
          //  "clouds":{"all":69},
          //  "dt":1600916525,
          //  "sys":{"type":3,"id":2035262,"country":"CN","sunrise":1600897724,"sunset":1600941243},
          //  "timezone":28800,
          //  "id":1808926,
          //  "name":"Hangzhou",
          //  "cod":200}

          if(weather_use_http_id == 0)
          http.begin(App_Html_Add);
          else if(weather_use_http_id == 1)
          http1.begin(App_Html_Add);
          
          //@-HTTP返回
          int httpCode;
          if(weather_use_http_id == 0)
          httpCode = http.GET(); //Make the request
          else if(weather_use_http_id == 1)
          httpCode = http1.GET(); //Make the request

          Serial.println(httpCode);
          
          //@-HTTP返回正常  (httpCode > 0) 
          if (httpCode == 200) 
          { 
                //@-获得数据
                String payload;

                if(weather_use_http_id == 0)
                payload = http.getString();
                else if(weather_use_http_id == 1)
                payload = http1.getString();

                // Serial.println(httpCode);
                Serial.println(payload);

                // JsonObject& message = jsonBuffer.parseObject((char *)payload);
                JsonObject& message = jsonBuffer.parseObject(payload);
                if (!message.success()) 
                {
                  weather_json_flag = false;
                  Serial.println("JSON parse failed"); 
                }
                else
                weather_json_flag = true;

              #if 1
              JsonObject& coord_data = message["coord"];
              int lon = coord_data["lon"];
              Serial.print("lon:");
              Serial.println(lon);

              String base = message["base"];
              Serial.print("base:");
              Serial.println(base);

              // JsonObject& weather_data = message["weather"];
              int id = message["weather"][0]["id"]; 
              // const char* main = message["weather"][0]["main"]; 
              String main = message["weather"][0]["main"]; 
              String description = message["weather"][0]["description"];

              weather_description = description;

              // Output to serial monitor
              Serial.print("id:");
              Serial.println(id);
              Serial.print("main:");
              Serial.println(main);
              Serial.print("description:");
              Serial.println(description);

              JsonObject& main_data = message["main"];
              float temp = main_data["temp"]; 
              weather_temputer_min = main_data["temp_min"]; 
              weather_temputer_max = main_data["temp_max"]; 
              int pressure = main_data["pressure"]; 
              int humidity = main_data["humidity"]; 

              weather_temputer = temp - 273.15;
              // weather_temputer_min = temp_min - 273.15;
              // weather_temputer_max = temp_max - 273.15;
              weather_pressure = pressure;
              weather_humidity = humidity;

              // Serial.print("temp:");
              // Serial.println((temp - 273.15));  //开氏温度与摄氏度之间换算
              // Serial.print("pressure:");
              // Serial.println(pressure);
              // Serial.print("humidity:");
              // Serial.println(humidity);
              Serial.print("temp_min:");
              Serial.println(weather_temputer_min);
              Serial.print("temp_max:");
              Serial.println(weather_temputer_max);
              #endif
          }
          else 
          {
            Serial.println("Error on HTTP request");
          }
      
          if(weather_use_http_id == 0)
          http.end(); //Free the resources
          else if(weather_use_http_id == 1)
          http1.end(); //Free the resources
        }
        vTaskDelay(5000);
      }
      jsonBuffer.clear();  //jsonBuffer 清空
      Getweather_tick = 0;
      weather_begin_flag = false;
      vTaskDelay(100);
      vTaskDelete(NULL);
    }
}
