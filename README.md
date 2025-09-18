# IQS5XX-B000 Trackpad Library

https://github.com/user-attachments/assets/17cc72a4-d8b3-4ad5-9ea0-216d676c8c09



An Arduino library for interfacing with the IQS5XX-B000 capacitive trackpad sensor from Azoteq. This library provides easy-to-use functions for touch detection, coordinate tracking, and gesture recognition over I2C communication.

## Features

- ✅ I2C communication with IQS5XX-B000 trackpad
- ✅ Touch detection and coordinate tracking
- ✅ Touch strength and area measurement
- ✅ Gesture recognition (tap, swipe, two-finger tap, scroll, zoom, press & hold)
- ✅ Multi-touch detection and finger count
- ✅ System status monitoring and error handling
- ✅ Device initialization and reset functionality
- ✅ Manual control mode for continuous updates
- ✅ Device wakeup from sleep mode
- ✅ Compatible with Arduino, ESP32, and other Arduino-compatible boards
- ✅ Support for multiple IQS5XX variants (IQS550, IQS525, IQS572)
- ✅ Web-based real-time touch visualization plotter

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

#define SDA_PIN 41        // I2C Data pin (ESP32)
#define SCL_PIN 42        // I2C Clock pin (ESP32)
#define RDY_PIN 39        // Ready signal pin (required)

IQS5XX_B000_Trackpad trackpad(RDY_PIN);  // RDY pin is required

void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);  // ESP32 custom I2C pins
  
  if (trackpad.begin(Wire)) {
    Serial.println("Trackpad initialized!");
    
    // Enable manual control mode for continuous updates
    if (trackpad.enableManualControl()) {
      Serial.println("Manual control enabled!");
    }
    
    // Optional: Increase communication speed
    trackpad.increaseSpeed();
    
    // Display device info
    Serial.print("Product Number: 0x");
    Serial.println(trackpad.getProductNumber(), HEX);
    Serial.print("Version Info: 0x");
    Serial.println(trackpad.getVersionInfo(), HEX);
  } else {
    Serial.println("Trackpad initialization failed!");
  }
}

void loop() {
  TouchData touchData;
  
  if (trackpad.readTouchData(touchData)) {
    // Output in CSV format for web plotter compatibility
    Serial.print(touchData.x);
    Serial.print(",");
    Serial.print(touchData.y);
    Serial.print(",");
    Serial.print(touchData.touchStrength);
  }
  
  delay(50);  // ~20fps update rate
}
```

## Data Visualization & Plotter

The library includes multiple ways to visualize touch data:

### Arduino IDE Serial Plotter
Use the built-in Arduino IDE Serial Plotter (Tools → Serial Plotter) to see real-time graphs. The BasicTouchDetectionESP32 example outputs CSV format that works directly with the serial plotter.

### Web-Based Touch Plotter
For advanced visualization, use the included web-based plotter with these features:
- Real-time touch position visualization with trail effects
- Touch strength indication through point size
- Touch area measurement display
- Interactive touch history with fade effects
- Compatible with WebSerial for direct browser connection to your device

To use the web plotter:
```
npx http-server ./web
```

#### Version 1.0.2 (Current)
- ✅ **Breaking Change**: Constructor now requires `readyPin` parameter
- ✅ Complete rewrite with proper IQS5XX register mapping
- ✅ Added gesture recognition support (swipe, tap, two-finger tap, scroll, zoom, press & hold)
- ✅ Enhanced TouchData structure with gesture flags and finger count
- ✅ Added manual control mode support (10fps continuous updates)
- ✅ Implemented proper sleep/wake handling with NACK/ACK sequence
- ✅ Added device compatibility checking for IQS550/IQS525/IQS572
- ✅ Enhanced error handling and status checking
- ✅ Added comprehensive ESP32 example with CSV output format
- ✅ Improved I2C communication with 16-bit register addressing
- ✅ Added `wakeupDevice()`, `enableManualControl()`, `isReadyForData()` methods
- ✅ Added `increaseSpeed()` method for optimized communication timing
- ✅ Included web-based touch plotter with WebSerial integration
- ✅ Complete API implementation for single and multi-touch detection
- ✅ Full touch strength and area measurement support
- ✅ Multi-touch finger counting and state detection
```
python3 -m http.server --directory ./web 8080
```

Then open your browser and navigate to `http://localhost:8080/plotter.html` (or the port shown in your terminal). 

### Web Plotter Features:
- **Real-time Visualization**: Live touch point display with smooth trails
- **WebSerial Integration**: Connect directly to your Arduino's serial port from the browser
- **Touch Metrics**: Displays X/Y coordinates, touch strength, and area values
- **Interactive Controls**: Toggle trail display, adjust visualization settings
- **Responsive Design**: Works on desktop and mobile browsers

The web plotter expects CSV data in the format: `X,Y,Strength,Area` which is exactly what the BasicTouchDetectionESP32 example outputs.
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
bool increaseSpeed();                       // Optimize I2C timing for faster updates
bool isReadyForData();                      // Check if ready pin is LOW
```

### Touch Detection
```c++
```cpp
bool readTouchData(TouchData &touchData);   // Read complete touch data including gestures
TouchState getTouchState();                 // Get current touch state (NO_TOUCH/SINGLE_TOUCH/MULTI_TOUCH)
uint16_t getTouchX();                       // Get X coordinate
uint16_t getTouchY();                       // Get Y coordinate
uint8_t getTouchStrength();                 // Get touch strength
uint8_t getTouchArea();                     // Get touch area

// Note: readTouchData() populates the complete TouchData structure including:
// - Basic coordinates and measurements (x, y, touchStrength, area)
// - Multi-touch information (numFingers, state)
// - Gesture flags (singleTap, twoFingerTap, swipes, scroll, zoom, pressAndHold)
```
```

### System Management
```c++
uint8_t getSystemFlags();                   // Read system status
bool needsReset();                          // Check if reset needed
bool softReset();                           // Perform soft reset
bool increaseSpeed();                       // Increase communication speed
```

### Data Structures
```c++
```cpp
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
  uint8_t numFingers;      // Number of fingers detected
  TouchState state;        // Current touch state
  
  // Gesture flags (GESTURE_EVENTS_0)
  bool swipeY_minus;       // Swipe down gesture
  bool swipeY_plus;        // Swipe up gesture  
  bool swipeX_plus;        // Swipe right gesture
  bool swipeX_minus;       // Swipe left gesture
  bool pressAndHold;       // Press and hold gesture
  bool singleTap;          // Single tap gesture
  
  // Gesture flags (GESTURE_EVENTS_1)
  bool zoom;               // Zoom gesture (pinch/spread)
  bool scroll;             // Scroll gesture
  bool twoFingerTap;       // Two finger tap gesture
};

// Note: Coordinate ranges depend on the specific IQS5XX device variant
// Common ranges: 0-1023 for some variants, 0-65535 for others
```
```

## Examples

The library includes comprehensive example sketches in the `examples/` folder:

- **BasicTouchDetectionESP32**: Complete example for ESP32 with touch detection, coordinate reading, and gesture recognition
  - Demonstrates proper I2C setup with custom SDA/SCL pins (GPIO41/GPIO42)
  - Device initialization with error handling and device information display
  - Manual control mode enabling for 10fps continuous updates
  - Continuous touch monitoring with all touch parameters (X, Y, strength, area, gestures)
  - Serial output in CSV format compatible with the web plotter
  - Support for gesture detection (swipes, taps, scroll, zoom, press & hold)
  - Multi-touch finger counting and state detection

### ESP32 Example Wiring
```c++
#define SDA_PIN 41        // I2C Data pin
#define SCL_PIN 42        // I2C Clock pin  
#define IQS550_RDY_PIN 39 // Ready signal pin
#define IQS550_RST_PIN 40 // Reset pin (available but not used in basic example)
```

### Example Output Format

The BasicTouchDetectionESP32 example outputs data in CSV format for easy integration with the web plotter:

```
Format: X,Y,Strength,Area
Example: 512,300,25,15
```

The TouchData structure also provides access to additional gesture and multi-touch information:
- `touchData.numFingers` - Number of detected fingers
- `touchData.state` - TouchState enum (NO_TOUCH, SINGLE_TOUCH, MULTI_TOUCH)  
- `touchData.singleTap`, `touchData.twoFingerTap` - Tap gesture detection
- `touchData.swipeX_plus`, `touchData.swipeX_minus` - Horizontal swipe gestures
- `touchData.swipeY_plus`, `touchData.swipeY_minus` - Vertical swipe gestures
- `touchData.scroll`, `touchData.zoom` - Advanced multi-touch gestures
- `touchData.pressAndHold` - Press and hold gesture detection

This format allows direct visualization using the included web-based plotter at `web/plotter.html`.
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

### Speed Optimization
For even faster communication, use the `increaseSpeed()` function:
- Reduces I2C timeout to 5ms (RDY pin LOW duration)
- Increases report rate to 5ms intervals
- Call after enabling manual control mode for best performance

### Gesture Recognition
The library provides comprehensive gesture support through the TouchData structure:
- **Basic gestures**: Single tap, two-finger tap, press and hold
- **Swipe gestures**: Four-directional swipe detection (up, down, left, right)
- **Advanced gestures**: Scroll and zoom (pinch/spread) detection
- **Multi-touch support**: Finger counting and multi-touch state detection

### Device Compatibility  
Supports IQS5XX family devices with product IDs:
- 40 (IQS550)
- 52 (IQS525) 
- 58 (IQS572)
```c++
```cpp
### Development Status
- Core touch detection: **Complete** ✅
- Single touch tracking: **Complete** ✅  
- Multi-touch detection: **Complete** ✅
- Touch strength & area measurement: **Complete** ✅
- Device initialization & wakeup: **Complete** ✅
- Manual control mode: **Complete** ✅
- Speed optimization: **Complete** ✅
- Gesture recognition: **Complete** ✅
- Multi-touch finger counting: **Complete** ✅
- ESP32 example with web plotter: **Complete** ✅
- Advanced configuration options: **Planned** 🔄
- Custom gesture configuration: **Planned** 🔄
```
```

### API Status

| Function | Status | Notes |
|----------|--------|-------|
| `begin()` | ✅ Complete | Auto-detects device, handles wakeup |
| `isConnected()` | ✅ Complete | Checks device availability |
| `readTouchData()` | ✅ Complete | Waits for RDY pin, reads all touch data including gestures |
| `getTouchX/Y()` | ✅ Complete | Individual coordinate access |
| `getTouchStrength/Area()` | ✅ Complete | Touch quality metrics |
| `getTouchState()` | ✅ Complete | Returns TouchState enum (NO_TOUCH/SINGLE_TOUCH/MULTI_TOUCH) |
| `getProductNumber()` | ✅ Complete | Device identification |
| `getVersionInfo()` | ✅ Complete | Firmware version reading |
| `getSystemFlags()` | ✅ Complete | System status monitoring |
| `needsReset()` | ✅ Complete | Reset condition detection |
| `enableManualControl()` | ✅ Complete | 10fps continuous mode |
| `wakeupDevice()` | ✅ Complete | Proper 150µs timing with NACK/ACK |
| `isReadyForData()` | ✅ Complete | RDY pin status check |
| `increaseSpeed()` | ✅ Complete | Optimizes I2C timing for faster updates |
| `softReset()` | ✅ Complete | Software reset implementation |
| **Gesture Recognition** | ✅ Complete | All gestures supported via TouchData structure |
| **Multi-touch Detection** | ✅ Complete | Finger counting and multi-touch state |
| **Advanced Gestures** | ✅ Complete | Swipe, tap, scroll, zoom, press & hold |