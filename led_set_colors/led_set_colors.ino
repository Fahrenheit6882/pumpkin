#include <FastLED.h>

#define NUM_LEDS 200  
#define DATA_PIN 5

CRGB leds[NUM_LEDS];

const long DELAY = 300;

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

void setPixelColor(int index, CRGB color) {
  if (index >= 0 && index < NUM_LEDS) {
    leds[index] = color;
  }
}

void showAndDelay(long delayTime) {
  FastLED.show();
  delay(delayTime);
}

void setPixelColorAndDelay(int index, CRGB color) {
  setPixelColor(index, color);
  showAndDelay(DELAY);
}

void loop() { 
    // Turn ONLY the very first LED solid Green
    setPixelColorAndDelay(0, CRGB::Blue);
    setPixelColorAndDelay(1, CRGB::DimGrey);
    setPixelColorAndDelay(2, CRGB::ForestGreen);
    setPixelColorAndDelay(3, CRGB::Plum);

    FastLED.clear();
    FastLED.show();
    delay(1000);
}