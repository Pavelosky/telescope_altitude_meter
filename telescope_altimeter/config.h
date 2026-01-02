/*
 * Configuration constants for Telescope Altimeter v2.1
 */

#ifndef CONFIG_H
#define CONFIG_H

// ==================== HARDWARE CONFIGURATION ====================

// Hardware pins
#define BUTTON_PIN D3      // GPIO0 - built-in button on NodeMCU
#define I2C_SDA D2         // GPIO4
#define I2C_SCL D1         // GPIO5

// Display zones (color-aware positioning)
#define YELLOW_ZONE_END 10   // Rows 0-10 are yellow (11 pixels high)
#define BLUE_ZONE_START 13   // Rows 13-64 are blue (row 11-12 is gap)

// ==================== ALGORITHM CONFIGURATION ====================

// Filter settings
#define ALPHA 0.2  // Exponential moving average factor (0-1, lower = smoother)

// Button settings
#define DEBOUNCE_DELAY 50
#define LONG_PRESS_TIME 2000

// Calibration settings
#define NUM_CALIBRATION_READINGS 50  // Number of samples to average during calibration

// ==================== EEPROM CONFIGURATION ====================

// EEPROM addresses
#define EEPROM_SIZE 512
#define ADDR_ZERO_OFFSET 0
#define ADDR_STOP_A_RAW 4
#define ADDR_STOP_B_RAW 8
#define ADDR_CALIBRATED_FLAG 12
#define ADDR_TUBE_AXIS_X 16
#define ADDR_TUBE_AXIS_Y 20
#define ADDR_TUBE_AXIS_Z 24

// ==================== VERSION ====================

#define VERSION_STRING "v2.1"

#endif // CONFIG_H
