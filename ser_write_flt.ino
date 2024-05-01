//
//#include <SPI.h>
//#include <Wire.h>
//#include <Adafruit_GFX.h>
//#include <Adafruit_SSD1306.h>
//
//#define OLED_RESET 4
//Adafruit_SSD1306 display(OLED_RESET);
#include "HX711.h"
#include <Encoder.h>

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 3;
const int LOADCELL_SCK_PIN = 4;
const byte interruptPin =2;
long reading,offset=0;
float weight=0;
//uint16_t enc_=0;
//bool state = HIGH;
int ledPin = 10;
volatile byte state = LOW;
signed int emg1_ =A1;
signed int emg2_ =A0;
Encoder myEnc(22, 23);

HX711 scale;
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
void setup() {
  Serial.begin(115200); // Initialize serial communication
//  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
//  display.clearDisplay();
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), blink, CHANGE);
  pinMode(ledPin,OUTPUT);
}


void loop() {
  read_data();
  write_data();
  display.clearDisplay();
  if (Serial.available() >= 5) { // Ensure that at least 5 bytes are available for processing
    byte header1 = Serial.read();
    byte header2 = Serial.read();
    byte payloadSize = Serial.read()-1;
    byte payload[payloadSize];
    
    // Read the payload bytes
    for (int i = 0; i < payloadSize; i++) {
      payload[i] = Serial.read();
    }

    byte checksum = Serial.read();

    // Verify checksum
    byte calculatedChecksum = 0;
    calculatedChecksum += header1;
    calculatedChecksum += header2;
    calculatedChecksum += payloadSize+1;
    for (int i = 0; i < payloadSize; i++) {
      calculatedChecksum += payload[i];
    }
    calculatedChecksum %= 256;


    if (calculatedChecksum == checksum) {
      // Convert payload bytes to float
      float received_float;
      memcpy(&received_float, payload, sizeof(float));

      // Process the received float value
      Serial.print("Received float value: ");
      Serial.println(received_float);
       display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(10,20);
      display.println(received_float);
      display.display();
      
      // Checksum is valid, process the payload
      // Here you can do whatever you need with the received payload
      
    } else {
      // Checksum error
      Serial.println("Checksum error");
    }
  }
}
