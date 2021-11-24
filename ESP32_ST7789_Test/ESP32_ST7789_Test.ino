#include <TFT_eSPI.h> 
#include <SPI.h>

#define DX_TFT_BL_Pin 15


TFT_eSPI tft = TFT_eSPI(); 

void setup() {

  Serial.begin(115200);

  pinMode(DX_TFT_BL_Pin, OUTPUT|PULLUP);
  digitalWrite(DX_TFT_BL_Pin, HIGH);

  tft.init();
  tft.begin();
  tft.setRotation(0);
  tft.fillScreen(TFT_RED);

  Serial.print("go...");

}



void loop() {
  // put your main code here, to run repeatedly:

}
