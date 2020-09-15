#pragma mark - Depend ESP8266Audio and ESP8266_Spiram libraries
/*
cd ~/Arduino/libraries
git clone https://github.com/earlephilhower/ESP8266Audio
git clone https://github.com/Gianbacchio/ESP8266_Spiram
*/


#define USE_MP3

#include "config.h"
#include <WiFi.h>
#include <HTTPClient.h>         //Remove Audio Lib error
#include "AudioFileSourcePROGMEM.h"
#include "AudioFileSourceID3.h"
#ifdef USE_MP3
#include "AudioGeneratorMP3.h"
#else
#include "AudioGeneratorWAV.h"
#endif
#include "AudioOutputI2S.h"
// #include "image.h"
// #include "test_music.h"
// #include "new1.h"
// #include "door1.h"




#ifdef USE_MP3
// #include "door1.h"
#include "geji_44k_mp3.h"
#else
#include "new_wav.h"
#endif

TTGOClass *ttgo;
#ifdef USE_MP3
AudioGeneratorMP3 *mp3;
#else
AudioGeneratorWAV *wav;
#endif
AudioFileSourcePROGMEM *file;
AudioOutputI2S *out;
AudioFileSourceID3 *id3;

void setup()
{
    WiFi.mode(WIFI_OFF);
    delay(500);
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

    #ifdef USE_MP3
    file = new AudioFileSourcePROGMEM(geji_44k_mp3, sizeof(geji_44k_mp3));
    id3 = new AudioFileSourceID3(file);
    #else
    file = new AudioFileSourcePROGMEM(new_wav, sizeof(new_wav));
    #endif


#if defined(STANDARD_BACKPLANE)
    out = new AudioOutputI2S(0, 1);
#elif defined(EXTERNAL_DAC_BACKPLANE)
    out = new AudioOutputI2S();
    //External DAC decoding
    out->SetPinout(TWATCH_DAC_IIS_BCK, TWATCH_DAC_IIS_WS, TWATCH_DAC_IIS_DOUT);
#endif

    #ifdef USE_MP3
    mp3 = new AudioGeneratorMP3();
    mp3->begin(id3, out);
    #else
    wav = new AudioGeneratorWAV();
    wav->begin(file, out);
    #endif
}

void loop()
{
    #ifdef USE_MP3
    if (mp3->isRunning()) 
    {
        if (!mp3->loop()) 
        mp3->stop();
    } 
    #else
    if (wav->isRunning()) 
    {
        if (!wav->loop()) 
        wav->stop();
    } 
    #endif
    else 
    {
        Serial.printf("MP3 done\n");
        delay(5000);

        file = new AudioFileSourcePROGMEM(geji_44k_mp3, sizeof(geji_44k_mp3));

        #ifdef USE_MP3
        id3 = new AudioFileSourceID3(file);
        mp3 = new AudioGeneratorMP3();
        mp3->begin(id3, out);
        #else
        wav = new AudioGeneratorWAV();
        wav->begin(file, out);
        #endif
    }
}
