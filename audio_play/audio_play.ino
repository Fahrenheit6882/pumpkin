#include <SoftwareSerial.h>
#include <JQ6500_Serial.h>

// Define our pins:
// Nano D2 connects to JQ6500 TX
// Nano D3 connects to JQ6500 RX (via 1k ohm resistor)
SoftwareSerial mySoftwareSerial(2, 3);
JQ6500_Serial mp3(mySoftwareSerial);

void setup() {
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

void loop() {
  // Command the JQ6500 to play the very first track we loaded
  mp3.playFileByIndexNumber(2);
  
  // Wait 10 seconds before sending the command again
  delay(20000);
}