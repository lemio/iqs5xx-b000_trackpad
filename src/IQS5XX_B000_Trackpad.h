/**
 * @file IQS5XX_B000_Trackpad.h
 * @brief Arduino library for IQS5XX-B000 trackpad from Azoteq
 * @version 1.0.0
 * @date 2024
 * @author lemio
 * 
 * This library provides an interface to communicate with and monitor the 
 * IQS5XX-B000 capacitive trackpad sensor from Azoteq over I2C.
 * 
 * @copyright This project is licensed under the GNU General Public License v3.0
 */

#ifndef IQS5XX_B000_TRACKPAD_H
#define IQS5XX_B000_TRACKPAD_H

#include <Arduino.h>
#include <Wire.h>

//Datasheet https://www.azoteq.com/images/stories/pdf/iqs5xx-b000_trackpad_datasheet.pdf#page=31


// Default I2C address for IQS5XX-B000
#define IQS5XX_DEFAULT_ADDRESS 0x74

// Device Information Registers (0x00 - 0x0F)
#define IQS5XX_REG_PRODUCT_NUMBER     0x00    // Product number
#define IQS5XX_REG_SOFTWARE_NUMBER    0x02    // Software number
#define IQS5XX_REG_MAJOR_VERSION      0x04    // Major version number
#define IQS5XX_REG_MINOR_VERSION      0x05    // Minor version number
#define IQS5XX_REG_BL_STATUS          0x06    // Bootloader status

// System Information Registers (0x10 - 0x1F)
#define IQS5XX_REG_SYSTEM_FLAGS       0x10    // System flags
#define IQS5XX_REG_XY_INFO0           0x11    // Number of touches and palm rejection info
#define IQS5XX_REG_REL_X              0x12    // Relative X coordinate
#define IQS5XX_REG_REL_Y              0x14    // Relative Y coordinate
#define IQS5XX_REG_TOUCH_X            0x16    // Absolute X coordinate
#define IQS5XX_REG_TOUCH_Y            0x18    // Absolute Y coordinate
#define IQS5XX_REG_TOUCH_STRENGTH     0x1A    // Touch strength
#define IQS5XX_REG_AREA               0x1B    // Touch area

#define IQS5XX_REG_ACTIVE_REPORT_RATE 0x057A  // Active report rate
#define IQS5XX_REG_I2C_TIMEOUT        0x058A  // I2C timeout

// System Configuration Registers (0x430 - 0x43F)
#define IQS5XX_REG_SYS_CNT0           0x0431  // System control 0
#define IQS5XX_REG_SYS_CNT1           0x0432  // System control 1

// System Configuration Registers (0x580 - 0x58F)
#define IQS5XX_REG_SYS_CFG0           0x058E  // System configuration 0
#define IQS5XX_REG_SYS_CFG1           0x058F  // System configuration 1

// Legacy register aliases for backward compatibility
#define IQS5XX_REG_VERSION_INFO       0x01
#define IQS5XX_REG_SYS_FLAGS          0x10
#define IQS5XX_REG_COORDINATES        0x11

// System flags bits
#define IQS5XX_SYS_FLAG_RESET         0x80
#define IQS5XX_SYS_FLAG_ATE_ERROR     0x40
#define IQS5XX_SYS_FLAG_REATI         0x20
#define IQS5XX_SYS_FLAG_ALP_ATI       0x10
#define IQS5XX_SYS_FLAG_LP_ATI        0x08
#define IQS5XX_SYS_FLAG_SNAP          0x02
#define IQS5XX_SYS_FLAG_SETUP         0x01

// Touch states
enum TouchState {
  NO_TOUCH = 0,
  SINGLE_TOUCH = 1,
  MULTI_TOUCH = 2
};

// Touch data structure
struct TouchData {
  uint16_t x;
  uint16_t y;
  uint8_t touchStrength;
  uint8_t area;
  TouchState state;
};

/**
 * @class IQS5XX_B000_Trackpad
 * @brief Main class for interfacing with the IQS5XX-B000 trackpad
 */
class IQS5XX_B000_Trackpad {
  public:
    /**
     * @brief Constructor for IQS5XX_B000_Trackpad
     * @param address I2C address of the device (default: IQS5XX_DEFAULT_ADDRESS)
     */
    IQS5XX_B000_Trackpad(uint8_t readyPin, uint8_t address = IQS5XX_DEFAULT_ADDRESS);
    
    /**
     * @brief Initialize the trackpad
     * @param wire Reference to Wire interface (default: Wire)
     * @return true if initialization successful, false otherwise
     */
    bool begin(TwoWire &wire = Wire);
    
    /**
     * @brief Check if device is connected and responding
     * @return true if device is connected, false otherwise
     */
    bool isConnected();
    
    /**
     * @brief Read product number from device
     * @return Product number, or 0 if read failed
     */
    uint16_t getProductNumber();
    
    /**
     * @brief Read version information from device
     * @return Version info, or 0 if read failed
     */
    uint16_t getVersionInfo();
    
    /**
     * @brief Read system flags
     * @return System flags byte
     */
    uint8_t getSystemFlags();
    
    /**
     * @brief Check if device needs reset based on system flags
     * @return true if reset needed, false otherwise
     */
    bool needsReset();
    
    /**
     * @brief Read touch data from the trackpad
     * @param touchData Reference to TouchData structure to fill
     * @return true if read successful, false otherwise
     */
    bool readTouchData(TouchData &touchData);
    
    /**
     * @brief Check if there is currently a touch detected
     * @return TouchState enum value
     */
    TouchState getTouchState();
    
    /**
     * @brief Get X coordinate of touch (if available)
     * @return X coordinate (0-65535), or 0 if no touch
     */
    uint16_t getTouchX();
    
    /**
     * @brief Get Y coordinate of touch (if available)  
     * @return Y coordinate (0-65535), or 0 if no touch
     */
    uint16_t getTouchY();
    
    /**
     * @brief Get touch strength
     * @return Touch strength value, or 0 if no touch
     */
    uint8_t getTouchStrength();
    
    /**
     * @brief Get touch area
     * @return Touch area value, or 0 if no touch
     */
    uint8_t getTouchArea();
    
    /**
     * @brief Perform soft reset of the device
     * @return true if reset successful, false otherwise
     */
    bool softReset();
    
    /**
     * @brief Perform proper wakeup sequence with NACK/ACK handling
     * @return true if wakeup successful, false otherwise
     */
    bool wakeupDevice();
    
    /**
     * @brief Enable manual control mode by setting bit 7 of System Config 0
     * @return true if successful, false otherwise
     */
    bool enableManualControl();
    
    /**
     * @brief Check if device is ready for data (RDY pin low)
     * @return true if ready, false otherwise
     */
    bool isReadyForData();

    /**
     * @brief Increase communication speed by reducing the I2C timeout and increasing the report rate
     * @return true if successful, false otherwise
     */
    bool increaseSpeed();

    /**
     * @brief Write 16-bit value to 16-bit register address
     * @param reg 16-bit register address
     * @param value Value to write
     * @return true if write successful, false otherwise
     */
    bool writeRegister16(uint16_t reg, uint16_t value);
    
    /**
     * @brief Write 8-bit value to register
     * @param reg Register address
     * @param value Value to write
     * @return true if write successful, false otherwise
     */
    bool writeRegister8(uint8_t reg, uint8_t value);

    /**
     * @brief Write 8-bit value to 16-bit register address
     * @param reg 16-bit register address
     * @param value Value to write
     * @return true if write successful, false otherwise
     */
    bool writeRegister8_16bit(uint16_t reg, uint8_t value);

  private:
    uint8_t _readyPin;
    uint8_t _address;
    TwoWire* _wire;
    TouchData _lastTouchData;
    
    /**
     * @brief Read 8-bit value from register
     * @param reg Register address
     * @return Register value, or 0 if read failed
     */
    uint8_t readRegister8(uint8_t reg);
    
    /**
     * @brief Read 8-bit value from 16-bit register address
     * @param reg 16-bit register address
     * @return Register value, or 0 if read failed
     */
    uint8_t readRegister8_16bit(uint16_t reg);
    
    /**
     * @brief Read 16-bit value from register
     * @param reg Register address
     * @return Register value, or 0 if read failed
     */
    uint16_t readRegister16(uint8_t reg);
    
    /**
     * @brief Read 16-bit value from 16-bit register address
     * @param reg 16-bit register address
     * @return Register value, or 0 if read failed
     */
    uint16_t readRegister16_16bit(uint16_t reg);
    
    /**
     * @brief Read multiple bytes from device
     * @param reg Starting register address
     * @param buffer Buffer to store read data
     * @param length Number of bytes to read
     * @return true if read successful, false otherwise
     */
    bool readBytes(uint8_t reg, uint8_t* buffer, uint8_t length);
};

#endif // IQS5XX_B000_TRACKPAD_H