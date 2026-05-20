#include <FastLED.h>

#define NUM_LEDS 200  
#define DATA_PIN 5

CRGB leds[NUM_LEDS];

void setup() { 
    FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
    //FastLED.addLeds<WS2811, DATA_PIN, GRB>(leds, NUM_LEDS);
    
    // Set brightness very low (only 15 out of 255) to protect the USB port
    FastLED.setBrightness(50); 
    
    // Force every single LED to turn completely OFF first
    FastLED.clear();
    FastLED.show();
    delay(500);
}

void loop() { 
    // Turn ONLY the very first LED solid Green
    leds[0] = CRGB::Blue;
    leds[1] = CRGB::Blue;
    leds[2] = CRGB::Blue;
    FastLED.show();
    delay(1000);
    FastLED.clear();
    FastLED.show();
    delay(1000);
}