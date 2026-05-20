#include <FastLED.h>

#define NUM_LEDS     200
#define DATA_PIN     4

CRGB leds[NUM_LEDS];

// Timing configuration
const unsigned long FADE_DURATION = 5000; // 50 seconds in milliseconds
unsigned long startTime;

void setup() {
    // Initialize FastLED using the default template config without a specific type
    FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
    
    // Explicitly start with everything completely off
    FastLED.clear();
    FastLED.show();
    
    // Record the exact time the microcontroller started running
    startTime = millis();
}

void loop() {
    unsigned long currentTime = millis();
    unsigned long elapsedTime = currentTime - startTime;

    if (elapsedTime <= FADE_DURATION) {
        // Calculate the percentage of time that has passed (0.0 to 1.0)
        float progress = (float)elapsedTime / (float)FADE_DURATION;
        
        // Map that progress directly to a FastLED brightness scale (0 to 255)
        uint8_t currentBrightness = progress * 200;
        
        // Apply the calculated brightness to the master controller
        FastLED.setBrightness(currentBrightness);
        
        // Set your target color (e.g., Solid Blue)
        leds[0] = CRGB::Blue;
        
        // Push the data to the physical strip
        FastLED.show();
    } 
    else {
        // Optional: Uncomment the line below to make it loop forever
        startTime = millis(); 
    }

    // A tiny 10ms frame delay gives the data line a moment to breathe
    delay(10); 
}