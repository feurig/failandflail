/*
 * WiiChuck
 * Flamingo EDA http://www.flamingoeda.com
 * 
 */

#ifndef WII_CHUCK_H
#define WII_CHUCK_H

#include <WProgram.h>
#include <Wire.h>


#define WII_I2C_ADDR   0x52
#define PACKAGE_LENGTH 6

extern TwoWire Wire;

class WiiChuck {

 public:
  /**
   * Initialize and join the I2C bus.
   */
  void init();

  /**
   * Initialize and join I2C bus with default power pins.
   * POWER: PC3 (Analog 3 pin in Arduino)
   *   GND: PC2 (Analog 2 pin in Arduino)
   */
  void initWithPower();

  /**
   * Initialize and join I2C bus with specified power pins.
   */
  void initWithPowerPins(byte powerPin, byte gndPin);

  /**
   * Read data from nunchuck.
   */
  boolean read();

  int getJoyAxisX();
  int getJoyAxisY();
  int getAccelAxisX();
  int getAccelAxisY();
  int getAccelAxisZ();
  int getButtonZ();
  int getButtonC(); 

 private:
  byte decodeByte(byte x);
  void sendNullRequest();

 private:
  uint8_t buffer[PACKAGE_LENGTH];
  int joyAxisX;
  int joyAxisY;
  int accelAxisX;
  int accelAxisY;
  int accelAxisZ;
  int buttonZ;
  int buttonC; 
};


#endif

