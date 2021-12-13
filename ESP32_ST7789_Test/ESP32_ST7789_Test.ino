
#include <Arduino.h>
// #include <U8g2lib.h>
#include <SPI.h>
#include <Wire.h>

//SH1107  SSD1306 128*80  -OLED裸屏
// I2C接口  SDA-GPIO21   SCL-GPIO22

#include <TFT_eSPI.h> 
// #include <SPI.h>

#define DX_TFT_BL_Pin 15
#define DX_LED2_Pin   27


// U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ 22, /* data=*/ 21, /* reset=*/ U8X8_PIN_NONE);   // Adafruit Feather M0 Basic Proto + FeatherWing OLED

// U8G2_SH1107_128X80_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ 22, /* data=*/21, /* reset=*/ U8X8_PIN_NONE);
//U8G2_SH1107_64X128_2_HW_I2C u8g2(U8G2_R1, U8X8_PIN_NONE);
// U8G2_SSD1306_128X80_NONAME_F_HW_I2C u8g2(U8G2_R3, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ 22, /* data=*/21);
//U8G2_SH1107_64X128_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ 22, /* data=*/21 );
//U8G2_SH1107_64X128_1_SW_I2C u8g2(U8G2_R0, /* clock=*/ 22, /* data=*/21, /* reset=*/ U8X8_PIN_NONE);
// U8G2_SH1107_128X128_1_4W_HW_SPI u8g2(U8G2_R3, /* cs=*/ 22, /* dc=*/ 21,/* reset=*/ U8X8_PIN_NONE);
// U8G2_SSD1306_128X80_NONAME_F_HW_I2C u8g2(U8G2_R3, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ 22, /* data=*/21);


TFT_eSPI tft = TFT_eSPI(); 

void setup() {

  Serial.begin(115200);

  pinMode(DX_TFT_BL_Pin, OUTPUT|PULLUP);
  digitalWrite(DX_TFT_BL_Pin, HIGH);

  pinMode(DX_LED2_Pin, OUTPUT|PULLUP);
  digitalWrite(DX_LED2_Pin, LOW);

//   u8g2.begin();
//   u8g2.enableUTF8Print();    // enable UTF8 support for the Arduino print() function
// //  u8g2.setFont(u8g2_font_10x20_tn ); 
//   u8g2.setFontDirection(0);
//   //@-设置字体
//   u8g2.setFont( u8g2_font_t0_16b_tr ); //10px
//   // u8g2.setFont( u8g2_font_6x13_t_hebrew ); //9PX

  // TFT_eSPI驱动---------------
  tft.init();
  tft.begin();
  tft.setRotation(0);
  tft.fillScreen(TFT_RED);

  Serial.print("go...");
  Serial.println("dx...");

}



void loop() {
  // put your main code here, to run repeatedly:
  char temp[20];

  // u8g2.clearBuffer();
  // u8g2.setCursor(10, 10);
  // // sprintf(temp, "%.0f", 78.23); 
  // // u8g2.print(String(temp));
  // u8g2.print("dingxiao");
  // u8g2.sendBuffer();

  digitalWrite(DX_LED2_Pin, LOW);
  delay(100);
  digitalWrite(DX_LED2_Pin, HIGH);
  delay(100);

  Serial.print("Run...");

}
