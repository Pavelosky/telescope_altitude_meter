/*
 * MPU6050 sensor implementation for Telescope Altimeter
 */

#include "sensor.h"
#include "config.h"
#include <Arduino.h>

TelescopeSensor::TelescopeSensor()
  : last_ax(0.0), last_ay(0.0), last_az(0.0) {
}

bool TelescopeSensor::begin() {
  mpu.initialize();

  if (!mpu.testConnection()) {
    return false;
  }

  // Configure MPU6050
  mpu.setFullScaleAccelRange(MPU6050_ACCEL_FS_2);
  mpu.setFullScaleGyroRange(MPU6050_GYRO_FS_250);
  mpu.setDLPFMode(MPU6050_DLPF_BW_20);

  Serial.println("MPU6050 initialized successfully");
  return true;
}

void TelescopeSensor::readGravity(float& ax, float& ay, float& az) {
  int16_t raw_ax, raw_ay, raw_az;
  int16_t gx, gy, gz;

  mpu.getMotion6(&raw_ax, &raw_ay, &raw_az, &gx, &gy, &gz);

  // Convert to g (MPU6050 returns raw values)
  // For ±2g range: sensitivity = 16384 LSB/g
  ax = raw_ax / 16384.0;
  ay = raw_ay / 16384.0;
  az = raw_az / 16384.0;

  // Store for later retrieval
  last_ax = ax;
  last_ay = ay;
  last_az = az;
}

void TelescopeSensor::getLastReading(float& ax, float& ay, float& az) {
  ax = last_ax;
  ay = last_ay;
  az = last_az;
}

void TelescopeSensor::readAveragedGravity(float& ax, float& ay, float& az, int numSamples) {
  float sum_ax = 0.0;
  float sum_ay = 0.0;
  float sum_az = 0.0;

  for (int i = 0; i < numSamples; i++) {
    float sample_ax, sample_ay, sample_az;
    readGravity(sample_ax, sample_ay, sample_az);

    sum_ax += sample_ax;
    sum_ay += sample_ay;
    sum_az += sample_az;

    delay(20);
  }

  ax = sum_ax / numSamples;
  ay = sum_ay / numSamples;
  az = sum_az / numSamples;
}

float TelescopeSensor::calculateRawAngle(float refGravityX, float refGravityY, float refGravityZ) {
  // Read current gravity
  float ax_g, ay_g, az_g;
  readGravity(ax_g, ay_g, az_g);

  // Calculate altitude angle using dot product with reference "level" gravity vector
  // This works regardless of sensor rotation around the telescope tube!

  // Current gravity magnitude
  float gravityMag = sqrt(ax_g*ax_g + ay_g*ay_g + az_g*az_g);

  // Reference gravity magnitude
  float refGravityMag = sqrt(refGravityX*refGravityX + refGravityY*refGravityY + refGravityZ*refGravityZ);

  if (gravityMag > 0.1 && refGravityMag > 0.1) {
    // Normalize both vectors
    float norm_ax = ax_g / gravityMag;
    float norm_ay = ay_g / gravityMag;
    float norm_az = az_g / gravityMag;

    float norm_ref_x = refGravityX / refGravityMag;
    float norm_ref_y = refGravityY / refGravityMag;
    float norm_ref_z = refGravityZ / refGravityMag;

    // Dot product gives us cos(angle) between the two gravity vectors
    float dotProduct = norm_ax * norm_ref_x + norm_ay * norm_ref_y + norm_az * norm_ref_z;

    // Clamp to [-1, 1] to avoid NaN from acos due to floating point errors
    dotProduct = constrain(dotProduct, -1.0, 1.0);

    // Angle between current and reference gravity vectors
    float angleBetweenVectors = acos(dotProduct) * (180.0 / PI);

    // This angle represents rotation of the telescope
    // When telescope points up, gravity vector rotates "backward" relative to sensor
    // We need to determine the sign (positive = up, negative = down)

    // Cross product to determine direction
    // cross = ref × current, the Z component tells us rotation direction
    float cross_z = (norm_ref_x * norm_ay - norm_ref_y * norm_ax);

    // If cross_z is positive, we're rotating upward; negative = downward
    if (cross_z < 0) {
      return angleBetweenVectors;
    } else {
      return -angleBetweenVectors;
    }
  } else {
    return 0.0;  // Fallback if gravity reading is invalid
  }
}
