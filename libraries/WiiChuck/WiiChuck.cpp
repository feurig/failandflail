/*
 * WiiChuck
 * Flamingo EDA http://www.flamingoeda.com
 * 
 */

#include "WiiChuck.h"

void WiiChuck::init()
{
  Wire.begin();
  Wire.beginTransmission(WII_I2C_ADDR);
  Wire.send(0x40);
  Wire.send(0x00);
  Wire.endTransmission();
}

void WiiChuck::initWithPower()
{
  initWithPowerPins(PORTC3, PORTC2);
}

void WiiChuck::initWithPowerPins(byte powerPin, byte gndPin)
{
  DDRC |= _BV(powerPin) | _BV(gndPin);
  PORTC &= ~_BV(gndPin);
  PORTC |= _BV(powerPin);
  delay(100);
  init();
}

boolean WiiChuck::read()
{
  int count = 0;
  Wire.requestFrom(WII_I2C_ADDR, PACKAGE_LENGTH);
  while(Wire.available()) {
    buffer[count++] = decodeByte(Wire.receive());
  }
  // send request for next bytes
  sendNullRequest();

  if (count < 5) {
    return false;
  }

  // update data

  joyAxisX = buffer[0];
  joyAxisY = buffer[1];
  accelAxisX = buffer[2];
  accelAxisY = buffer[3];
  accelAxisZ = buffer[4];
  
  buttonZ = 0;
  buttonC = 0;
  if ((buffer[5] >> 0) & 1) {
    buttonZ = 1;
  }
  if ((buffer[5] >> 1) & 1) {
    buttonC = 1;
  }

  if ((buffer[5] >> 2) & 1) {
    accelAxisX += 2;
  }
  if ((buffer[5] >> 3) & 1) {
    accelAxisX += 1;
  }

  if ((buffer[5] >> 4) & 1) {
    accelAxisY += 2;
  }
  if ((buffer[5] >> 5) & 1) {
    accelAxisY += 1;
  }

  if ((buffer[5] >> 2) & 1) {
    accelAxisZ += 2;
  }
  if ((buffer[5] >> 3) & 1) {
    accelAxisZ += 1;
  }

  return true;
}

int WiiChuck::getJoyAxisX()
{
  return joyAxisX;
}

int WiiChuck::getJoyAxisY()
{
  return joyAxisY;
}

int WiiChuck::getAccelAxisX()
{
  return accelAxisX;
}

int WiiChuck::getAccelAxisY()
{
  return accelAxisY;
}

int WiiChuck::getAccelAxisZ()
{
  return accelAxisZ;
}

int WiiChuck::getButtonZ()
{
  return buttonZ;
}

int WiiChuck::getButtonC()
{
  return buttonC;
} 

/*****************************************************************************
 *  PRIVATE METHODS                                                            
 ****************************************************************************/

byte WiiChuck::decodeByte(byte x)
{
  x = (x ^ 0x17) + 0x17;
  return x;
}

void WiiChuck::sendNullRequest()
{
  Wire.beginTransmission(WII_I2C_ADDR);
  Wire.send(0x00);
  Wire.endTransmission();
}
