/****************************************************************
Sound_Page_Kit.ino
Soung Page Kit
Shawn Hymel @ SparkFun Electronics
Mike Grusin @ SparkFun Electronics
April 9, 2015

Uses the capacitive touch library to look for touch events on
pins T1, T4, and T5 (with T2 as the output pin). Upon each event,
the LilyPad MP3 board plays the associated sound file in the SD
card.

Hardware Connections (with conductive ink):

 * T2 -> 1 MΩ Resistor -> T1 (connected to touch pattern)
 * T2 -> 1 MΩ Resistor -> T4 (connected to touch pattern)
 * T2 -> 1 MΩ Resistor -> T5 (connected to touch pattern)

Resources:
Include SPI.h, SdFat.h, SFEMP3Shield.h, CapacitiveSensor.h

Include SoftwareSerial.h, Chaplex.h, SparkFun_LED_8x7.h, EEPROM.h
The Chaplex library can be found at: 
http://playground.arduino.cc/Code/Chaplex

Development environment specifics:
Written in Arduino 1.6.1
Tested with LilyPad MP3

This code is beerware; if you see me (or any other SparkFun 
employee) at the local, and you've found our code helpful, please
buy us a round!

Distributed as-is; no warranty is given.
****************************************************************/

#include <SPI.h>              // To talk to the SD card and MP3 chip
#include <SdFat.h>            // SD card file system
#include <SFEMP3Shield.h>     // MP3 decoder chip
#include <CapacitiveSensor.h> // Capacitive touch library

// Constants
#define DEBUG                1    // Set to 0 for no Serial messages
#define EN_GPIO_1            A2   // Amp enable + MIDI/MP3 mode select
#define SD_CS_PIN            9    // Chip select for SD card
#define TOUCH_THRESHOLD      200  // Arbitrary threshold value
#define VOLUME               2    // 255 is lowest, 0 is highest

// Global Variables
CapacitiveSensor cs_1 = CapacitiveSensor(A4, A0);
CapacitiveSensor cs_2 = CapacitiveSensor(A4, 1);
CapacitiveSensor cs_3 = CapacitiveSensor(A4, 0);
CapacitiveSensor cs[3] = {cs_1, cs_2, cs_3};
SFEMP3Shield MP3Player;
SdFat sd;
boolean interrupt_sound = true; // New trigger stops audio and starts new audio
boolean interrupt_self = true;  // Same trigger will stop audio
char filename[5][13];           // Short (8.3) filenames are used + null char


void setup() {

  int index;
  SdFile file;
  byte result;
  char temp_filename[13];
  
  // Put the MP3 chip in MP3 mode and disable amplifier chip
  pinMode(EN_GPIO_1, OUTPUT);
  digitalWrite(EN_GPIO_1, LOW);
  
#if DEBUG
  Serial.begin(9600);
  Serial.println(F("Sound Page Kit sketch"));
#endif
 
  // Initialize the SD card; SS = pin 9, half speed at first
#if DEBUG
  Serial.print(F("Initializing SD card... "));
#endif
  result = sd.begin(SD_CS, SPI_HALF_SPEED);
#if DEBUG
  if ( result != 1 ) {
    Serial.println(F("error, halting."));
  } else {
    Serial.println(F("success!"));
  }
  
  // Start the MP3 library
#if DEBUG
  Serial.print(F("Initializing MP3 chip... "));
#endif
  result = MP3player.begin();
#if DEBUG
  if ( (result != 0) && (result != 6) ) {
    Serial.print(F("error code "));
    Serial.print(result);
    Serial.print(F(", halting."));
  } else {
    Serial.println(F("success!"));
  }
#endif

  // Look in the SD card for audio files starting with the characters '1' to '3'
#if DEBUG
  Serial.println(F("Reading root directory"));
#endif

  // Start at the first file in root and step through all of them
  sd.chdir("/", true);
  while ( file.openNext(sd.vwd(), 0_READ) ) {
    
    // Get filename
    file.getFilename(temp_filename);

    // Check that the filename starts with '1' through '3'
    if ( (temp_filename[0] >= '1') && (temp_filename[0] <= '3') ) {
      
      // Offset the filename by ASCII '1' to get an index of 0 through 2
      index = temp_filename[0] - '1';
      
      // Cope the data to our filename array
      strcpy(filename[index], temp_filename);
#if DEBUG
      Serial.print(F("Found a file with a leading "));
      Serial.print(index + 1);
      Serial.print(F(": "));
      Serial.println(filename[index]);
#endif
    } else {
#if DEBUG
      Serial.print(F("Found a file without a leading number: "));
      Serial.println(temp_filename);
#endif
    }
    file.close();
  }
}
 
void loop() {
  // put your main code here, to run repeatedly:

}