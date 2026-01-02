/*
 * OLED Display implementation for Telescope Altimeter
 */

#include "display.h"
#include "config.h"
#include <Arduino.h>

TelescopeDisplay::TelescopeDisplay()
  : display(U8G2_R0, /* reset=*/ U8X8_PIN_NONE) {
}

bool TelescopeDisplay::begin() {
  display.begin();
  display.clearBuffer();
  display.sendBuffer();

  Serial.println("Display initialized successfully");
  return true;
}

void TelescopeDisplay::update(UIMode mode, float filteredAltitude, float rawAngle, bool isCalibrated) {
  display.clearBuffer();

  switch (mode) {
    case MODE_NORMAL:
      displayNormalMode(filteredAltitude, rawAngle, isCalibrated);
      break;

    case MODE_CALIBRATION_MENU:
      displayCalibrationMenu();
      break;

    case MODE_ZERO_CALIBRATION:
      displayZeroCalibration(rawAngle);
      break;

    case MODE_STOP_A_CALIBRATION:
      displayStopACalibration(rawAngle);
      break;

    case MODE_STOP_B_CALIBRATION:
      displayStopBCalibration(rawAngle);
      break;

    case MODE_SESSION_SYNC_A:
      displaySessionSyncA(rawAngle);
      break;

    case MODE_SESSION_SYNC_B:
      displaySessionSyncB(rawAngle);
      break;
  }

  display.sendBuffer();
}

void TelescopeDisplay::displayNormalMode(float filteredAltitude, float rawAngle, bool isCalibrated) {
  // YELLOW ZONE (0-10): Title only
  display.setFont(u8g2_font_6x10_tf);
  display.drawStr(0, 9, "ALTITUDE");

  if (!isCalibrated) {
    display.drawStr(70, 9, "[UNCAL]");
  }

  // BLUE ZONE (13-64): Main altitude reading in degrees and minutes
  // Convert decimal degrees to degrees and minutes
  int degrees = (int)filteredAltitude;
  float decimalPart = filteredAltitude - degrees;
  int minutes = (int)(abs(decimalPart) * 60.0);

  // Build degrees string (no sign)
  char altStr[16];
  snprintf(altStr, sizeof(altStr), "%d", abs(degrees));

  // Display degrees (larger font)
  display.setFont(u8g2_font_logisoso22_tn);
  int xPos = display.drawStr(1, 42, altStr);

  // Display degree symbol (medium font, superscript position)
  display.setFont(u8g2_font_9x18_tf);
  display.drawStr(xPos + 1, 28, "\xB0");  // \xB0 is degree symbol

  // Display minutes (medium font, right after degrees)
  display.setFont(u8g2_font_10x20_tf);
  char minStr[8];
  snprintf(minStr, sizeof(minStr), "%02d'", minutes);
  display.drawStr(xPos + 10, 42, minStr);

  // Debug info at bottom
  display.setFont(u8g2_font_6x10_tf);
  display.setCursor(0, 62);
  display.print("Raw: ");
  display.print(rawAngle, 1);
  display.print((char)176);  // Degree symbol
}

void TelescopeDisplay::displayCalibrationMenu() {
  // YELLOW ZONE (0-10): Title
  display.setFont(u8g2_font_7x13_tf);
  display.drawStr(0, 9, "CALIBRATION");

  // BLUE ZONE (13-64): Instructions
  display.setFont(u8g2_font_6x10_tf);
  display.drawStr(0, 25, "1. Level telescope");
  display.drawStr(0, 37, "2. Press button");
  display.drawStr(0, 52, "Hold to cancel");
}

void TelescopeDisplay::displayZeroCalibration(float rawAngle) {
  // YELLOW ZONE (0-10): Title
  display.setFont(u8g2_font_7x13_tf);
  display.drawStr(0, 9, "STEP 1: ZERO");

  // BLUE ZONE (13-64): Instructions
  display.setFont(u8g2_font_6x10_tf);
  display.drawStr(0, 25, "Level telescope");
  display.drawStr(0, 37, "with bubble level");
  display.drawStr(0, 52, "Press when ready");

  // Current angle at bottom
  display.setCursor(0, 62);
  display.print("Raw: ");
  display.print(rawAngle, 1);
  display.print("°");
}

void TelescopeDisplay::displayStopACalibration(float rawAngle) {
  // YELLOW ZONE (0-10): Title
  display.setFont(u8g2_font_7x13_tf);
  display.drawStr(0, 9, "STEP 2: STOP A");

  // BLUE ZONE (13-64): Instructions
  display.setFont(u8g2_font_6x10_tf);
  display.drawStr(0, 25, "Move to Stop A");
  display.drawStr(0, 37, "(low position)");
  display.drawStr(0, 52, "Press when ready");

  // Current angle at bottom
  display.setCursor(0, 62);
  display.print("Raw: ");
  display.print(rawAngle, 1);
  display.print("o");
}

void TelescopeDisplay::displayStopBCalibration(float rawAngle) {
  // YELLOW ZONE (0-10): Title
  display.setFont(u8g2_font_7x13_tf);
  display.drawStr(0, 9, "STEP 3: STOP B");

  // BLUE ZONE (13-64): Instructions
  display.setFont(u8g2_font_6x10_tf);
  display.drawStr(0, 25, "Move to Stop B");
  display.drawStr(0, 37, "(high position)");
  display.drawStr(0, 52, "Press when ready");

  // Current angle at bottom
  display.setCursor(0, 62);
  display.print("Raw: ");
  display.print(rawAngle, 1);
  display.print("o");
}

void TelescopeDisplay::displaySessionSyncA(float rawAngle) {
  // YELLOW ZONE (0-10): Title
  display.setFont(u8g2_font_7x13_tf);
  display.drawStr(0, 9, "SESSION SYNC");

  // BLUE ZONE (13-64): Instructions
  display.setFont(u8g2_font_6x10_tf);
  display.drawStr(0, 25, "Move to Stop A");
  display.drawStr(0, 37, "(low position)");
  display.drawStr(0, 52, "Press when ready");

  // Current angle at bottom
  display.setCursor(0, 62);
  display.print("Raw: ");
  display.print(rawAngle, 1);
  display.print("o");
}

void TelescopeDisplay::displaySessionSyncB(float rawAngle) {
  // YELLOW ZONE (0-10): Title
  display.setFont(u8g2_font_7x13_tf);
  display.drawStr(0, 9, "SESSION SYNC");

  // BLUE ZONE (13-64): Instructions
  display.setFont(u8g2_font_6x10_tf);
  display.drawStr(0, 25, "Move to Stop B");
  display.drawStr(0, 37, "(high position)");
  display.drawStr(0, 52, "Press when ready");

  // Current angle at bottom
  display.setCursor(0, 62);
  display.print("Raw: ");
  display.print(rawAngle, 1);
  display.print("o");
}

void TelescopeDisplay::showStartup() {
  display.clearBuffer();

  // YELLOW ZONE (0-10): Empty
  display.setFont(u8g2_font_7x13_tf);
  display.drawStr(0, 8, "Sky Watcher Classic 200");

  // BLUE ZONE (13-64): Main content
  display.setFont(u8g2_font_10x20_tf);
  display.drawStr(10, 32, "TELESCOPE");
  display.drawStr(10, 50, "ALTIMETER");
  display.setFont(u8g2_font_6x10_tf);
  display.drawStr(50, 60, VERSION_STRING);

  display.sendBuffer();
}

void TelescopeDisplay::showError(const char* message) {
  display.clearBuffer();

  // YELLOW ZONE (0-10): Empty

  // BLUE ZONE (13-64): Error message
  display.setFont(u8g2_font_10x20_tf);
  display.drawStr(10, 32, "ERROR:");
  display.setFont(u8g2_font_6x10_tf);
  display.drawStr(10, 50, message);

  display.sendBuffer();
}

void TelescopeDisplay::showMessage(const char* title, const char* message) {
  display.clearBuffer();

  // YELLOW ZONE (0-10): Empty

  // BLUE ZONE (13-64): Message
  display.setFont(u8g2_font_10x20_tf);
  display.drawStr(0, 30, title);
  display.setFont(u8g2_font_6x10_tf);
  display.drawStr(0, 50, message);

  display.sendBuffer();
}
