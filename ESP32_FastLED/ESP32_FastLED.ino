#include "FastLED.h"
// #include <Adafruit_NeoPixel.h>

#define PIN            18
// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      60 //12   //60

#define NUM_LEDS 60
#define DATA_PIN 18
#define serialRate 500000
static const uint8_t prefix[] = {'A', 'd', 'a'};
 
// Define the array of leds
CRGB leds[NUM_LEDS];

// Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

uint32_t color;
 
void setup() { 
     FastLED.addLeds<WS2811, DATA_PIN, RGB>(leds, NUM_LEDS);
      Serial.begin(serialRate);
      Serial.print("Ada\n");
      // pixels.begin(); // This initializes the NeoPixel library. 

//               pixels.setPixelColor(0, 0x345612); 
//          pixels.show();
}
 
void loop() { 
      for(int i = 0; i < sizeof(prefix); ++i){
            while (!Serial.available());
            if(prefix[i] != Serial.read()) 
            return;
      }
      while(Serial.available() < 3);
      int highByte = Serial.read();
      int lowByte  = Serial.read();
      int checksum = Serial.read();
      if (checksum != (highByte ^ lowByte ^ 0x55)){
            return;}
 
      uint16_t ledCount = ((highByte & 0x00FF) << 8 | (lowByte & 0x00FF) ) + 1;
      if (ledCount > NUM_LEDS){
            ledCount = NUM_LEDS;}
            
      for (int i = 0; i < ledCount; i++){
            while(Serial.available() < 3);
            leds[i].r = Serial.read();
            leds[i].g = Serial.read();
            leds[i].b = Serial.read();}

      // for(int k=0; k<ledCount; k++)
      // {
      //   color = pixels.Color(leds[k].r, leds[k].g, leds[k].b);
      //   pixels.setPixelColor(k, color);   
      // //   pixels.show();
      // }
      // pixels.show();  
            
           FastLED.show();
}
