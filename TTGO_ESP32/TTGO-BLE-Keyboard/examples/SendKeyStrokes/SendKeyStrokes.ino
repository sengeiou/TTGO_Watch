/**
 * This example turns the ESP32 into a Bluetooth LE keyboard that writes the words, presses Enter, presses a media key and then Ctrl+Alt+Delete
 */
#include <TFT_eSPI.h>
#include <BleKeyboard.h>

BleKeyboard bleKeyboard;
TFT_eSPI tft = TFT_eSPI();

//@-KEY状态
uint8_t KEY_Left_Status = 0;
uint8_t KEY_Right_Status = 0;
int key_send_trigger1 = 0;
int key_send_trigger2 = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE work!");

  //@-TFT屏幕初始化
  tft.init();
  tft.setRotation(0);
  tft.setTextColor(TFT_WHITE,TFT_BLACK); 
  tft.fillScreen(TFT_GREEN);
  tft.setSwapBytes(true);
  tft.setTextFont(2);



  //@-配置按键
  pinMode(0,INPUT_PULLUP);
  pinMode(35,INPUT_PULLUP);

  bleKeyboard.begin();
}


void loop() {

  //@-检测按键状态
  KEY_Left_Status = digitalRead(0);
  KEY_Right_Status = digitalRead(35);


  if(bleKeyboard.isConnected()) {

    if((KEY_Left_Status == 0) && (key_send_trigger1 == 0))
    {
      key_send_trigger1 = 1;
      // Serial.println("Sending 'Hello world'...");
      // bleKeyboard.print("10238831\n");

      // 锁屏
      bleKeyboard.press(KEY_LEFT_GUI);
      bleKeyboard.print("l");
      bleKeyboard.releaseAll();

    }
    if(KEY_Left_Status == 1)
    key_send_trigger1 = 0;

    if((KEY_Right_Status == 0) && (key_send_trigger2 == 0))
    {
      key_send_trigger2 = 1;
      // 输入密码
      bleKeyboard.print("10238831\n");
    }
    if(KEY_Right_Status == 1)
    key_send_trigger2 = 0;

    // delay(1000);

    // Serial.println("Sending Enter key...");
    // bleKeyboard.write(KEY_RETURN);

    // delay(1000);

    // Serial.println("Sending Play/Pause media key...");
    // bleKeyboard.write(KEY_MEDIA_PLAY_PAUSE);

    // delay(1000);

    // Serial.println("Sending Ctrl+Alt+Delete...");
    // bleKeyboard.press(KEY_LEFT_CTRL);
    // bleKeyboard.press(KEY_LEFT_ALT);
    // bleKeyboard.press(KEY_DELETE);
    // delay(100);
    // bleKeyboard.releaseAll();
  }

  // Serial.println("Waiting 5 seconds...");
  // delay(5000);
}
