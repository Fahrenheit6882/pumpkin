#include <FastLED.h>

const int NUM_LEDS = 2;
const int LED_DATA_PIN = 6;

const uint8_t CANDLE_HUE = 20;
const uint8_t CANDLE_SAT = 240;
const uint8_t BRIGHTNESS_MIN = 90;
const uint8_t BRIGHTNESS_MAX = 255;

CRGB leds[NUM_LEDS];
uint8_t brightness[NUM_LEDS];
uint8_t target[NUM_LEDS];

void setup() {
  FastLED.addLeds<NEOPIXEL, LED_DATA_PIN>(leds, NUM_LEDS);
  for (int i = 0; i < NUM_LEDS; i++) {
    brightness[i] = random8(BRIGHTNESS_MIN, BRIGHTNESS_MAX);
    target[i] = brightness[i];
  }
}

void loop() {
  for (int i = 0; i < NUM_LEDS; i++) {
    if (random8() < 80) {
      target[i] = random8(BRIGHTNESS_MIN, BRIGHTNESS_MAX);
    }

    uint8_t step = random8(5, 25);
    if (brightness[i] < target[i]) {
      brightness[i] = qadd8(brightness[i], step);
    } else {
      brightness[i] = qsub8(brightness[i], step);
    }

    leds[i] = CHSV(CANDLE_HUE, CANDLE_SAT, brightness[i]);
  }

  FastLED.show();
  delay(random8(20, 50));
}
