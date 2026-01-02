/*
 * MPU6050 sensor interface for Telescope Altimeter
 * Handles sensor initialization and angle calculations
 */

#ifndef SENSOR_H
#define SENSOR_H

#include <MPU6050.h>

class TelescopeSensor {
public:
  TelescopeSensor();

  // Initialization
  bool begin();

  // Angle calculation
  float calculateRawAngle(float refGravityX, float refGravityY, float refGravityZ);

  // Get last raw sensor readings
  void getLastReading(float& ax, float& ay, float& az);

  // Read and average multiple samples (for calibration)
  void readAveragedGravity(float& ax, float& ay, float& az, int numSamples);

private:
  MPU6050 mpu;

  // Last sensor reading
  float last_ax;
  float last_ay;
  float last_az;

  // Read single sample
  void readGravity(float& ax, float& ay, float& az);
};

#endif // SENSOR_H
