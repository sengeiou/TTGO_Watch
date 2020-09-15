
#define LILYGO_WATCH_2020_V1             //To use T-Watch2020, please uncomment this line

#include <LilyGoWatch.h>


#define ESP32
#include <Arduino.h>
#ifdef ESP32
    #include <WiFi.h>
#else
    #include <ESP8266WiFi.h>
#endif

#include <HTTPClient.h>         //Remove Audio Lib error
#include "AudioFileSourcePROGMEM.h" 
#include "AudioGeneratorWAV.h"
#include "AudioOutputI2SNoDAC.h"
#include "AudioOutputI2S.h"

// VIOLA sample taken from https://ccrma.stanford.edu/~jos/pasp/Sound_Examples.html
#include "viola.h"
// #include "geji_192_wav.h"

TTGOClass *ttgo;

AudioGeneratorWAV *wav;
AudioFileSourcePROGMEM *file;
// AudioOutputI2SNoDAC *out;
AudioOutputI2S *out;

void setup()
{
  WiFi.mode(WIFI_OFF); 
  Serial.begin(115200);
  ttgo = TTGOClass::getWatch();
  ttgo->begin();
  ttgo->openBL();

  ttgo->tft->fillScreen(TFT_BLACK);
  ttgo->tft->setTextFont(2);
  ttgo->tft->setCursor(0, 0);
  ttgo->tft->fillScreen(TFT_BLACK);
  ttgo->tft->println("Sample MP3 playback begins");

  //!Turn on the audio power
  ttgo->enableLDO3();

  delay(1000);

  out = new AudioOutputI2S();
  // out = new AudioOutputI2SNoDAC();
  //External DAC decoding
  out->SetPinout(TWATCH_DAC_IIS_BCK, TWATCH_DAC_IIS_WS, TWATCH_DAC_IIS_DOUT);

  Serial.printf("WAV start\n");

  file = new AudioFileSourcePROGMEM( viola, sizeof(viola) );
  // out = new AudioOutputI2SNoDAC();
  wav = new AudioGeneratorWAV();
  wav->begin(file, out);
}

void loop()
{
  if (wav->isRunning()) {
    if (!wav->loop()) wav->stop();
  } else {
    Serial.printf("WAV done\n");
    delay(1000);
  }
}
