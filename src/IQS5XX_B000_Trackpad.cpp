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

IQS5XX_B000_Trackpad::IQS5XX_B000_Trackpad(uint8_t address) {
  _address = address;
  _wire = nullptr;
  _lastTouchData = {0, 0, 0, 0, NO_TOUCH};
}

bool IQS5XX_B000_Trackpad::begin(TwoWire &wire) {
  _wire = &wire;
  _wire->begin();
  
  // Check if device is connected
  if (!isConnected()) {
    return false;
  }
  
  // Perform initial setup if needed
  delay(10); // Allow device to stabilize
  
  // Check if reset is needed
  if (needsReset()) {
    if (!softReset()) {
      return false;
    }
    delay(100); // Wait for reset to complete
  }
  
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
  return readRegister16(IQS5XX_REG_PRODUCT_NUMBER);
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
  uint8_t buffer[8];
  
  // Read coordinate and touch data starting from coordinates register
  if (!readBytes(IQS5XX_REG_COORDINATES, buffer, 8)) {
    return false;
  }
  
  // Parse coordinate data (assuming little-endian format)
  touchData.x = (buffer[1] << 8) | buffer[0];
  touchData.y = (buffer[3] << 8) | buffer[2];
  
  // Read touch strength and area
  touchData.touchStrength = buffer[4];
  touchData.area = buffer[6];
  
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

uint16_t IQS5XX_B000_Trackpad::readRegister16(uint8_t reg) {
  uint8_t buffer[2];
  if (!readBytes(reg, buffer, 2)) {
    return 0;
  }
  
  // Assuming little-endian format
  return (buffer[1] << 8) | buffer[0];
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