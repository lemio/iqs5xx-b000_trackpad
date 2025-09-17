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
 * - SDA: Pin 41 (ESP32)
 * - SCL: Pin 42 (ESP32)
 * - RDY: Optional ready pin (can be connected to any digital pin)
 * 
 * @copyright This project is licensed under the GNU General Public License v3.0
 */

#include <Arduino.h>
#include <Wire.h>
#include <IQS5XX_B000_Trackpad.h>



#define SDA_PIN 41        // I2C Data pin
#define SCL_PIN 42        // I2C Clock pin
#define IQS550_RDY_PIN 39 // Ready signal pin
#define IQS550_RST_PIN 40 // Reset pin

// Create trackpad instance with default I2C address
IQS5XX_B000_Trackpad trackpad(IQS550_RDY_PIN, IQS5XX_DEFAULT_ADDRESS);

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  while (!Serial) {
    ; // Wait for serial port to connect (needed for native USB)
  }
  
  Serial.println("IQS5XX-B000 Trackpad Basic Touch Detection");
  Serial.println("==========================================");
  Wire.begin(SDA_PIN, SCL_PIN);
  // Initialize the trackpad
  if (trackpad.begin(Wire)) {
    delay(10);
    // Make sure to enable manual control mode to get 10fps data 
    // updates and don't wait for wakeup (more power consumption)
    if (trackpad.enableManualControl()){
      Serial.println("Manual control mode enabled.");
    } else {
      Serial.println("Failed to enable manual control mode.");
    }
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
    Serial.println("Format: X: 0 - 1023, Y: 0 - 1023, Strength: xxx, Area: xxx");
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
    //if (touchData.state == SINGLE_TOUCH) {
      
      Serial.print(touchData.x);
      Serial.print(",");
      Serial.print(touchData.y);
      Serial.print(",");
      Serial.print(touchData.touchStrength);
      Serial.print(",");
      Serial.println(touchData.area);
    //}
  } else {
    Serial.println("Error reading touch data");
  }
  
  // Small delay to avoid overwhelming the serial output
  delay(50);
}