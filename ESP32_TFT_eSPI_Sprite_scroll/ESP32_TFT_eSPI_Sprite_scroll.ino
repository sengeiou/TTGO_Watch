//代码参考网址:https://github.com/ShotokuTech/T-Display_Scrolling_Text/blob/main/Sprite_scroll_v8.zip

#include <TFT_eSPI.h>
#include "st2240x135.h" //added background image


TFT_eSPI tft = TFT_eSPI();
TFT_eSprite stext1 = TFT_eSprite(&tft); // Sprite object stext1
TFT_eSprite stext2 = TFT_eSprite(&tft); // Sprite object stext2
TFT_eSprite stext3 = TFT_eSprite(&tft); // Sprite object stext2
int tcount = 0;

//==========================================================================================
void setup() {

    //@-初始化串口
  Serial.begin(115200);

  //@-文件系统启动
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  tft.init();
  tft.fillScreen(TFT_BLACK);
  tft.setRotation(0);
  tft.setSwapBytes(true);
  tft.pushImage(0, 48,  240, 135, st2240x135); //added background image

  stext1.setColorDepth(8);
  stext1.createSprite(400, 16); // Sprite wider than the display plus the text to allow text to scroll from the right.
  stext1.fillSprite(TFT_DARKGREY);
  stext1.setScrollRect(0, 0, 400, 16, TFT_DARKGREY); // Sprite wider than the display plus the text to allow text to scroll from the right.
  stext1.setTextColor(TFT_GREEN); // White text, no background
  stext1.loadFont("STKAITI18");

  // Create a sprite for Hello World
  stext2.setColorDepth(8);
  stext2.createSprite(400, 16); // Sprite wider than the display plus the text to allow text to scroll from the right.
  stext2.fillSprite(TFT_DARKGREY);
  stext2.setScrollRect(0, 0, 400, 16, TFT_DARKGREY); // Sprite wider than the display plus the text to allow text to scroll from the right.
  stext2.setTextColor(TFT_WHITE); // White text, no background
  stext2.loadFont("STKAITI18");

  stext3.setColorDepth(8);
  stext3.createSprite(400, 16); // Sprite wider than the display plus the text to allow text to scroll from the right.
  stext3.fillSprite(TFT_DARKGREY);
  stext3.setScrollRect(0, 0, 400, 16, TFT_DARKGREY); // Sprite wider than the display plus the text to allow text to scroll from the right.
  stext3.setTextColor(TFT_RED); // White text, no background
  stext3.loadFont("STKAITI18");
}

//==========================================================================================
void loop() {

  stext1.pushSprite(0, 0);
  stext2.pushSprite(0, 16);
  stext3.pushSprite(0, 32);

  delay(30); // sped it up a little

  stext1.scroll(-1);     // scroll stext 1 pixel left, up/down default is 0
  stext2.scroll(-1);     // scroll stext 1 pixel left, up/down default is 0
  stext3.scroll(-1);     // scroll stext 1 pixel left, up/down default is 0

  tcount--;
  if (tcount <=0)
  { // If we have scrolled 40 pixels the redraw text
    tcount = 400;
    // stext2.drawString("ShotokuTech.com", 240, 0, 2); // draw at 240,0 in sprite, font 2
    stext1.drawString("1.人民群众的呼声很高，马上就要过年了", 0, 0, 2); // draw at 240,0 in sprite, font 2
    stext2.drawString("2.1234 abc 中的黄金干将发掘和国航凤凰哈哈个 @~!", 0, 0, 2); // draw at 240,0 in sprite, font 2
    stext3.drawString("3.dingxiao 丁霄是个好人! 今天真高兴 快乐的日子", 0, 0, 2); // draw at 240,0 in sprite, font 2
  }

} // Loop back and do it all again
//==========================================================================================
