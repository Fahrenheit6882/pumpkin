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
// Order matches the mp3s in sounds/ (see scripts/copy-sounds-in-order.ps1):
// 001_disco, 002_crazy_clown_laugh,
// 003_music_box, 004_organ, 005_cat, 006_electricty, 007_beast,
// 008_space_weasel, 009_ufo, 010_wolf
const unsigned long TRACK_DURATIONS[] = {15000, 10000, 30000, 20000, 8000, 8000, 24000, 8000, 14000, 10000};
const int TRACK_COUNT = sizeof(TRACK_DURATIONS) / sizeof(TRACK_DURATIONS[0]);
const bool SEQUENTIAL_MODE = true;

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
bool lastMotionState = false;
int nextTrack = 0;

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
    bool currentMotion = digitalRead(MOTION_PIN) == HIGH;
    Serial.println(currentMotion);
    if (currentMotion && !lastMotionState) {
      motionLastDetectedTime = millis();
      Serial.println("Motion detected");
      int track;
      if (SEQUENTIAL_MODE) {
        track = nextTrack;
        nextTrack = (nextTrack + 1) % TRACK_COUNT;
      } else {
        track = random(TRACK_COUNT);
      }
      motionDelayTime = TRACK_DURATIONS[track];
      clearLEDs();
      mp3.playFileByIndexNumber(track + 1);
      state = track + 1;
    }
    lastMotionState = currentMotion;
  } else if (state == 1) {
    loopDisco();
  } else if (state == 2) {
    loopLaugh();
  } else if (state == 3) {
    loopMusicBox();
  } else if (state == 4) {
    loopOrgan();
  } else if (state == 6) {
    loopElectricity();
  } else if (state == 7) {
    loopGreen();
  } else if (state == 8) {
    loopSpace();
  } else if (state == 10) {
    loopBlood();
  } else {
    loopLaugh();
  }

  if (state != 0) {
    if (millis() - motionLastDetectedTime >= motionDelayTime) {
      lastMotionState = digitalRead(MOTION_PIN) == HIGH;
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
  CRGB color = policePhase ? CRGB::Red : CRGB::Blue;
  leds[0] = color;
  leds[1] = color;
  policePhase = !policePhase;
  FastLED.show();
  delay(150);
  clearLEDs();
  delay(50);
}

void loopMachineGun() {
  unsigned long elapsed = millis() - motionLastDetectedTime;

  bool inWindow = (elapsed < 2000) ||
                  (elapsed >= 4000  && elapsed < 5000) ||
                  (elapsed >= 6500  && elapsed < 8000) ||
                  (elapsed >= 9250  && elapsed < 10000) ||
                  (elapsed >= 11250 && elapsed < 12250);

  if (inWindow) {
    leds[0] = CRGB::White;
    leds[1] = CRGB::White;
    FastLED.show();
    delay(40);
    clearLEDs();
    delay(40);
  } else {
    clearLEDs();
    delay(20);
  }
}

void loopLaugh() {
  leds[0] = CRGB::White;
  leds[1] = CRGB::White;
  FastLED.show();
  delay(80);
  clearLEDs();
  delay(80);
}

void loopOrgan() {
  leds[0] = CRGB::Blue;
  leds[1] = CRGB::Blue;
  FastLED.show();
  delay(20);
}

void loopMusicBox() {
  unsigned long elapsed = millis() - motionLastDetectedTime;
  int colorIndex = (elapsed / 500) % 4;
  CRGB color;
  switch (colorIndex) {
    case 0: color = CRGB::Red; break;
    case 1: color = CRGB::Blue; break;
    case 2: color = CRGB::Yellow; break;
    default: color = CRGB::Green; break;
  }
  leds[0] = color;
  leds[1] = color;
  FastLED.show();
  delay(20);
}

void loopElectricity() {
  const unsigned long BLINK_SLOT = 60;    // one on/off slot, ms
  const int BLINK_COUNT = 3;              // rapid purple blinks
  const unsigned long BLINK_PHASE = BLINK_SLOT * BLINK_COUNT * 2;
  const unsigned long WHITE_PHASE = 500;  // solid white, ms
  const unsigned long CYCLE = BLINK_PHASE + WHITE_PHASE;

  unsigned long elapsed = millis() - motionLastDetectedTime;
  unsigned long t = elapsed % CYCLE;

  if (t < BLINK_PHASE) {
    int slot = t / BLINK_SLOT;
    bool on = (slot % 2 == 0);
    if (on) {
      leds[0] = CRGB::Purple;
      leds[1] = CRGB::Purple;
      FastLED.show();
    } else {
      clearLEDs();
    }
  } else {
    leds[0] = CRGB::White;
    leds[1] = CRGB::White;
    FastLED.show();
  }
  delay(20);
}

void loopSpace() {
  CRGB color = policePhase ? CRGB::Purple : CRGB::Yellow;
  leds[0] = color;
  leds[1] = color;
  policePhase = !policePhase;
  FastLED.show();
  delay(150);
  clearLEDs();
  delay(50);
}

void loopGreen() {
  leds[0] = CRGB::Green;
  leds[1] = CRGB::Green;
  FastLED.show();
  delay(20);
}

void loopBlood() {
  leds[0] = CRGB::Red;
  leds[1] = CRGB::Red;
  FastLED.show();
  delay(20);
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