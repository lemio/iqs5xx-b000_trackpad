# IQS5XX-B000 Trackpad Library

https://github.com/user-attachments/assets/17cc72a4-d8b3-4ad5-9ea0-216d676c8c09



An Arduino library for interfacing with the IQS5XX-B000 capacitive trackpad sensor from Azoteq. This library provides easy-to-use functions for touch detection, coordinate tracking, and gesture recognition over I2C communication.

## Features

- ✅ I2C communication with IQS5XX-B000 trackpad
- ✅ Touch detection and coordinate tracking
- ✅ Touch strength and area measurement
- ✅ System status monitoring and error handling
- ✅ Device initialization and reset functionality
- ✅ Manual control mode for continuous updates
- ✅ Device wakeup from sleep mode
- ✅ Compatible with Arduino, ESP32, and other Arduino-compatible boards
- ✅ Support for multiple IQS5XX variants (IQS550, IQS525, IQS572)

## Hardware Requirements

- Arduino-compatible board (Arduino Uno, ESP32, etc.)
- IQS5XX-B000 trackpad sensor
- I2C connection (SDA/SCL pins)
- **RDY (Ready) pin connection (required)**
- Pull-up resistors for I2C lines (usually 4.7kΩ)

## Wiring

| IQS5XX-B000 Pin | Arduino Uno | ESP32 | Description |
|-----------------|-------------|-------|-------------|
| VCC | 3.3V or 5V | 3.3V | Power supply |
| GND | GND | GND | Ground |
| SDA | A4 | GPIO21 | I2C Data |
| SCL | A5 | GPIO22 | I2C Clock |
| RDY | Digital Pin (**required**) | GPIO (**required**) | Ready signal |

**Note:** The RDY pin connection is mandatory for proper operation as it's used to determine when the device has data ready.

## Installation

### Arduino Library Manager (Recommended)
1. Open Arduino IDE
2. Go to Sketch → Include Library → Manage Libraries
3. Search for "IQS5XX-B000 Trackpad"
4. Click Install

### Manual Installation
1. Download this repository as ZIP
2. In Arduino IDE, go to Sketch → Include Library → Add .ZIP Library
3. Select the downloaded ZIP file

## Quick Start
```c++
#include <Wire.h>
#include <IQS5XX_B000_Trackpad.h>

#define RDY_PIN 2  // Connect RDY pin to digital pin 2

IQS5XX_B000_Trackpad trackpad(RDY_PIN);  // RDY pin is required

void setup() {
  Serial.begin(9600);
  
  if (trackpad.begin()) {
    Serial.println("Trackpad initialized!");
    
    // Enable manual control mode for continuous updates
    if (trackpad.enableManualControl()) {
      Serial.println("Manual control enabled!");
    }
    
    // Display device info
    Serial.print("Product Number: 0x");
    Serial.println(trackpad.getProductNumber(), HEX);
  } else {
    Serial.println("Trackpad initialization failed!");
  }
}

void loop() {
  TouchData touchData;
  
  if (trackpad.readTouchData(touchData)) {
    if (touchData.state == SINGLE_TOUCH) {
      Serial.print(touchData.x);
      Serial.print(",");
      Serial.print(touchData.y);
      Serial.print(",");
      Serial.print(touchData.touchStrength);
    }
  }
  
  delay(50);
}
```

## Plotter

To visualize touch data from the IQS5XX-B000 trackpad, you can use the built-in Plotter feature in the Arduino IDE. This allows you to see real-time graphs of touch coordinates and other parameters. Another option is to use web-based serial plotter specifically designed for touchpads in the web folder.

If you have node installed:
```
npx http-server ./web
```

Or if you have python3 installed:
```
python3 -m http.server --directory ./web 8080
```

Then open your browser and navigate to `http://localhost:8080/plotter.html` (or the port shown in your terminal). Here you can connect to the Aruino's serial port using webserial and visualize the touch data.

## API Reference

### Constructor

**Important:** The constructor requires a ready pin parameter for proper operation.
```c++
IQS5XX_B000_Trackpad trackpad(readyPin, address);
// Parameters:
// - readyPin: Digital pin connected to RDY signal (required)
// - address: I2C address (default: 0x74)
```
```c++
bool begin(TwoWire &wire = Wire);           // Initialize trackpad
bool isConnected();                         // Check connection
uint16_t getProductNumber();                // Get product ID
uint16_t getVersionInfo();                  // Get firmware version
bool wakeupDevice();                        // Wake device from sleep
bool enableManualControl();                 // Enable manual control mode
bool isReadyForData();                      // Check if ready pin is LOW
```

### Touch Detection
```c++
bool readTouchData(TouchData &touchData);   // Read complete touch data
TouchState getTouchState();                 // Get current touch state
uint16_t getTouchX();                       // Get X coordinate
uint16_t getTouchY();                       // Get Y coordinate
uint8_t getTouchStrength();                 // Get touch strength
uint8_t getTouchArea();                     // Get touch area
```

### System Management
```c++
uint8_t getSystemFlags();                   // Read system status
bool needsReset();                          // Check if reset needed
bool softReset();                           // Perform soft reset
```

### Data Structures
```c++
enum TouchState {
  NO_TOUCH = 0,
  SINGLE_TOUCH = 1,
  MULTI_TOUCH = 2
};

struct TouchData {
  uint16_t x;              // X coordinate (device-dependent range)
  uint16_t y;              // Y coordinate (device-dependent range)  
  uint8_t touchStrength;   // Touch strength value
  uint8_t area;            // Touch area value
  TouchState state;        // Current touch state
};

// Note: Coordinate ranges depend on the specific IQS5XX device variant
// Common ranges: 0-1023 for some variants, 0-65535 for others
```

## Examples

The library includes example sketches:

- **BasicTouchDetectionESP32**: Complete example for ESP32 with touch detection and coordinate reading
- Demonstrates proper I2C setup, device initialization, and continuous touch monitoring

### ESP32 Example Wiring
```c++
#define SDA_PIN 41        // I2C Data pin
#define SCL_PIN 42        // I2C Clock pin  
#define IQS550_RDY_PIN 39 // Ready signal pin
```

## Important Notes

### Constructor Changes
⚠️ **Breaking Change**: The constructor now requires a `readyPin` parameter:
```c++
// Correct usage
IQS5XX_B000_Trackpad trackpad(RDY_PIN);

// With custom I2C address  
IQS5XX_B000_Trackpad trackpad(RDY_PIN, 0x74);
```

### Manual Control Mode
The library supports manual control mode which provides:
- Continuous data updates at ~10fps
- Higher power consumption but faster response
- Enable with `trackpad.enableManualControl()` after initialization

### Device Compatibility  
Supports IQS5XX family devices with product IDs:
- 40 (IQS550)
- 52 (IQS525) 
- 58 (IQS572)

### Sleep/Wake Behavior
- Device may initially respond with NACK (address not acknowledged) when sleeping
- Library automatically handles wakeup sequence with proper timing (150µs minimum)
- Manual control mode prevents automatic sleep for faster response

## Troubleshooting

### Common Issues

1. **"Trackpad initialization failed!" message**
   - **Missing RDY pin**: Ensure RDY pin is connected and specified in constructor
   - Check I2C wiring (SDA/SCL connections)
   - Verify power supply (3.3V recommended for ESP32)
   - Ensure pull-up resistors are present on I2C lines (4.7kΩ)
   - Try different I2C pins if using ESP32

2. **Device not detected during I2C scan**
   - Device may be sleeping (normal behavior)
   - Check for proper grounding
   - Verify I2C address (default: 0x74)
   - Test with I2C scanner that handles NACK responses

3. **Erratic or no touch readings**
   - Call `trackpad.enableManualControl()` after initialization
   - Check if device needs reset using `needsReset()`
   - Verify RDY pin is properly connected and functioning
   - Ensure stable power supply with adequate current capacity
   - Add decoupling capacitors (0.1µF, 10µF) near the sensor

4. **"Manual control mode failed" message**
   - Device may not be properly initialized
   - Try calling `wakeupDevice()` before enabling manual control
   - Check I2C communication is stable

### I2C Scanner
```c++
#include <Wire.h>

void setup() {
  Wire.begin();
  Serial.begin(9600);
  Serial.println("I2C Scanner for IQS5XX");
  Serial.println("Default address should be 0x74");
}

void loop() {
  byte error, address;
  int nDevices;
  
  Serial.println("Scanning...");
  nDevices = 0;
  
  for(address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    
    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address<16) Serial.print("0");
      Serial.print(address,HEX);
      if (address == 0x74) {
        Serial.print(" (IQS5XX default)");
      }
      Serial.println();
      nDevices++;
    }
    else if (error == 2) {
      // Device may be sleeping - this is normal for IQS5XX
      Serial.print("Device found but not responding at 0x");
      if (address<16) Serial.print("0");
      Serial.print(address,HEX);
      Serial.println(" (may be sleeping)");
    }
  }
  
  if (nDevices == 0) {
    Serial.println("No I2C devices found");
    Serial.println("Check wiring and ensure RDY pin is connected");
  }
  Serial.println();
  
  delay(5000);
}
```

## Version Information

**Current Version: 1.0.0**

### Changelog

#### Version 1.0.0
- ✅ Complete rewrite with proper IQS5XX register mapping
- ✅ **Breaking Change**: Constructor now requires `readyPin` parameter
- ✅ Added manual control mode support
- ✅ Implemented proper sleep/wake handling with NACK/ACK sequence
- ✅ Added device compatibility checking for IQS550/IQS525/IQS572
- ✅ Enhanced error handling and status checking
- ✅ Added comprehensive ESP32 example
- ✅ Improved I2C communication with 16-bit register addressing
- ✅ Added `wakeupDevice()`, `enableManualControl()`, `isReadyForData()` methods

### API Status

| Function | Status | Notes |
|----------|--------|-------|
| `begin()` | ✅ Complete | Auto-detects device, handles wakeup |
| `readTouchData()` | ✅ Complete | Waits for RDY pin, reads all touch data |
| `getTouchX/Y()` | ✅ Complete | Individual coordinate access |
| `getTouchStrength/Area()` | ✅ Complete | Touch quality metrics |
| `getProductNumber()` | ✅ Complete | Device identification |
| `enableManualControl()` | ✅ Complete | 10fps continuous mode |
| `wakeupDevice()` | ✅ Complete | Proper 150µs timing |
| `softReset()` | ⚠️ Basic | Placeholder implementation |

## Contributing

Contributions are welcome! Please feel free to submit issues, feature requests, or pull requests.

### Development Status
- Core touch detection: **Complete** ✅
- Multi-touch support: **Planned** 🔄
- Gesture recognition: **Planned** 🔄
- Advanced configuration: **Planned** 🔄

## License

This project is licensed under the GNU General Public License v3.0 - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- Azoteq for the IQS5XX-B000 trackpad sensor and documentation
- Arduino community for the development environment and libraries
