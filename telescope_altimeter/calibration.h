/*
 * Calibration system for Telescope Altimeter
 * Manages EEPROM storage and calibration procedures
 */

#ifndef CALIBRATION_H
#define CALIBRATION_H

#include "sensor.h"

class CalibrationManager {
public:
  CalibrationManager(TelescopeSensor& sensor);

  // Initialization
  void begin();

  // Load/Save calibration from/to EEPROM
  void loadFromEEPROM();
  void saveToEEPROM();

  // Calibration procedures
  void calibrateZero();
  void calibrateStopA();
  void calibrateStopB();

  // Session sync (field adjustments without bubble level)
  void syncAtStopA();
  void syncAtStopB();

  // Get calibration data
  bool isCalibrated() const { return calibrated; }
  float getZeroOffset() const { return zeroOffset; }
  float getStopARaw() const { return stopA_raw; }
  float getStopBRaw() const { return stopB_raw; }
  void getReferenceGravity(float& x, float& y, float& z) const {
    x = tubeAxis_x;
    y = tubeAxis_y;
    z = tubeAxis_z;
  }

  // Apply calibration to raw angle
  float applyCalibratedOffset(float rawAngle) const;

private:
  TelescopeSensor& sensor;

  // Calibration data
  float zeroOffset;
  float stopA_raw;
  float stopB_raw;
  bool calibrated;

  // Reference tube axis (gravity vector in sensor coordinates when telescope is level)
  float tubeAxis_x;
  float tubeAxis_y;
  float tubeAxis_z;

  // Helper to calculate angle using reference gravity
  float calculateAngle(float ax, float ay, float az);
};

#endif // CALIBRATION_H
