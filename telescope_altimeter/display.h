/*
 * OLED Display interface for Telescope Altimeter
 * Handles all display operations and UI modes
 */

#ifndef DISPLAY_H
#define DISPLAY_H

#include <U8g2lib.h>

// UI Modes
enum UIMode {
  MODE_NORMAL,
  MODE_CALIBRATION_MENU,
  MODE_ZERO_CALIBRATION,
  MODE_STOP_A_CALIBRATION,
  MODE_STOP_B_CALIBRATION,
  MODE_SESSION_SYNC_A,
  MODE_SESSION_SYNC_B
};

class TelescopeDisplay {
public:
  TelescopeDisplay();

  // Initialization
  bool begin();

  // Update display based on current mode
  void update(UIMode mode, float filteredAltitude, float rawAngle, bool isCalibrated);

  // Show special screens
  void showStartup();
  void showError(const char* message);
  void showMessage(const char* title, const char* message);

private:
  U8G2_SSD1306_128X64_NONAME_F_HW_I2C display;

  // Mode-specific display functions
  void displayNormalMode(float filteredAltitude, float rawAngle, bool isCalibrated);
  void displayCalibrationMenu();
  void displayZeroCalibration(float rawAngle);
  void displayStopACalibration(float rawAngle);
  void displayStopBCalibration(float rawAngle);
  void displaySessionSyncA(float rawAngle);
  void displaySessionSyncB(float rawAngle);
};

#endif // DISPLAY_H
