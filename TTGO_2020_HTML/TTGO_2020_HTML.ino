

//https://home.openweathermap.org/api_keys   8874f49040c9bad35a788cbdb34f5444

// https://api.openweathermap.org/data/2.5/weather?q=Shanghai&appid=8874f49040c9bad35a788cbdb34f5444

// #include <WiFi.h>
// #include <HTTPClient.h>
 
// const char* ssid = "DX_JS";
// const char* password =  "dingxiao";
 
// void setup() {
 
//   Serial.begin(115200);
//   delay(1000);
 
//   WiFi.begin(ssid, password); 
 
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(1000);
//     Serial.println("Connecting to WiFi..");
//   }
 
//   Serial.println("Connected to the WiFi network");
// }
 
// const char* root_ca= \
// "-----BEGIN CERTIFICATE-----\n" \
// "MIICiTCCAg+gAwIBAgIQH0evqmIAcFBUTAGem2OZKjAKBggqhkjOPQQDAzCBhTEL\n" \
// "MAkGA1UEBhMCR0IxGzAZBgNVBAgTEkdyZWF0ZXIgTWFuY2hlc3RlcjEQMA4GA1UE\n" \
// "BxMHU2FsZm9yZDEaMBgGA1UEChMRQ09NT0RPIENBIExpbWl0ZWQxKzApBgNVBAMT\n" \
// "IkNPTU9ETyBFQ0MgQ2VydGlmaWNhdGlvbiBBdXRob3JpdHkwHhcNMDgwMzA2MDAw\n" \
// "MDAwWhcNMzgwMTE4MjM1OTU5WjCBhTELMAkGA1UEBhMCR0IxGzAZBgNVBAgTEkdy\n" \
// "ZWF0ZXIgTWFuY2hlc3RlcjEQMA4GA1UEBxMHU2FsZm9yZDEaMBgGA1UEChMRQ09N\n" \
// "T0RPIENBIExpbWl0ZWQxKzApBgNVBAMTIkNPTU9ETyBFQ0MgQ2VydGlmaWNhdGlv\n" \
// "biBBdXRob3JpdHkwdjAQBgcqhkjOPQIBBgUrgQQAIgNiAAQDR3svdcmCFYX7deSR\n" \
// "FtSrYpn1PlILBs5BAH+X4QokPB0BBO490o0JlwzgdeT6+3eKKvUDYEs2ixYjFq0J\n" \
// "cfRK9ChQtP6IHG4/bC8vCVlbpVsLM5niwz2J+Wos77LTBumjQjBAMB0GA1UdDgQW\n" \
// "BBR1cacZSBm8nZ3qQUfflMRId5nTeTAOBgNVHQ8BAf8EBAMCAQYwDwYDVR0TAQH/\n" \
// "BAUwAwEB/zAKBggqhkjOPQQDAwNoADBlAjEA7wNbeqy3eApyt4jf/7VGFAkK+qDm\n" \
// "fQjGGoe9GKhzvSbKYAydzpmfz1wPMOG+FDHqAjAU9JM8SaczepBGR7NjfRObTrdv\n" \
// "GDeAU/7dIOA1mjbRxwG55tzd8/8dLDoWV9mSOdY=\n" \
// "-----END CERTIFICATE-----\n";
 
// void loop() {
 
//   if ((WiFi.status() == WL_CONNECTED)) { //Check the current connection status
 
//     HTTPClient http;
 
//     http.begin("https://jsonplaceholder.typicode.com/posts?userId=1", root_ca); //Specify the URL and certificate
//     int httpCode = http.GET();                                                  //Make the request
 
//     if (httpCode > 0) { //Check for the returning code
 
//         String payload = http.getString();
//         Serial.println(httpCode);
//         Serial.println(payload);
//       }
 
//     else {
//       Serial.println("Error on HTTP request");
//     }
 
//     http.end(); //Free the resources
//   }
 
//   delay(10000);
// }


// #define ESP32

// #include <Arduino.h>
// // load libraries
// #include <ArduinoJson.h>   //V6
// #ifdef ESP32
//   #include <WiFi.h> 
//   #include <HTTPClient.h> 
// #else
//   #include <ESP8266WiFi.h> 
//   #include <ESP8266HTTPClient.h> 
// #endif

// #define docSize          800
// #define displayFreeHeap  false
// String api_key = "78f59b6fdfd442afa6901249dcbc9530";
// String city = "101210101";
// String state = "Ile-de-France";
// String country = "france";

// String Json_test = "{\"dingxiao\": \"1234\", \"value\": \"8888\"}";

// int heap_at_startup = 0;

// // Replace with your network credentials
// const char* SSID = "DX_JS"; 
// const char* PASSWORD = "dingxiao";

// const char* root_ca= \
// "-----BEGIN CERTIFICATE-----\n" \
// "MIIF2jCCBMKgAwIBAgIQRbbWU9V+laPxVUno/mg/JDANBgkqhkiG9w0BAQsFADCB\n" \
// "jzELMAkGA1UEBhMCR0IxGzAZBgNVBAgTEkdyZWF0ZXIgTWFuY2hlc3RlcjEQMA4G\n" \
// "A1UEBxMHU2FsZm9yZDEYMBYGA1UEChMPU2VjdGlnbyBMaW1pdGVkMTcwNQYDVQQD\n" \
// "Ey5TZWN0aWdvIFJTQSBEb21haW4gVmFsaWRhdGlvbiBTZWN1cmUgU2VydmVyIENB\n" \
// "MB4XDTIwMDUzMDAwMDAwMFoXDTIwMTAzMTIzNTk1OVowGDEWMBQGA1UEAxMNaGV3\n" \
// "ZWF0aGVyLmNvbTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBANsOiIwz\n" \
// "CLjZQPapbQxGUvtMQRUNT1/hc6FvAr0Eu1NArntJQ+7X90tCoAbH6sC4yBEOwYgB\n" \
// "QMZepbyuFAA/8hbRhW1R2LA7T4zTvMTMF25fIsJ25kOcbXuwnmnby89f8UsxPrWW\n" \
// "uCJYEJ9ptk6ZJV89WPgHWGtCmCcqR/O1jtSYhULdzR90F0OIeQKTBdeCb2nYJaAd\n" \
// "Q8BOoOrOfkI/YPa3TICwmP8Hozr9+5zZwTIrGFUS62XsvX5FSIh7Qu/t6tfFuwau\n" \
// "BbAftaSw11Lq6xhaZFcJb9l60Z+HXZ2I7XRmuYA+nF/9A6XfXrITjJ2RSWzMOOTi\n" \
// "n5q4AXDNhL+ledsCAwEAAaOCAqYwggKiMB8GA1UdIwQYMBaAFI2MXsRUrYrhd+mb\n" \
// "+ZsF4bgBjWHhMB0GA1UdDgQWBBSKr7b+8aEadsVoGuecdwfeAW/hzjAOBgNVHQ8B\n" \
// "Af8EBAMCBaAwDAYDVR0TAQH/BAIwADAdBgNVHSUEFjAUBggrBgEFBQcDAQYIKwYB\n" \
// "BQUHAwIwSQYDVR0gBEIwQDA0BgsrBgEEAbIxAQICBzAlMCMGCCsGAQUFBwIBFhdo\n" \
// "dHRwczovL3NlY3RpZ28uY29tL0NQUzAIBgZngQwBAgEwgYQGCCsGAQUFBwEBBHgw\n" \
// "djBPBggrBgEFBQcwAoZDaHR0cDovL2NydC5zZWN0aWdvLmNvbS9TZWN0aWdvUlNB\n" \
// "RG9tYWluVmFsaWRhdGlvblNlY3VyZVNlcnZlckNBLmNydDAjBggrBgEFBQcwAYYX\n" \
// "aHR0cDovL29jc3Auc2VjdGlnby5jb20wggEDBgorBgEEAdZ5AgQCBIH0BIHxAO8A\n" \
// "dQAHt1wb5X1o//Gwxh0jFce65ld8V5S3au68YToaadOiHAAAAXJm2YzpAAAEAwBG\n" \
// "MEQCIDl8x6QUba+qzGp7YEO1e76BQs7Ou6XbCoC27OdPIoh4AiAApCwOCF55fPhV\n" \
// "pwcNPt+R/b1STUIGZNWxEQpE1sibYAB2AF6nc/nfVsDntTZIfdBJ4DJ6kZoMhKES\n" \
// "EoQYdZaBcUVYAAABcmbZjTQAAAQDAEcwRQIgSMkuQrvsuH7ftaH5sa2otXn0NjHt\n" \
// "nCeuDpWa6imjY7oCIQDbejo07YI1XCy7NnKvcQZNepgx0EPUnQ/NMpazbfux+jBK\n" \
// "BgNVHREEQzBBgg1oZXdlYXRoZXIuY29tgg4qLmhld2VhdGhlci5jboIPKi5oZXdl\n" \
// "YXRoZXIuY29tgg8qLmhld2VhdGhlci5uZXQwDQYJKoZIhvcNAQELBQADggEBAKF4\n" \
// "1OVPWHC615rRK4oxeZVsX83HoOnJK3m28FNqO54gkL8ZBbnXq6myNEMYfZSjWMZx\n" \
// "kngPAZ+/fT3ErgtQZ5KJCmYWhJBlqaE+cFCG05/Apgo+4GAdVIAlfSGTQ9YLyvOp\n" \
// "q4JIUmcTwhu0yYalwC7zKVL8xps9K2WD73pfimMCo/jVB2JqpynJ89qsANqiae+n\n" \
// "P2zz5oXPHP1ODAIhVMJ1Q28IUyupKaHapoEtVT/u/5gw+ywt+AxyMn+NW5B0OLnz\n" \
// "DMBg5/exNsZVwDy0tq5FrwdmxGwCrvnNn3Mx8cYnqpOteaYNIKRgjfPooI6ElHGb\n" \
// "8RaXKNryyWao+PiGzO8=\n" \
// "-----END CERTIFICATE-----\n";


// String jsonBuffer;
// StaticJsonDocument<500> aiqAir;

// void setup() {
 
//   Serial.begin(115200);
//   Serial.println("\n");
  
//   Serial.print("Connecting to ");
//   Serial.println(SSID);
//   WiFi.begin(SSID, PASSWORD);
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.print(".");
//   }
//   // Print local IP address 
//   Serial.println("");
//   Serial.println("WiFi connected at IP address:");
//   Serial.println(WiFi.localIP());



//   // Start Web Server
//   if(WiFi.status()== WL_CONNECTED){
//     String serverPath = "https://devapi.heweather.net/v7/weather/now?location=" + city + "&key=" + api_key;

//     // https://devapi.heweather.net/v7/weather/now?location=101210101&key=78f59b6fdfd442afa6901249dcbc9530

//     HTTPClient http;
//     http.begin(serverPath.c_str(), root_ca);
//     int httpResponseCode = http.GET();
//     if ( httpResponseCode == 200 ) {
//       Serial.print("HTTP Response code: ");
//       Serial.println(httpResponseCode);
//       String payload = http.getString();
//       Serial.println(payload);
//       deserializeJson(aiqAir, payload);
//       // deserializeJson(aiqAir, Json_test);
//     }
//   }    
  
//   if ( aiqAir.isNull() ) {
//     Serial.println("! No Data returned by server");
//   } else {
//     Serial.print("Memory used (bytes): ");
//     Serial.println(aiqAir.memoryUsage());

//     // All data 
//     JsonObject data = aiqAir["now"];
//     // JsonObject data = aiqAir["dingxiao"];
//     // Only pollution 
//     // JsonObject data_current_pollution = data["current"]["pollution"];
//     // const char* data_current_pollution_ts = data_current_pollution["ts"]; // "2020-09-04T18:00:00.000Z"
//     // int data_current_pollution_aqius = data_current_pollution["aqius"]; // 6
//     // const char* data_current_pollution_mainus = data_current_pollution["mainus"]; // "n2"
//     // int data_current_pollution_aqicn = data_current_pollution["aqicn"]; // 17
//     // const char* data_current_pollution_maincn = data_current_pollution["maincn"]; // "n2"

//     // const char* data_temp = data["temp"];
//     const char* data_temp = data["temp"];

//     // Serial.print("Pollution at "); Serial.print(city); Serial.println(": ");
//     // Serial.print("IAQ "); Serial.print(data_current_pollution_aqius);
//     // Serial.print(" Polluting "); Serial.println(data_current_pollution_maincn);

//     Serial.print(" Temp "); Serial.println(data_temp);
//   }

//   #ifdef ESP32
//     // Display free heap on startup 
//     Serial.print("ESP32 free Heap at startup: ");
//     heap_at_startup = ESP.getFreeHeap();
//     Serial.println(heap_at_startup);
//   #endif   

//   #ifdef ESP32
//     if ( displayFreeHeap  ) {
//       Serial.print("Heap memory used");
//       Serial.println(ESP.getFreeHeap() - heap_at_startup);
//     }  
//   #endif 
// }
 
// void loop() {
// }





#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>   //V5.13.5
 
// const char* ssid = "wuyiyi";
// const char* password =  "10238831";

const char* ssid = "DX_JS";
const char* password =  "dingxiao";

const char* root_ca= \
"-----BEGIN CERTIFICATE-----\n" \
"MIIGvjCCBaagAwIBAgIRAKL7IEo7D+v9u0G4ItYCJYwwDQYJKoZIhvcNAQELBQAw\n" \
"gY8xCzAJBgNVBAYTAkdCMRswGQYDVQQIExJHcmVhdGVyIE1hbmNoZXN0ZXIxEDAO\n" \
"BgNVBAcTB1NhbGZvcmQxGDAWBgNVBAoTD1NlY3RpZ28gTGltaXRlZDE3MDUGA1UE\n" \
"AxMuU2VjdGlnbyBSU0EgRG9tYWluIFZhbGlkYXRpb24gU2VjdXJlIFNlcnZlciBD\n" \
"QTAeFw0yMDAzMTcwMDAwMDBaFw0yMjA2MTkwMDAwMDBaMB8xHTAbBgNVBAMMFCou\n" \
"b3BlbndlYXRoZXJtYXAub3JnMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKC\n" \
"AQEA2DMTq6QbiQ6N/PK6u6dv8J1w5/w/GLm1d7J3daL80/15qRlsxUEpM78/OWmE\n" \
"s60kKSfyOVyxOHrVoXMfEhIxATdYQtRtN2JQEFYDkRauvVgr5eXQO2EJZXBZUb2C\n" \
"0dLFMD2WtrQGl7059kCOBlA/vX2+uTIQwFx/qZyVKkhzgdthtoDQ5jDzx7scDM0U\n" \
"9c/be/aWNPzoJV1HK37luC0nHUyT0zDpXMt82DgoCRix9z9RzDNkyjsPW2qP/pOE\n" \
"RpXk0z49jOFqtUxTtR9HfbKoeQ/RobxD2fG5P1cfunZ2lU3lyl5PeKbmMlSdSlci\n" \
"4OuileGdauTqgU254X7bB/9iTQIDAQABo4IDgjCCA34wHwYDVR0jBBgwFoAUjYxe\n" \
"xFStiuF36Zv5mwXhuAGNYeEwHQYDVR0OBBYEFP2HTXuP9/WVxbHQk4RHPpXCLktU\n" \
"MA4GA1UdDwEB/wQEAwIFoDAMBgNVHRMBAf8EAjAAMB0GA1UdJQQWMBQGCCsGAQUF\n" \
"BwMBBggrBgEFBQcDAjBJBgNVHSAEQjBAMDQGCysGAQQBsjEBAgIHMCUwIwYIKwYB\n" \
"BQUHAgEWF2h0dHBzOi8vc2VjdGlnby5jb20vQ1BTMAgGBmeBDAECATCBhAYIKwYB\n" \
"BQUHAQEEeDB2ME8GCCsGAQUFBzAChkNodHRwOi8vY3J0LnNlY3RpZ28uY29tL1Nl\n" \
"Y3RpZ29SU0FEb21haW5WYWxpZGF0aW9uU2VjdXJlU2VydmVyQ0EuY3J0MCMGCCsG\n" \
"AQUFBzABhhdodHRwOi8vb2NzcC5zZWN0aWdvLmNvbTAzBgNVHREELDAqghQqLm9w\n" \
"ZW53ZWF0aGVybWFwLm9yZ4ISb3BlbndlYXRoZXJtYXAub3JnMIIB9gYKKwYBBAHW\n" \
"eQIEAgSCAeYEggHiAeAAdwBGpVXrdfqRIDC1oolp9PN9ESxBdL79SbiFq/L8cP5t\n" \
"RwAAAXDobGj8AAAEAwBIMEYCIQDuoxRU3qxvOhsXh/vQPwAzBQfmu0b76RYKY27r\n" \
"3IjeuwIhAKhiaG0C9WMqsBNviTNJHl8iUZppSoDbreFWKU3ju715AHYA36Veq2iC\n" \
"Tx9sre64X04+WurNohKkal6OOxLAIERcKnMAAAFw6GxoyAAABAMARzBFAiEAiPLZ\n" \
"oR9BVGbeBKcZWWCWe5khT1jrbwqFFs1qqciHhmUCICNPG3dRIueExiu3HF6tUiNb\n" \
"rlGF/mf9Efr3JkAkqGsZAHUAQcjKsd8iRkoQxqE6CUKHXk4xixsD6+tLx2jwkGKW\n" \
"BvYAAAFw6Gxo7AAABAMARjBEAiAzzodBqseRU0wn7ukh37SvTOjmv8vpayKuZ4AE\n" \
"ut06BAIgArnrQObBVZU87a6ubmSWGHPiEi8cyPYdqZkMVycT3TgAdgBvU3asMfAx\n" \
"GdiZAKRRFf93FRwR2QLBACkGjbIImjfZEwAAAXDobGnaAAAEAwBHMEUCIGo9M7aa\n" \
"TjzbYPbR16+gwPnAGNiZI0ujRTDXRUJsW+D8AiEAgexT/9i23R7/XZfh5sL1Q9E/\n" \
"pE40zy1wXC1O3BHvz2MwDQYJKoZIhvcNAQELBQADggEBANJ4pa0tYp5QOtGy1RxM\n" \
"hcX2WydaU89WwySUB41pxbXBvaRLQyFBzC/COjPyN6zR52irYeBr0uFLLmwkaZfg\n" \
"eavkaExosslVP9g1js4j7wAKR5CdlEJfgw4eTxu8LAx5WUhm66HaMQol2neSyky2\n" \
"XPZt4KvZC9Fk/0x28JpXbMpckpH1/VpWPz3ulQw1/9TgV0+saRpFaKVXoZT5IObo\n" \
"j6cAp85OGBmRNJFypFFZRvy85aPJCP8IIyNoC9MoZIQ2VEuXQMTrIDU14Y46BTDq\n" \
"HaolM6WQZl42iGBzqJcOF2PGzcZ5YUahZW1GMxwB3NCyugR93FMCwtM4Wip6Ja5Q\n" \
"5fs=\n" \
"-----END CERTIFICATE-----\n";
 
void setup() {
 
  Serial.begin(115200);
  delay(4000);
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
 
  Serial.println("Connected to the WiFi network");
 
}
 
void loop() {
 
  if ((WiFi.status() == WL_CONNECTED)) { //Check the current connection status
 
    HTTPClient http;

    // http.begin("http://www.dx1023.com/video"); //Specify the URL

    //@-杭州天气
    // https://devapi.heweather.net/v7/weather/now?key=78f59b6fdfd442afa6901249dcbc9530&location=101210101
    //@-杭州ID
    // https://geoapi.heweather.net/v2/city/lookup?key=78f59b6fdfd442afa6901249dcbc9530&location=%E6%9D%AD%E5%B7%9E&adm=%E6%B5%99%E6%B1%9F
    // http.begin("https://devapi.heweather.net/v7/weather/now?key=78f59b6fdfd442afa6901249dcbc9530&location=101210101"); //Specify the URL
    
    http.begin("https://api.openweathermap.org/data/2.5/weather?q=hangzhou&appid=8874f49040c9bad35a788cbdb34f5444",root_ca);

    int httpCode = http.GET();                                        //Make the request
 
    if (httpCode > 0) { //Check for the returning code
 
        String payload = http.getString();
        Serial.println(httpCode);
        Serial.println(payload);

        StaticJsonBuffer<2000> jsonBuffer;
        // JsonObject& message = jsonBuffer.parseObject((char *)payload);
        JsonObject& message = jsonBuffer.parseObject(payload);
        if (!message.success()) {
          Serial.println("JSON parse failed");  
        }

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

      JsonObject& coord_data = message["coord"];
      int lon = coord_data["lon"];
      Serial.print("lon:");
      Serial.println(lon);

      String base = message["base"];
      Serial.print("base:");
      Serial.println(base);

      // JsonObject& weather_data = message["weather"];
      int id = message["weather"][0]["id"]; 
      const char* main = message["weather"][0]["main"]; 
      String description = message["weather"][0]["description"];
      // Output to serial monitor
      Serial.print("id:");
      Serial.println(id);
      Serial.print("main:");
      Serial.println(main);
      Serial.print("description:");
      Serial.println(description);

      JsonObject& main_data = message["main"];
      float temp = main_data["temp"]; 
      float temp_min = main_data["temp_min"]; 
      float temp_maz = main_data["temp_max"]; 
      int pressure = main_data["pressure"]; 
      int humidity = main_data["humidity"]; 

      Serial.print("temp:");
      Serial.println((temp - 273.15));  //开氏温度与摄氏度之间换算
      Serial.print("pressure:");
      Serial.println(pressure);
      Serial.print("humidity:");
      Serial.println(humidity);

      }
 
    else {
      Serial.println("Error on HTTP request");
    }
 
    http.end(); //Free the resources
  }
 
  delay(20000);
 
}




// /*
//   Rui Santos
//   Complete project details at Complete project details at https://RandomNerdTutorials.com/esp32-http-get-post-arduino/

//   Permission is hereby granted, free of charge, to any person obtaining a copy
//   of this software and associated documentation files.

//   The above copyright notice and this permission notice shall be included in all
//   copies or substantial portions of the Software.
// */

// #include <WiFi.h>
// #include <HTTPClient.h>
// #include <Arduino_JSON.h>

// const char* ssid = "wuyiyi";
// const char* password = "10238831";

// //Your Domain name with URL path or IP address with path
// const char* serverName = "https://devapi.heweather.net/v7/weather/now?key=78f59b6fdfd442afa6901249dcbc9530&location=101210101";

// // the following variables are unsigned longs because the time, measured in
// // milliseconds, will quickly become a bigger number than can be stored in an int.
// unsigned long lastTime = 0;
// // Timer set to 10 minutes (600000)
// //unsigned long timerDelay = 600000;
// // Set timer to 5 seconds (5000)
// unsigned long timerDelay = 5000;

// String sensorReadings;
// float sensorReadingsArr[3];

// void setup() {
//   Serial.begin(115200);

//   WiFi.begin(ssid, password);
//   Serial.println("Connecting");
//   while(WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.print(".");
//   }
//   Serial.println("");
//   Serial.print("Connected to WiFi network with IP Address: ");
//   Serial.println(WiFi.localIP());
 
//   Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");
// }

// void loop() {
//   //Send an HTTP POST request every 10 minutes
//   if ((millis() - lastTime) > timerDelay) {
//     //Check WiFi connection status
//     if(WiFi.status()== WL_CONNECTED){
              
//       sensorReadings = httpGETRequest(serverName);
//       Serial.println(sensorReadings);
//       JSONVar myObject = JSON.parse(sensorReadings);
  
//       // JSON.typeof(jsonVar) can be used to get the type of the var
//       if (JSON.typeof(myObject) == "undefined") {
//         Serial.println("Parsing input failed!");
//         return;
//       }
    
//       Serial.print("JSON object = ");
//       Serial.println(myObject);
    
//       // myObject.keys() can be used to get an array of all the keys in the object
//       JSONVar keys = myObject.keys();
    
//       for (int i = 0; i < keys.length(); i++) {
//         JSONVar value = myObject[keys[i]];
//         Serial.print(keys[i]);
//         Serial.print(" = ");
//         Serial.println(value);
//         sensorReadingsArr[i] = double(value);
//       }
//       Serial.print("1 = ");
//       Serial.println(sensorReadingsArr[0]);
//       Serial.print("2 = ");
//       Serial.println(sensorReadingsArr[1]);
//       Serial.print("3 = ");
//       Serial.println(sensorReadingsArr[2]);
//     }
//     else {
//       Serial.println("WiFi Disconnected");
//     }
//     lastTime = millis();
//   }
// }

// String httpGETRequest(const char* serverName) {
//   HTTPClient http;
    
//   // Your IP address with path or Domain name with URL path 
//   http.begin(serverName);
  
//   // Send HTTP POST request
//   int httpResponseCode = http.GET();
  
//   String payload = "{}"; 
  
//   if (httpResponseCode>0) {
//     Serial.print("HTTP Response code: ");
//     Serial.println(httpResponseCode);
//     payload = http.getString();
//   }
//   else {
//     Serial.print("Error code: ");
//     Serial.println(httpResponseCode);
//   }
//   // Free resources
//   http.end();

//   return payload;
// }

