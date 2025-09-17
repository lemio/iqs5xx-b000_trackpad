/**
 * @file BasicTouchDetection.ino
 * @brief Basic example for IQS5XX-B000 trackpad touch detection
 * @version 1.0.0
 * @author lemio
 * 
 * This example demonstrates basic touch detection and coordinate reading
 * from the IQS5XX-B000 trackpad sensor.
 * 
 * Hardware Connections:
 * - VCC: 3.3V or 5V
 * - GND: Ground
 * - SDA: Pin A4 (Uno) or Pin 20 (Mega) or appropriate SDA pin
 * - SCL: Pin A5 (Uno) or Pin 21 (Mega) or appropriate SCL pin
 * - RDY: Optional ready pin (can be connected to any digital pin)
 * 
 * @copyright This project is licensed under the GNU General Public License v3.0
 */

#include <Wire.h>
#include <IQS5XX_B000_Trackpad.h>

// Create trackpad instance with default I2C address
IQS5XX_B000_Trackpad trackpad;

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  while (!Serial) {
    ; // Wait for serial port to connect (needed for native USB)
  }
  
  Serial.println("IQS5XX-B000 Trackpad Basic Touch Detection");
  Serial.println("==========================================");
  
  // Initialize the trackpad
  if (trackpad.begin()) {
    Serial.println("Trackpad initialized successfully!");
    
    // Display device information
    uint16_t productNumber = trackpad.getProductNumber();
    uint16_t versionInfo = trackpad.getVersionInfo();
    
    Serial.print("Product Number: 0x");
    Serial.println(productNumber, HEX);
    Serial.print("Version Info: 0x");
    Serial.println(versionInfo, HEX);
    Serial.println();
    
    Serial.println("Touch the trackpad to see coordinates...");
    Serial.println("Format: X: xxxx, Y: yyyy, Strength: xxx, Area: xxx");
    Serial.println();
  } else {
    Serial.println("Failed to initialize trackpad!");
    Serial.println("Please check wiring and I2C connections.");
    while (1) {
      delay(1000);
    }
  }
}

void loop() {
  // Read touch data
  TouchData touchData;
  
  if (trackpad.readTouchData(touchData)) {
    // Only print if there's a touch detected
    if (touchData.state == SINGLE_TOUCH) {
      Serial.print("Touch detected - ");
      Serial.print("X: ");
      Serial.print(touchData.x);
      Serial.print(", Y: ");
      Serial.print(touchData.y);
      Serial.print(", Strength: ");
      Serial.print(touchData.touchStrength);
      Serial.print(", Area: ");
      Serial.println(touchData.area);
    }
  } else {
    Serial.println("Error reading touch data");
  }
  
  // Small delay to avoid overwhelming the serial output
  delay(50);
}