/**
 * @file IQS5XX_B000_Trackpad.cpp
 * @brief Implementation of Arduino library for IQS5XX-B000 trackpad from Azoteq
 * @version 1.0.0
 * @date 2024
 * @author lemio
 * 
 * This library provides an interface to communicate with and monitor the 
 * IQS5XX-B000 capacitive trackpad sensor from Azoteq over I2C.
 * 
 * @copyright This project is licensed under the GNU General Public License v3.0
 */

#include "IQS5XX_B000_Trackpad.h"

IQS5XX_B000_Trackpad::IQS5XX_B000_Trackpad(uint8_t readyPin, uint8_t address) {
  _readyPin = readyPin;
  pinMode(_readyPin, INPUT);
  _address = address;
  _wire = nullptr;
  _lastTouchData = {0, 0, 0, 0, NO_TOUCH};
}

bool IQS5XX_B000_Trackpad::begin(TwoWire &wire) {
  _wire = &wire;
  _wire->begin();
  
  // Initial delay to allow device to stabilize
  delay(1);
  
  // Check if device responds at expected address
  _wire->beginTransmission(_address);
  uint8_t error = _wire->endTransmission();
  
  if (error == 0) {
    // Device found and already awake
  } else if (error == 2) {
    // Device found but needs wakeup - expect NACK initially
    if (!wakeupDevice()) {
      return false;
    }
  } else {
    // No device found
    return false;
  }
  
  // Verify device by reading product number
  uint16_t productNumber = getProductNumber();
  if (productNumber == 0) {
    return false;
  }
  
  // Check product ID (lower byte)
  uint8_t productID = productNumber & 0xFF;
  if (productID != 40 && productID != 58 && productID != 52) {
    // Not a recognized IQS5XX device (40=IQS550, 58=IQS572, 52=IQS525)
    return false;
  }
  
  // Enable manual control mode
  if (!enableManualControl()) {
    return false;
  }
  
  // Additional stabilization delay after configuration
  delay(1);
  
  return true;
}

bool IQS5XX_B000_Trackpad::isConnected() {
  if (_wire == nullptr) {
    return false;
  }
  
  _wire->beginTransmission(_address);
  return (_wire->endTransmission() == 0);
}

uint16_t IQS5XX_B000_Trackpad::getProductNumber() {
  if (_wire == nullptr) {
    return 0;
  }
  
  _wire->beginTransmission(_address);
  _wire->write((IQS5XX_REG_PRODUCT_NUMBER >> 8) & 0xFF); // High byte of address
  _wire->write(IQS5XX_REG_PRODUCT_NUMBER & 0xFF);        // Low byte of address
  
  if (_wire->endTransmission() != 0) {
    return 0;
  }
  
  if (_wire->requestFrom(_address, (uint8_t)2) != 2) {
    return 0;
  }
  
  uint8_t productHigh = _wire->read();
  uint8_t productLow = _wire->read();
  
  return (productHigh << 8) | productLow;
}

uint16_t IQS5XX_B000_Trackpad::getVersionInfo() {
  return readRegister16(IQS5XX_REG_VERSION_INFO);
}

uint8_t IQS5XX_B000_Trackpad::getSystemFlags() {
  return readRegister8(IQS5XX_REG_SYS_FLAGS);
}

bool IQS5XX_B000_Trackpad::needsReset() {
  uint8_t flags = getSystemFlags();
  return (flags & IQS5XX_SYS_FLAG_RESET) != 0;
}

bool IQS5XX_B000_Trackpad::readTouchData(TouchData &touchData) {
  // Wait for RDY pin to be LOW (device ready)
  while(digitalRead(_readyPin) == HIGH) { 
    delayMicroseconds(10); // Small delay to prevent busy waiting
  }
  
  // Read X coordinate (0x0016)
  touchData.x = readRegister16_16bit(IQS5XX_REG_TOUCH_X);
  if (touchData.x == 0) {
    touchData.state = NO_TOUCH;
    return false;
  }
  
  // Read Y coordinate (0x0018)  
  touchData.y = readRegister16_16bit(IQS5XX_REG_TOUCH_Y);
  if (touchData.y == 0) {
    touchData.state = NO_TOUCH;
    return false;
  }
  

  // Read gesture events 

  /* 
  bool swipeY_minus; //bit 5 GESTURE_EVENTS_0
  bool swipeY_plus;  //bit 4 GESTURE_EVENTS_0
  bool swipeX_plus;  //bit 3 GESTURE_EVENTS_0
  bool swipeX_minus; //bit 2 GESTURE_EVENTS_0
  bool pressAndHold; //bit 1 GESTURE_EVENTS_0
  bool singleTap;    //bit 0 GESTURE_EVENTS_0
  bool zoom;         //bit 2 GESTURE_EVENTS_1
  bool scroll;       //bit 1 GESTURE_EVENTS_1
  bool twoFingerTap; //bit 0 GESTURE_EVENTS_1
  */
  uint8_t gesture0 = readRegister8_16bit(IQS5XX_SYS_GESTURE_EVENTS_0);
  uint8_t gesture1 = readRegister8_16bit(IQS5XX_SYS_GESTURE_EVENTS_1);
  touchData.swipeY_minus = (gesture0 & 0b00100000) != 0;
  touchData.swipeY_plus  = (gesture0 & 0b00010000) != 0;
  touchData.swipeX_plus  = (gesture0 & 0b00001000) != 0;
  touchData.swipeX_minus = (gesture0 & 0b00000100) != 0;
  touchData.pressAndHold = (gesture0 & 0b00000010) != 0;
  touchData.singleTap    = (gesture0 & 0b00000001) != 0;
  touchData.zoom         = (gesture1 & 0b00000100) != 0;
  touchData.scroll       = (gesture1 & 0b00000010) != 0;
  touchData.twoFingerTap = (gesture1 & 0b00000001) != 0;

  // Read touch strength (0x001A)
  touchData.touchStrength = readRegister8_16bit(IQS5XX_REG_TOUCH_STRENGTH);
  
  // Read touch area (0x001B)
  touchData.area = readRegister8_16bit(IQS5XX_REG_AREA);
  
  // Determine touch state based on coordinates and strength
  if (touchData.touchStrength == 0) {
    touchData.state = NO_TOUCH;
  } else if (touchData.x == 0 && touchData.y == 0) {
    touchData.state = NO_TOUCH;
  } else {
    // For now, assume single touch - multi-touch detection would need
    // additional logic based on the specific IQS5XX variant capabilities
    touchData.state = SINGLE_TOUCH;
  }
  
  //Get the amount of fingers touching the trackpad
  uint8_t numFingers = readRegister8_16bit(IQS5XX_REG_NUM_FINGERS);
  touchData.numFingers = numFingers;
  _lastTouchData = touchData;
  return true;
}

TouchState IQS5XX_B000_Trackpad::getTouchState() {
  TouchData touchData;
  if (readTouchData(touchData)) {
    return touchData.state;
  }
  return NO_TOUCH;
}

uint16_t IQS5XX_B000_Trackpad::getTouchX() {
  TouchData touchData;
  if (readTouchData(touchData) && touchData.state != NO_TOUCH) {
    return touchData.x;
  }
  return 0;
}

uint16_t IQS5XX_B000_Trackpad::getTouchY() {
  TouchData touchData;
  if (readTouchData(touchData) && touchData.state != NO_TOUCH) {
    return touchData.y;
  }
  return 0;
}

uint8_t IQS5XX_B000_Trackpad::getTouchStrength() {
  TouchData touchData;
  if (readTouchData(touchData) && touchData.state != NO_TOUCH) {
    return touchData.touchStrength;
  }
  return 0;
}

uint8_t IQS5XX_B000_Trackpad::getTouchArea() {
  TouchData touchData;
  if (readTouchData(touchData) && touchData.state != NO_TOUCH) {
    return touchData.area;
  }
  return 0;
}

bool IQS5XX_B000_Trackpad::softReset() {
  // Implementation would depend on specific reset procedure for IQS5XX-B000
  // This is a placeholder for the reset functionality
  return writeRegister8(IQS5XX_REG_SYS_FLAGS, IQS5XX_SYS_FLAG_RESET);
}

bool IQS5XX_B000_Trackpad::wakeupDevice() {
  if (_wire == nullptr) {
    return false;
  }
  
  // First attempt - expect NACK (device is sleeping)
  _wire->beginTransmission(_address);
  uint8_t result1 = _wire->endTransmission();
  
  // Wait at least 150µs as required by datasheet
  delayMicroseconds(200); // 200µs to be safe
  
  // Second attempt - should get ACK if wakeup was successful
  _wire->beginTransmission(_address);
  uint8_t result2 = _wire->endTransmission();
  
  return (result2 == 0);
}

bool IQS5XX_B000_Trackpad::enableManualControl() {
  if (_wire == nullptr) {
    return false;
  }
  
  // Read current System Configuration 0 register (0x058E)
  uint8_t sysConf0 = readRegister8_16bit(IQS5XX_REG_SYS_CFG0);
  
  // Set bit 7 to 1 to enable manual control
  sysConf0 |= 0b10000000;
  
  // Write back the modified value
  return writeRegister8_16bit(IQS5XX_REG_SYS_CFG0, sysConf0);
}

bool IQS5XX_B000_Trackpad::isReadyForData() {
  return digitalRead(_readyPin) == LOW;
}

uint8_t IQS5XX_B000_Trackpad::readRegister8(uint8_t reg) {
  if (_wire == nullptr) {
    return 0;
  }
  
  _wire->beginTransmission(_address);
  _wire->write(reg);
  
  if (_wire->endTransmission() != 0) {
    return 0;
  }
  
  if (_wire->requestFrom(_address, (uint8_t)1) != 1) {
    return 0;
  }
  
  return _wire->read();
}

uint8_t IQS5XX_B000_Trackpad::readRegister8_16bit(uint16_t reg) {
  if (_wire == nullptr) {
    return 0;
  }
  
  _wire->beginTransmission(_address);
  _wire->write((reg >> 8) & 0xFF); // High byte of address
  _wire->write(reg & 0xFF);        // Low byte of address
  
  if (_wire->endTransmission() != 0) {
    return 0;
  }
  
  if (_wire->requestFrom(_address, (uint8_t)1) != 1) {
    return 0;
  }
  
  return _wire->read();
}

uint16_t IQS5XX_B000_Trackpad::readRegister16(uint8_t reg) {
  uint8_t buffer[2];
  if (!readBytes(reg, buffer, 2)) {
    return 0;
  }
  
  // Assuming little-endian format
  return (buffer[1] << 8) | buffer[0];
}

uint16_t IQS5XX_B000_Trackpad::readRegister16_16bit(uint16_t reg) {
  if (_wire == nullptr) {
    return 0;
  }
  
  _wire->beginTransmission(_address);
  _wire->write((reg >> 8) & 0xFF); // High byte of address
  _wire->write(reg & 0xFF);        // Low byte of address
  
  if (_wire->endTransmission() != 0) {
    return 0;
  }
  
  if (_wire->requestFrom(_address, (uint8_t)2) != 2) {
    return 0;
  }
  
  uint8_t high = _wire->read();
  uint8_t low = _wire->read();
  
  return (high << 8) | low;
}

bool IQS5XX_B000_Trackpad::writeRegister8(uint8_t reg, uint8_t value) {
  if (_wire == nullptr) {
    return false;
  }
  
  _wire->beginTransmission(_address);
  _wire->write(reg);
  _wire->write(value);
  
  return (_wire->endTransmission() == 0);
}

bool IQS5XX_B000_Trackpad::writeRegister8_16bit(uint16_t reg, uint8_t value) {
  if (_wire == nullptr) {
    return false;
  }
  
  _wire->beginTransmission(_address);
  _wire->write((reg >> 8) & 0xFF); // High byte of address
  _wire->write(reg & 0xFF);        // Low byte of address
  _wire->write(value);
  
  return (_wire->endTransmission() == 0);
}
bool IQS5XX_B000_Trackpad::writeRegister16(uint16_t reg, uint16_t value) {
  if (_wire == nullptr) {
    return false;
  }

  _wire->beginTransmission(_address);
  _wire->write((reg >> 8) & 0xFF); // High byte of address
  _wire->write(reg & 0xFF);        // Low byte of address
  _wire->write((value >> 8) & 0xFF); // High byte of value
  _wire->write(value & 0xFF);        // Low byte of value

  return (_wire->endTransmission() == 0);
}

bool IQS5XX_B000_Trackpad::readBytes(uint8_t reg, uint8_t* buffer, uint8_t length) {
  if (_wire == nullptr || buffer == nullptr || length == 0) {
    return false;
  }
  
  _wire->beginTransmission(_address);
  _wire->write(reg);
  
  if (_wire->endTransmission() != 0) {
    return false;
  }
  
  if (_wire->requestFrom(_address, length) != length) {
    return false;
  }
  
  for (uint8_t i = 0; i < length; i++) {
    buffer[i] = _wire->read();
  }
  
  return true;
}

bool IQS5XX_B000_Trackpad::increaseSpeed() {
  //Set the I2C timeout (0x058A) to a lower value (e.g., 5ms)
  //This means that the RDY pin is only LOW for 5 ms
  if (!trackpad.writeRegister8_16bit(IQS5XX_REG_I2C_TIMEOUT, 5)){
    return false;
  }
  // Set the Active Report Rate (0x057A) to a higher value (e.g., 5ms)
  // This means that the device will attempt to report data every 5 ms
  // instead of the default 100 ms
  if (!trackpad.writeRegister16(IQS5XX_REG_ACTIVE_REPORT_RATE, 5)) {
    return false;
  }
  return true;
}