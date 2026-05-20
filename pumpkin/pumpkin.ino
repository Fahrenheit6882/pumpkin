#include <SoftwareSerial.h>
#include <JQ6500_Serial.h>
#include <FastLED.h>

// LEDS
const int NUM_LEDS = 200;
const int LED_DATA_PIN = 6;
const int LED_BRIGHTNESS = 50;
CRGB leds[NUM_LEDS];

// Audio
SoftwareSerial mySoftwareSerial(2, 3);
JQ6500_Serial mp3(mySoftwareSerial);
const unsigned long TRACK_DURATIONS[] = {15000, 10000};
const int TRACK_COUNT = 2;

// Candle
const uint8_t CANDLE_HUE = 20;
const uint8_t CANDLE_SAT = 240;
const uint8_t CANDLE_BRIGHTNESS_MIN = 90;
const uint8_t CANDLE_BRIGHTNESS_MAX = 255;
const int CANDLE_LEDS = 2;
uint8_t candleBrightness[CANDLE_LEDS];
uint8_t candleTarget[CANDLE_LEDS];

// Motion
const int MOTION_PIN = 5;
unsigned long motionDelayTime;
unsigned long motionLastDetectedTime;

// States
// 1 - Motion detected
// 0 - No motion
int state;
bool policePhase = false;

void setup() {
  Serial.begin(9600);
  setupLEDs();
  setupMotion();
  setupAudio();
  state = 0;
  Serial.println("Setup complete");
}

void loop() {
  if (state == 0) {
    loopCandle();
    int motionState = digitalRead(MOTION_PIN);
    Serial.println(motionState);
    if (motionState == HIGH) {
      motionLastDetectedTime = millis();
      Serial.println("Motion detected");
      int track = random(TRACK_COUNT);
      motionDelayTime = TRACK_DURATIONS[track];
      clearLEDs();
      mp3.playFileByIndexNumber(track + 1);
      state = track + 1;
    }
  } else if (state == 1) {
    loopDisco();
  } else if (state == 2) {
    loopPolice();
  }

  if (state != 0) {
    if (millis() - motionLastDetectedTime >= motionDelayTime) {
      resetLEDs();
      state = 0;
    }
  }
}

void clearLEDs() {
  FastLED.clear();
  FastLED.show();
}

void setupLEDs() {
    FastLED.addLeds<NEOPIXEL, LED_DATA_PIN>(leds, NUM_LEDS);
    FastLED.setBrightness(LED_BRIGHTNESS); 

    resetLEDs();
}

void setPixelColor(int index, CRGB color) {
  if (index >= 0 && index < NUM_LEDS) {
    leds[index] = color;
  }
}

void resetLEDs() {
  for (int i = 0; i < CANDLE_LEDS; i++) {
    candleBrightness[i] = random8(CANDLE_BRIGHTNESS_MIN, CANDLE_BRIGHTNESS_MAX);
    candleTarget[i] = candleBrightness[i];
  }
}

void loopPolice() {
  if (policePhase) {
    leds[0] = CRGB::Red;
    leds[1] = CRGB::Blue;
  } else {
    leds[0] = CRGB::Blue;
    leds[1] = CRGB::Red;
  }
  policePhase = !policePhase;
  FastLED.show();
  delay(150);
}

void loopDisco() {
  leds[0] = CHSV(random8(), 255, 255);
  leds[1] = CHSV(random8(), 255, 255);
  FastLED.show();
  delay(random8(30, 80));
}

void loopCandle() {
  for (int i = 0; i < CANDLE_LEDS; i++) {
    if (random8() < 80) {
      candleTarget[i] = random8(CANDLE_BRIGHTNESS_MIN, CANDLE_BRIGHTNESS_MAX);
    }
    uint8_t step = random8(5, 25);
    if (candleBrightness[i] < candleTarget[i]) {
      candleBrightness[i] = qadd8(candleBrightness[i], step);
    } else {
      candleBrightness[i] = qsub8(candleBrightness[i], step);
    }
    leds[i] = CHSV(CANDLE_HUE, CANDLE_SAT, candleBrightness[i]);
  }
  FastLED.show();
  delay(random8(20, 50));
}

void setupAudio() {
  // Start serial communication at the required 9600 baud rate
  mySoftwareSerial.begin(9600);
  
  // 1. Give the sound board 2 full seconds to wake up after powering on
  delay(2000);
  
  // 2. Clear any frozen states on the chip
  mp3.reset();
  delay(1000); 
  
  // 3. Force the chip to look at its internal flash memory (not an SD card)
  mp3.setSource(MP3_SRC_BUILTIN);
  delay(500);
  
  // 4. Set the volume level (0 is mute, 30 is maximum)
  mp3.setVolume(30);
  delay(500);
}

void setupMotion() {
  pinMode(MOTION_PIN, INPUT);
  motionLastDetectedTime = millis();
}