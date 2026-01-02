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

  // BLUE ZONE (13-64): Main altitude reading
  display.setFont(u8g2_font_logisoso24_tn);

  char altStr[10];
  if (filteredAltitude >= 0) {
    snprintf(altStr, sizeof(altStr), "+%.1f", filteredAltitude);
  } else {
    snprintf(altStr, sizeof(altStr), "%.1f", filteredAltitude);
  }
  display.drawStr(0, 42, altStr);

  // Degree symbol
  display.setFont(u8g2_font_6x10_tf);
  display.drawStr(90, 42, "o");

  // Debug info at bottom
  display.setCursor(0, 62);
  display.print("Raw: ");
  display.print(rawAngle, 1);
  display.print("o");
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
  display.print("o");
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
  display.drawStr(0, 8, "By: pavelosky");

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
