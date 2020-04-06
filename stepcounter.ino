#include <SPI.h>
#include <EEPROM.h>
#include <SparkFun_ADXL345.h>

const int serialBaudRate = 9600;
const int chipSelectPin = 10; // CS pin for SPI
const int ledPin = 2;         // The LED blinks when a step is detected
const int jumperPin = 3;      // Put this pin low to read the step count
const int eepromAddress = 0x01; // Address where to store the step count
const int rangeSetting = 4;  // Sensitivity of the ADXL345 (2, 4, 8 or 16G)
const int tapThreshold = 70; // 62.5 mG per increment
const int tapDuration = 150; // 625 μs per increment 

unsigned int numberOfSteps = 0; // Total count of steps

ADXL345 adxl = ADXL345(chipSelectPin);

void setup() {
  Serial.begin(serialBaudRate);

  pinMode(jumperPin, INPUT_PULLUP);
  if (!digitalRead(jumperPin))           // if the pin is low, read and print the step count
  {
    unsigned int steps = 0;
    Serial.print("Nombre de pas : ");
    EEPROM.get(eepromAddress, steps);
    Serial.println(steps, DEC);
    while (1);
  }

  EEPROM.put(eepromAddress, 0);         // Reset the step count in eeprom

  adxl.powerOn();
  adxl.setRangeSetting(rangeSetting);
  adxl.setSpiBit(0);
  adxl.setActivityXYZ(0, 0, 0);
  adxl.setInactivityXYZ(0, 0, 0);
  adxl.setTapDetectionOnXYZ(0, 0, 1);
  adxl.setTapThreshold(tapThreshold);
  adxl.setTapDuration(tapDuration);
  adxl.InactivityINT(0);
  adxl.ActivityINT(0);
  adxl.FreeFallINT(0);
  adxl.doubleTapINT(0);
  adxl.singleTapINT(1);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
}

void loop() {
  byte interrupts = adxl.getInterruptSource();
  if(adxl.triggered(interrupts, ADXL345_SINGLE_TAP)){
    Serial.println("** STEP DETECTED! **");
    
    digitalWrite(ledPin, HIGH);        // Blinks the LED
    delay(250);
    digitalWrite(ledPin, LOW);
    delay(100);

    EEPROM.put(eepromAddress, ++numberOfSteps);
  }
}
