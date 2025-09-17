# IQS5XX-B000 Trackpad Library

An Arduino library for interfacing with the IQS5XX-B000 capacitive trackpad sensor from Azoteq. This library provides easy-to-use functions for touch detection, coordinate tracking, and gesture recognition over I2C communication.

## Features

- ✅ I2C communication with IQS5XX-B000 trackpad
- ✅ Touch detection and coordinate tracking
- ✅ Touch strength and area measurement
- ✅ System status monitoring and error handling
- ✅ Device initialization and reset functionality
- ✅ Compatible with Arduino, ESP32, and other Arduino-compatible boards
- ✅ Multiple example sketches included

## Hardware Requirements

- Arduino-compatible board (Arduino Uno, ESP32, etc.)
- IQS5XX-B000 trackpad sensor
- I2C connection (SDA/SCL pins)
- Pull-up resistors for I2C lines (usually 4.7kΩ)

## Wiring

| IQS5XX-B000 Pin | Arduino Uno | ESP32 | Description |
|-----------------|-------------|-------|-------------|
| VCC | 3.3V or 5V | 3.3V | Power supply |
| GND | GND | GND | Ground |
| SDA | A4 | GPIO21 | I2C Data |
| SCL | A5 | GPIO22 | I2C Clock |
| RDY | Digital Pin (optional) | GPIO (optional) | Ready signal |

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

```cpp
#include <Wire.h>
#include <IQS5XX_B000_Trackpad.h>

IQS5XX_B000_Trackpad trackpad;

void setup() {
  Serial.begin(9600);
  
  if (trackpad.begin()) {
    Serial.println("Trackpad initialized!");
  } else {
    Serial.println("Trackpad initialization failed!");
  }
}

void loop() {
  TouchData touchData;
  
  if (trackpad.readTouchData(touchData)) {
    if (touchData.state == SINGLE_TOUCH) {
      Serial.print("Touch at X: ");
      Serial.print(touchData.x);
      Serial.print(", Y: ");
      Serial.println(touchData.y);
    }
  }
  
  delay(50);
}
```

## API Reference

### Constructor
```cpp
IQS5XX_B000_Trackpad trackpad(address);
```
- `address`: I2C address (default: 0x74)

### Basic Functions
```cpp
bool begin(TwoWire &wire = Wire);           // Initialize trackpad
bool isConnected();                         // Check connection
uint16_t getProductNumber();                // Get product ID
uint16_t getVersionInfo();                  // Get firmware version
```

### Touch Detection
```cpp
bool readTouchData(TouchData &touchData);   // Read complete touch data
TouchState getTouchState();                 // Get current touch state
uint16_t getTouchX();                       // Get X coordinate
uint16_t getTouchY();                       // Get Y coordinate
uint8_t getTouchStrength();                 // Get touch strength
uint8_t getTouchArea();                     // Get touch area
```

### System Management
```cpp
uint8_t getSystemFlags();                   // Read system status
bool needsReset();                          // Check if reset needed
bool softReset();                           // Perform soft reset
```

### Data Structures
```cpp
enum TouchState {
  NO_TOUCH = 0,
  SINGLE_TOUCH = 1,
  MULTI_TOUCH = 2
};

struct TouchData {
  uint16_t x;              // X coordinate (0-65535)
  uint16_t y;              // Y coordinate (0-65535)
  uint8_t touchStrength;   // Touch strength value
  uint8_t area;            // Touch area value
  TouchState state;        // Current touch state
};
```

## Examples

The library includes several example sketches:

- **BasicTouchDetection**: Simple touch detection and coordinate reading
- **AdvancedTouchMonitor**: Advanced monitoring with gesture detection

## Troubleshooting

### Common Issues

1. **Trackpad not detected**
   - Check I2C wiring (SDA/SCL connections)
   - Verify power supply (3.3V or 5V)
   - Ensure pull-up resistors are present on I2C lines
   - Try scanning for I2C devices to confirm address

2. **Erratic readings**
   - Check for proper grounding
   - Add decoupling capacitors near the sensor
   - Ensure stable power supply

3. **No touch response**
   - Verify the trackpad is properly calibrated
   - Check if device needs reset using `needsReset()`
   - Try performing a soft reset with `softReset()`

### I2C Scanner
Use this code to scan for I2C devices:

```cpp
#include <Wire.h>

void setup() {
  Wire.begin();
  Serial.begin(9600);
  Serial.println("I2C Scanner");
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
      Serial.println(address,HEX);
      nDevices++;
    }
  }
  
  if (nDevices == 0) {
    Serial.println("No I2C devices found\n");
  } else {
    Serial.println("done\n");
  }
  
  delay(5000);
}
```

## Contributing

Contributions are welcome! Please feel free to submit issues, feature requests, or pull requests.

## License

This project is licensed under the GNU General Public License v3.0 - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- Azoteq for the IQS5XX-B000 trackpad sensor
- Arduino community for the development environment and libraries
