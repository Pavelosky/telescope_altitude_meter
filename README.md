# Telescope Digital Altimeter v2.1

A digital altitude measurement system for the Skywatcher Classic 200 telescope using ESP8266 and MPU6050.

**New in v2.1:** Rotation-invariant angle calculation - sensor can now be mounted at any rotational angle around the telescope tube!

## Hardware Requirements

- **NodeMCU ESP8266**
- **MPU6050** 6-axis gyroscope/accelerometer
- **128×64 I²C OLED Display** (SSD1306)
- **Push button** (uses built-in button on NodeMCU D3/GPIO0)
- **Power supply** (battery pack, 5V USB)
- **Mounting hardware** (3D printed brackets recommended)

## Wiring Diagram

```
MPU6050          NodeMCU
-------          -------
VCC    -------> 3.3V
GND    -------> GND
SCL    -------> D1 (GPIO5)
SDA    -------> D2 (GPIO4)

OLED Display     NodeMCU
-------------    -------
VCC    -------> 3.3V
GND    -------> GND
SCL    -------> D1 (GPIO5)
SDA    -------> D2 (GPIO4)

Button: Built-in FLASH button on D3 (GPIO0)
```

**Note:** MPU6050 and OLED share the same I²C bus.

## Arduino IDE Setup

### 1. Install ESP8266 Board Support

1. Open Arduino IDE
2. Go to **File → Preferences**
3. Add to "Additional Board Manager URLs":
   ```
   http://arduino.esp8266.com/stable/package_esp8266com_index.json
   ```
4. Go to **Tools → Board → Boards Manager**
5. Search for "esp8266" and install **ESP8266 by ESP8266 Community**

### 2. Install Required Libraries

Go to **Sketch → Include Library → Manage Libraries** and install:

- **MPU6050** (by Electronic Cats)
- **U8g2** (by oliver)

### 3. Board Settings

Select the following in Arduino IDE:

- **Board:** "NodeMCU 1.0 (ESP-12E Module)"
- **Upload Speed:** 115200
- **CPU Frequency:** 80 MHz
- **Flash Size:** "4MB (FS:2MB OTA:~1019KB)"
- **Port:** Select your NodeMCU's COM port

## Installation

1. Download the entire `telescope_altimeter` folder (contains all required files)
2. Open `telescope_altimeter.ino` in Arduino IDE
3. Connect NodeMCU via USB
4. Click **Upload** (or Ctrl+U)
5. Open **Serial Monitor** (115200 baud) to see debug output

**Important:** All files must be in the same folder:
- `telescope_altimeter.ino` - Main program
- `config.h` - Configuration constants
- `sensor.h` / `sensor.cpp` - Sensor operations
- `calibration.h` / `calibration.cpp` - Calibration system
- `display.h` / `display.cpp` - Display management
- `button.h` / `button.cpp` - Button handling

## Physical Installation

### MPU6050 Mounting

1. **Position:** Mount on the side of the telescope tube (OTA) near the altitude bearing
2. **Orientation:** **Any rotational angle works!** (v2.1 feature) - sensor just needs to be firmly attached to the tube
3. **Method:** Use 3D printed bracket or strong adhesive
4. **Critical:** Ensure rigid mounting with no flex or movement

**Note:** The sensor can be mounted at any rotation around the tube axis. The calibration procedure will automatically detect and compensate for the mounting orientation.

### OLED Display Mounting

1. **Position:** Mount on rocker box or front panel, easily readable while observing
2. **Cable:** Keep cable run neat and allow for telescope movement
3. **Access:** Button should be easily reachable

### Cable Management

- Keep MPU6050 cable short (<30 cm) to reduce noise
- Route cables to allow full telescope movement without snagging
- Secure cables to prevent accidental disconnection

## Calibration Procedure

### Initial Setup (One-Time)

1. **Power on** the system
2. **Long press** the button (2 seconds) to enter calibration mode
3. Display shows: "CALIBRATION"

#### Step 1: Zero Point
- Level the telescope using a bubble level
- Press button when perfectly level
- System records zero reference

#### Step 2: Stop A
- Move telescope to mechanical Stop A (~30°)
- Press button
- System records Stop A position

#### Step 3: Stop B
- Move telescope to mechanical Stop B (~105°)
- Press button
- System saves calibration to EEPROM
- Returns to normal mode

**Calibration is now complete and saved permanently!**

### Session Startup (Quick Sync)

At the start of each observing session:

1. Move telescope to either Stop A or Stop B
2. **Short press** button
3. System automatically detects which stop and syncs
4. Ready to observe!

## Usage

### Normal Operation

- **Display shows:** Current altitude in degrees
- **Update rate:** 10 Hz (smooth, real-time)
- **Accuracy:** ±0.5° (after calibration)
- **Range:** Full telescope movement range

### Button Functions

| Action | Function |
|--------|----------|
| **Short press** | Sync at mechanical stop (during session) |
| **Long press (2s)** | Enter calibration mode |
| **Short press during calibration** | Advance to next calibration step |
| **Long press during calibration** | Cancel and return to normal mode |

### Display Modes

#### Normal Mode
```
ALTITUDE
+45.3°

Raw: 46.1°
```

#### Uncalibrated Warning
```
ALTITUDE      [UNCAL]
+45.3°

Raw: 46.1°
```

## Troubleshooting

### "MPU6050 FAIL" Error

**Cause:** MPU6050 not detected on I²C bus

**Solutions:**
- Check wiring (especially SDA/SCL)
- Verify 3.3V power connection
- Check I²C address (should be 0x68)
- Run I²C scanner to detect devices

### Display Not Working

**Cause:** OLED not detected or wrong address

**Solutions:**
- Check wiring
- Verify OLED address (0x3C or 0x3D)
- Try different OLED reset pin setting

### Altitude Readings Unstable

**Possible causes:**
- MPU6050 not rigidly mounted (most common)
- Cable too long or picking up interference
- Vibration from nearby equipment

**Solutions:**
- Reinforce mounting bracket
- Shorten sensor cable
- Add filtering (increase `ALPHA` value in code)

### Calibration Drift

**Cause:** Temperature changes, mounting flex

**Solution:**
- Re-run quick sync at session start
- If persistent, re-run full calibration

## Configuration Options

Edit constants in `config.h` to customize:

```cpp
// Smoothing filter (0.1 = very smooth, 0.5 = responsive)
#define ALPHA 0.2

// Long press duration (milliseconds)
#define LONG_PRESS_TIME 2000

// Number of samples during calibration
#define NUM_CALIBRATION_READINGS 50

// Hardware pins
#define BUTTON_PIN D3
#define I2C_SDA D2
#define I2C_SCL D1
```

All configuration is centralized in `config.h` for easy customization.

## Code Architecture

v2.1 features a modular, well-organized code structure:

### File Organization

- **config.h** - All configuration constants
- **sensor.h/cpp** - MPU6050 sensor interface
- **calibration.h/cpp** - Calibration system and EEPROM
- **display.h/cpp** - OLED display management
- **button.h/cpp** - Button handling and debouncing
- **telescope_altimeter.ino** - Main program coordinator

### Benefits

- **Separation of concerns** - Each module has a single responsibility
- **Easy maintenance** - Changes isolated to specific modules
- **Testable** - Individual components can be tested separately
- **Reusable** - Modules can be used in other projects
- **Readable** - Clean, well-documented code

## Future Expansion

The modular architecture makes additions straightforward:

### Planned Features

1. **Azimuth measurement** (magnetometer or encoder)
2. **Push-to functionality** (calculate slew to target)
3. **WiFi connectivity** (remote monitoring)
4. **Star alignment** (enhanced calibration)
5. **GoTo coordinate display**

### Adding New Features

The code structure supports easy expansion:

1. Create new module (e.g., `azimuth.h/cpp`)
2. Add configuration to `config.h`
3. Update display module for new UI
4. Integrate in main `.ino` file

## Serial Monitor Commands

Open Serial Monitor (115200 baud) to see:

- Startup diagnostics
- Calibration values
- Real-time altitude readings
- Button press detection
- Error messages

## Technical Details

### Altitude Calculation (v2.1)

The system uses rotation-invariant vector mathematics:

1. **Reference Capture:** During zero calibration, stores the gravity vector
2. **Angle Calculation:** Computes angle between current and reference gravity vectors
3. **Direction Detection:** Uses cross product to determine up/down direction

```cpp
// Dot product gives angle magnitude
angle = acos(current_gravity · reference_gravity) * 180/π

// Cross product determines sign (up vs down)
sign = cross_z(reference_gravity × current_gravity)
```

**Benefits:**
- Works with any sensor mounting orientation
- More accurate than single-axis calculation
- Robust against mounting variations

### Calibration Storage

Data stored in ESP8266 EEPROM:
- Zero offset (4 bytes)
- Stop A raw value (4 bytes)
- Stop B raw value (4 bytes)
- Reference gravity vector (12 bytes - 3 floats)
- Calibration valid flag (1 byte)

### Filter Performance

- **Response time:** ~1 second (with ALPHA=0.2)
- **Noise reduction:** ~80% (depending on ALPHA)
- **No lag** in steady-state tracking

## License

Open source - feel free to modify and improve!

## Version History

- **v2.1** - Major refactoring and enhancement
  - **Rotation-invariant angle calculation** - sensor can be mounted at any angle
  - Modular code architecture (6 separate modules)
  - Improved accuracy using vector mathematics
  - Better code organization and maintainability
  - Updated libraries (MPU6050, U8g2)

- **v2.0** - Enhanced calibration
  - Two-point calibration with mechanical stops
  - Session sync feature
  - Improved EEPROM storage

- **v1.0** - Initial release
  - Basic altitude measurement
  - Simple calibration
  - OLED display
  - EEPROM storage

## Author

Pawel Bialkowski

---

**Enjoy your enhanced telescope observing experience!** 🔭✨
