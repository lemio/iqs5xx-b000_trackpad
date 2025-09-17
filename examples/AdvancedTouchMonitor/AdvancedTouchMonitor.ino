/**
 * @file AdvancedTouchMonitor.ino
 * @brief Advanced example for IQS5XX-B000 trackpad with gesture detection
 * @version 1.0.0
 * @author lemio
 * 
 * This example demonstrates advanced touch monitoring including:
 * - Touch state monitoring
 * - Simple gesture detection (tap, hold)
 * - System status monitoring
 * - Error handling
 * 
 * Hardware Connections:
 * - VCC: 3.3V or 5V
 * - GND: Ground
 * - SDA: Pin A4 (Uno) or Pin 20 (Mega) or appropriate SDA pin
 * - SCL: Pin A5 (Uno) or Pin 21 (Mega) or appropriate SCL pin
 * 
 * @copyright This project is licensed under the GNU General Public License v3.0
 */

#include <Wire.h>
#include <IQS5XX_B000_Trackpad.h>

// Create trackpad instance
IQS5XX_B000_Trackpad trackpad;

// Variables for gesture detection
unsigned long touchStartTime = 0;
bool wasTouching = false;
uint16_t lastX = 0, lastY = 0;
const unsigned long TAP_THRESHOLD = 200;  // ms
const unsigned long HOLD_THRESHOLD = 1000; // ms
const uint16_t MOVEMENT_THRESHOLD = 50;     // coordinate units

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ;
  }
  
  Serial.println("IQS5XX-B000 Advanced Touch Monitor");
  Serial.println("==================================");
  
  if (trackpad.begin()) {
    Serial.println("Trackpad initialized successfully!");
    
    // Display device information
    displayDeviceInfo();
    
    Serial.println("Advanced touch monitoring started...");
    Serial.println("Touch patterns will be detected and reported.");
    Serial.println();
  } else {
    Serial.println("Failed to initialize trackpad!");
    handleInitError();
  }
}

void loop() {
  // Check system status periodically
  static unsigned long lastStatusCheck = 0;
  if (millis() - lastStatusCheck > 5000) { // Every 5 seconds
    checkSystemStatus();
    lastStatusCheck = millis();
  }
  
  // Read and process touch data
  processTouchData();
  
  delay(10); // Small delay for stability
}

void displayDeviceInfo() {
  uint16_t productNumber = trackpad.getProductNumber();
  uint16_t versionInfo = trackpad.getVersionInfo();
  
  Serial.print("Product Number: 0x");
  Serial.println(productNumber, HEX);
  Serial.print("Version Info: 0x");
  Serial.println(versionInfo, HEX);
  
  if (productNumber == 0) {
    Serial.println("Warning: Could not read product number. Check connections.");
  }
  Serial.println();
}

void handleInitError() {
  Serial.println("Troubleshooting tips:");
  Serial.println("1. Check I2C wiring (SDA/SCL pins)");
  Serial.println("2. Verify power connections (VCC/GND)");
  Serial.println("3. Confirm I2C address (default: 0x74)");
  Serial.println("4. Check for proper pull-up resistors on I2C lines");
  
  while (1) {
    // Blink LED or provide other indication
    delay(1000);
  }
}

void checkSystemStatus() {
  uint8_t systemFlags = trackpad.getSystemFlags();
  
  if (systemFlags & 0x80) { // Check for any error flags (simplified)
    Serial.print("System Status - Flags: 0x");
    Serial.println(systemFlags, HEX);
    
    if (trackpad.needsReset()) {
      Serial.println("Device needs reset, attempting soft reset...");
      if (trackpad.softReset()) {
        Serial.println("Reset successful.");
      } else {
        Serial.println("Reset failed!");
      }
    }
  }
}

void processTouchData() {
  TouchData touchData;
  bool currentlyTouching = false;
  
  if (trackpad.readTouchData(touchData)) {
    currentlyTouching = (touchData.state == SINGLE_TOUCH);
    
    if (currentlyTouching) {
      handleTouchActive(touchData);
    } else if (wasTouching) {
      handleTouchEnd();
    }
    
    wasTouching = currentlyTouching;
  } else {
    // Handle read error
    static unsigned long lastErrorReport = 0;
    if (millis() - lastErrorReport > 1000) { // Report errors max once per second
      Serial.println("Error: Failed to read touch data");
      lastErrorReport = millis();
    }
  }
}

void handleTouchActive(TouchData &touchData) {
  if (!wasTouching) {
    // Touch just started
    touchStartTime = millis();
    lastX = touchData.x;
    lastY = touchData.y;
    Serial.println("Touch started");
  } else {
    // Touch continuing - check for movement
    uint16_t deltaX = abs(touchData.x - lastX);
    uint16_t deltaY = abs(touchData.y - lastY);
    
    if (deltaX > MOVEMENT_THRESHOLD || deltaY > MOVEMENT_THRESHOLD) {
      Serial.print("Touch movement - X: ");
      Serial.print(touchData.x);
      Serial.print(", Y: ");
      Serial.print(touchData.y);
      Serial.print(", Strength: ");
      Serial.print(touchData.touchStrength);
      Serial.print(", Area: ");
      Serial.println(touchData.area);
      
      lastX = touchData.x;
      lastY = touchData.y;
    }
  }
}

void handleTouchEnd() {
  unsigned long touchDuration = millis() - touchStartTime;
  
  Serial.print("Touch ended - Duration: ");
  Serial.print(touchDuration);
  Serial.print("ms - ");
  
  if (touchDuration < TAP_THRESHOLD) {
    Serial.println("TAP detected!");
  } else if (touchDuration > HOLD_THRESHOLD) {
    Serial.println("HOLD detected!");
  } else {
    Serial.println("TOUCH detected");
  }
  
  Serial.print("Final position - X: ");
  Serial.print(lastX);
  Serial.print(", Y: ");
  Serial.println(lastY);
  Serial.println();
}