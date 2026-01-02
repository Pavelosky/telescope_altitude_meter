/*
 * Calibration system implementation for Telescope Altimeter
 */

#include "calibration.h"
#include "config.h"
#include <EEPROM.h>
#include <Arduino.h>

CalibrationManager::CalibrationManager(TelescopeSensor& sensorRef)
  : sensor(sensorRef),
    zeroOffset(0.0),
    stopA_raw(0.0),
    stopB_raw(0.0),
    calibrated(false),
    tubeAxis_x(0.0),
    tubeAxis_y(1.0),  // Default: assume Y-axis
    tubeAxis_z(0.0) {
}

void CalibrationManager::begin() {
  EEPROM.begin(EEPROM_SIZE);
}

void CalibrationManager::loadFromEEPROM() {
  // Read calibration flag
  byte flag;
  EEPROM.get(ADDR_CALIBRATED_FLAG, flag);

  if (flag == 0xAA) {
    // Valid calibration exists
    EEPROM.get(ADDR_ZERO_OFFSET, zeroOffset);
    EEPROM.get(ADDR_STOP_A_RAW, stopA_raw);
    EEPROM.get(ADDR_STOP_B_RAW, stopB_raw);
    EEPROM.get(ADDR_TUBE_AXIS_X, tubeAxis_x);
    EEPROM.get(ADDR_TUBE_AXIS_Y, tubeAxis_y);
    EEPROM.get(ADDR_TUBE_AXIS_Z, tubeAxis_z);
    calibrated = true;

    Serial.println("Calibration loaded:");
    Serial.print("  Zero offset: "); Serial.println(zeroOffset);
    Serial.print("  Stop A raw: "); Serial.println(stopA_raw);
    Serial.print("  Stop B raw: "); Serial.println(stopB_raw);
    Serial.print("  Tube axis: (");
    Serial.print(tubeAxis_x, 3); Serial.print(", ");
    Serial.print(tubeAxis_y, 3); Serial.print(", ");
    Serial.print(tubeAxis_z, 3); Serial.println(")");
  } else {
    Serial.println("No calibration found in EEPROM");
    calibrated = false;
  }
}

void CalibrationManager::saveToEEPROM() {
  EEPROM.put(ADDR_ZERO_OFFSET, zeroOffset);
  EEPROM.put(ADDR_STOP_A_RAW, stopA_raw);
  EEPROM.put(ADDR_STOP_B_RAW, stopB_raw);
  EEPROM.put(ADDR_TUBE_AXIS_X, tubeAxis_x);
  EEPROM.put(ADDR_TUBE_AXIS_Y, tubeAxis_y);
  EEPROM.put(ADDR_TUBE_AXIS_Z, tubeAxis_z);
  byte flag = 0xAA;
  EEPROM.put(ADDR_CALIBRATED_FLAG, flag);
  EEPROM.commit();

  calibrated = true;

  Serial.println("Calibration saved to EEPROM");
}

void CalibrationManager::calibrateZero() {
  // Read averaged gravity when telescope is level
  sensor.readAveragedGravity(tubeAxis_x, tubeAxis_y, tubeAxis_z, NUM_CALIBRATION_READINGS);

  // For now, zero offset is 0 (we'll refine this after Stop A and B calibration)
  zeroOffset = 0.0;

  Serial.print("Zero reference gravity: (");
  Serial.print(tubeAxis_x, 3); Serial.print(", ");
  Serial.print(tubeAxis_y, 3); Serial.print(", ");
  Serial.print(tubeAxis_z, 3); Serial.println(")");
}

void CalibrationManager::calibrateStopA() {
  float sum_angle = 0.0;

  for (int i = 0; i < NUM_CALIBRATION_READINGS; i++) {
    float ax, ay, az;
    sensor.readAveragedGravity(ax, ay, az, 1);  // Single reading
    float angle = calculateAngle(ax, ay, az);
    sum_angle += angle;
    delay(20);
  }

  stopA_raw = sum_angle / NUM_CALIBRATION_READINGS;
  Serial.print("Stop A calibrated: "); Serial.println(stopA_raw);
}

void CalibrationManager::calibrateStopB() {
  float sum_angle = 0.0;

  for (int i = 0; i < NUM_CALIBRATION_READINGS; i++) {
    float ax, ay, az;
    sensor.readAveragedGravity(ax, ay, az, 1);  // Single reading
    float angle = calculateAngle(ax, ay, az);
    sum_angle += angle;
    delay(20);
  }

  stopB_raw = sum_angle / NUM_CALIBRATION_READINGS;
  Serial.print("Stop B calibrated: "); Serial.println(stopB_raw);
}

void CalibrationManager::syncAtStopA() {
  float sum_angle = 0.0;

  for (int i = 0; i < NUM_CALIBRATION_READINGS; i++) {
    float ax, ay, az;
    sensor.readAveragedGravity(ax, ay, az, 1);  // Single reading
    float angle = calculateAngle(ax, ay, az);
    sum_angle += angle;
    delay(20);
  }

  stopA_raw = sum_angle / NUM_CALIBRATION_READINGS;
  Serial.print("Stop A synced: "); Serial.println(stopA_raw);
}

void CalibrationManager::syncAtStopB() {
  float sum_angle = 0.0;

  for (int i = 0; i < NUM_CALIBRATION_READINGS; i++) {
    float ax, ay, az;
    sensor.readAveragedGravity(ax, ay, az, 1);  // Single reading
    float angle = calculateAngle(ax, ay, az);
    sum_angle += angle;
    delay(20);
  }

  stopB_raw = sum_angle / NUM_CALIBRATION_READINGS;
  Serial.print("Stop B synced: "); Serial.println(stopB_raw);

  // Save updated calibration
  saveToEEPROM();
}

float CalibrationManager::calculateAngle(float ax, float ay, float az) {
  // Calculate angle using same method as sensor
  float gravityMag = sqrt(ax*ax + ay*ay + az*az);
  float refGravityMag = sqrt(tubeAxis_x*tubeAxis_x + tubeAxis_y*tubeAxis_y + tubeAxis_z*tubeAxis_z);

  if (gravityMag > 0.1 && refGravityMag > 0.1) {
    float norm_ax = ax / gravityMag;
    float norm_ay = ay / gravityMag;
    float norm_az = az / gravityMag;

    float norm_ref_x = tubeAxis_x / refGravityMag;
    float norm_ref_y = tubeAxis_y / refGravityMag;
    float norm_ref_z = tubeAxis_z / refGravityMag;

    float dotProduct = norm_ax * norm_ref_x + norm_ay * norm_ref_y + norm_az * norm_ref_z;
    dotProduct = constrain(dotProduct, -1.0, 1.0);

    float angleBetweenVectors = acos(dotProduct) * (180.0 / PI);

    float cross_z = (norm_ref_x * norm_ay - norm_ref_y * norm_ax);

    if (cross_z < 0) {
      return angleBetweenVectors;
    } else {
      return -angleBetweenVectors;
    }
  }

  return 0.0;
}

float CalibrationManager::applyCalibratedOffset(float rawAngle) const {
  if (!calibrated) {
    return rawAngle;
  }

  // Apply zero offset first
  float corrected = rawAngle - zeroOffset;

  // Apply two-point linear calibration
  float range = stopB_raw - stopA_raw;

  if (abs(range) > 0.1) {  // Avoid division by zero
    // Use the stops as reference points
    // The actual altitude difference between stops
    float stopDiff = (stopB_raw - zeroOffset) - (stopA_raw - zeroOffset);

    // Scale factor (should be close to 1.0 if sensor is well-aligned)
    float scale = stopDiff / range;

    return corrected;
  } else {
    // Fallback to zero-point calibration only
    return corrected;
  }
}
