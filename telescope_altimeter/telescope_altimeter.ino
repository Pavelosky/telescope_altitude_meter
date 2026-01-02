/*
 * Digital Altimeter for Skywatcher Classic 200 Telescope v2.1
 *
 * Hardware:
 * - NodeMCU ESP8266
 * - MPU6050 (I2C address 0x68)
 * - 128x64 OLED SSD1306 (I2C address 0x3C)
 *   Display zones: Top 10 rows YELLOW, rows 11-64 BLUE
 * - Push button on D3 (GPIO0) with internal pullup
 *
 * Features:
 * - Real-time altitude display
 * - Rotation-invariant angle calculation (sensor can be mounted at any angle around tube)
 * - Two-point calibration using mechanical stops
 * - Zero-point calibration with bubble level (one-time only)
 * - Two-stop session sync (no bubble level needed in field)
 * - EEPROM storage for calibration data
 * - Expandable for future azimuth integration
 */

#include <Wire.h>
#include "config.h"
#include "sensor.h"
#include "calibration.h"
#include "display.h"
#include "button.h"

// ==================== GLOBAL OBJECTS ====================

TelescopeSensor sensor;
CalibrationManager calibration(sensor);
TelescopeDisplay displayManager;
ButtonHandler button(BUTTON_PIN);

// ==================== STATE VARIABLES ====================

// Current readings
float currentAltitude = 0.0;
float filteredAltitude = 0.0;
float rawAngle = 0.0;

// UI state
UIMode currentMode = MODE_NORMAL;

// ==================== SETUP ====================

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);

  Serial.println("\n=== Telescope Altimeter " VERSION_STRING " ===");

  // Initialize I2C
  Wire.begin(I2C_SDA, I2C_SCL);
  Wire.setClock(400000); // 400kHz fast mode

  // Initialize button
  button.begin();

  // Initialize calibration manager (EEPROM)
  calibration.begin();

  // Initialize MPU6050
  if (!sensor.begin()) {
    Serial.println("ERROR: MPU6050 initialization failed!");
    displayManager.begin();
    displayManager.showError("MPU6050 FAIL");
    while (1) delay(100);
  }

  // Initialize display
  if (!displayManager.begin()) {
    Serial.println("ERROR: Display initialization failed!");
    while (1) delay(100);
  }

  // Load calibration from EEPROM
  calibration.loadFromEEPROM();

  // Show startup screen
  displayManager.showStartup();
  delay(2000);

  Serial.println("Setup complete!");
  Serial.println("Ready to measure altitude.");
  if (calibration.isCalibrated()) {
    Serial.println("Calibration loaded from EEPROM.");
  } else {
    Serial.println("WARNING: Not calibrated! Long press button to calibrate.");
  }
}

// ==================== MAIN LOOP ====================

void loop() {
  // Read sensor
  readSensor();

  // Handle button input
  handleButton();

  // Update display based on current mode
  displayManager.update(currentMode, filteredAltitude, rawAngle, calibration.isCalibrated());

  // Small delay for stability
  delay(100); // 10 Hz refresh rate
}

// ==================== SENSOR READING ====================

void readSensor() {
  // Get reference gravity vector from calibration
  float refGravityX, refGravityY, refGravityZ;
  calibration.getReferenceGravity(refGravityX, refGravityY, refGravityZ);

  // Calculate raw angle
  rawAngle = sensor.calculateRawAngle(refGravityX, refGravityY, refGravityZ);

  // Apply calibration if available
  if (calibration.isCalibrated()) {
    currentAltitude = calibration.applyCalibratedOffset(rawAngle);
  } else {
    // No calibration, just show raw angle
    currentAltitude = rawAngle;
  }

  // Apply exponential moving average filter
  if (filteredAltitude == 0.0) {
    // Initialize filter on first reading
    filteredAltitude = currentAltitude;
  } else {
    filteredAltitude = ALPHA * currentAltitude + (1.0 - ALPHA) * filteredAltitude;
  }
}

// ==================== BUTTON HANDLING ====================

void handleButton() {
  button.update();

  if (button.wasShortPressed()) {
    onShortPress();
  }

  if (button.wasLongPressed()) {
    onLongPress();
  }
}

void onShortPress() {
  Serial.println("Button: Short press");

  switch (currentMode) {
    case MODE_NORMAL:
      if (calibration.isCalibrated()) {
        // Start two-point session sync
        currentMode = MODE_SESSION_SYNC_A;
      } else {
        displayManager.showMessage("ERROR", "Not calibrated!");
        delay(1500);
      }
      break;

    case MODE_SESSION_SYNC_A:
      // Sync at Stop A
      displayManager.showMessage("SYNCING...", "Please wait");
      calibration.syncAtStopA();
      displayManager.showMessage("STOP A", "Synced!");
      delay(1000);
      currentMode = MODE_SESSION_SYNC_B;
      break;

    case MODE_SESSION_SYNC_B:
      // Sync at Stop B
      displayManager.showMessage("SYNCING...", "Please wait");
      calibration.syncAtStopB();
      displayManager.showMessage("SYNCED!", "Ready to observe");
      delay(1500);
      currentMode = MODE_NORMAL;
      break;

    case MODE_CALIBRATION_MENU:
      // Start zero calibration
      currentMode = MODE_ZERO_CALIBRATION;
      break;

    case MODE_ZERO_CALIBRATION:
      displayManager.showMessage("MEASURING...", "Please wait");
      calibration.calibrateZero();
      displayManager.showMessage("ZERO SET", "");
      delay(1500);
      currentMode = MODE_STOP_A_CALIBRATION;
      break;

    case MODE_STOP_A_CALIBRATION:
      displayManager.showMessage("MEASURING...", "Please wait");
      calibration.calibrateStopA();
      displayManager.showMessage("STOP A SET", "");
      delay(1500);
      currentMode = MODE_STOP_B_CALIBRATION;
      break;

    case MODE_STOP_B_CALIBRATION:
      displayManager.showMessage("MEASURING...", "Please wait");
      calibration.calibrateStopB();
      calibration.saveToEEPROM();
      displayManager.showMessage("CALIBRATED!", "Saved to memory");
      delay(2000);
      currentMode = MODE_NORMAL;
      break;
  }
}

void onLongPress() {
  Serial.println("Button: Long press");

  if (currentMode == MODE_NORMAL) {
    // Enter calibration menu
    currentMode = MODE_CALIBRATION_MENU;
    Serial.println("Entering calibration mode");
  } else {
    // Cancel calibration/sync and return to normal
    currentMode = MODE_NORMAL;
    Serial.println("Operation cancelled");
    displayManager.showMessage("CANCELLED", "");
    delay(1000);
  }
}
