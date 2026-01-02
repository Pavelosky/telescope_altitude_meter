# Calibration Guide (v2.1)

Complete procedures for calibrating your telescope digital altimeter.

## Overview

The system uses **rotation-invariant vector calibration** plus **two-point linear correction** to achieve accurate altitude angles. This accounts for:
- **Any sensor mounting orientation** (new in v2.1!)
- Sensor mounting angle errors
- Telescope mechanical geometry
- Temperature drift
- Accelerometer non-linearity

## Calibration Types

### 1. Full Calibration (One-Time Setup)
Complete calibration procedure establishing all reference points.
- **When:** First installation, after sensor remounting, or major drift
- **Duration:** ~5 minutes
- **Accuracy gained:** Maximum possible

### 2. Session Sync (Each Observing Session)
Quick synchronization using existing calibration data.
- **When:** Start of each observing session
- **Duration:** ~10 seconds
- **Accuracy gained:** Removes session-specific drift

---

## Full Calibration Procedure

### Prerequisites

✅ **You need:**
- Bubble level (accurate to ±0.5°)
- Access to both mechanical stops
- Stable telescope mounting
- System powered on

✅ **Environmental conditions:**
- Minimal wind (for level reference)
- Stable temperature (avoid direct sunlight)
- Let system warm up 2-3 minutes

### Step-by-Step Instructions

#### Step 0: Enter Calibration Mode

1. Power on the system
2. Wait for startup screen to complete
3. **Long press button** (hold for 2 seconds)
4. Display shows: "CALIBRATION"
5. Display prompts: "1. Level telescope / 2. Press button"

---

#### Step 1: Zero Point Calibration

**Purpose:** Establish horizontal reference (0° altitude)

**Procedure:**

1. **Level the telescope tube**
   - Place bubble level on top of tube
   - Adjust altitude until perfectly level
   - Double-check from both ends of tube
   - Remove bubble level

2. **Let system stabilize**
   - Wait 3-5 seconds
   - Ensure no movement
   - Display shows current raw angle

3. **Record zero reference**
   - **Press button** when ready
   - System takes 50 averaged readings (1 second)
   - Display shows: "ZERO SET"
   - Automatically advances to Step 2

**What it does (v2.1):**
```cpp
// Stores complete 3D gravity vector when telescope is level
referenceGravityX = average_of_50_readings_X();
referenceGravityY = average_of_50_readings_Y();
referenceGravityZ = average_of_50_readings_Z();
// This vector is used to calculate angle regardless of sensor rotation
zeroOffset = 0.0;  // Set to zero by definition
```

**Key improvement:** The system now captures the full 3D gravity direction, not just a single angle. This means the sensor can be mounted at any rotational position around the tube!

**Troubleshooting:**
- If readings are jumping: Check rigid mounting
- If bubble shows not level: Adjust and try again
- If unsure: Just press button, you can recalibrate later

---

#### Step 2: Stop A Calibration

**Purpose:** Establish first mechanical reference point

**Procedure:**

1. **Move to Stop A**
   - Slowly move telescope to lower mechanical stop
   - Should be around ~30° altitude
   - Let system settle for 2-3 seconds
   - Display shows current raw angle

2. **Ensure at mechanical stop**
   - Telescope should be firmly against stop
   - Not just close to it - actually touching
   - No pressure needed, just resting

3. **Record Stop A**
   - **Press button** when ready
   - System takes 50 averaged readings
   - Display shows: "STOP A SET"
   - Automatically advances to Step 3

**What it does:**
```cpp
stopA_raw = average_of_50_readings();
// Links this raw reading to STOP_A_ALTITUDE (30.0°)
```

**Note:** If your telescope's Stop A is NOT at 30°, you can:
- Measure actual angle with inclinometer
- Update `STOP_A_ALTITUDE` in code
- Re-upload and recalibrate

---

#### Step 3: Stop B Calibration

**Purpose:** Establish second mechanical reference point

**Procedure:**

1. **Move to Stop B**
   - Slowly move telescope to upper mechanical stop
   - Should be around ~105° altitude
   - Let system settle for 2-3 seconds
   - Display shows current raw angle

2. **Ensure at mechanical stop**
   - Telescope firmly against upper stop
   - Secure and stable position
   - No oscillation

3. **Record Stop B**
   - **Press button** when ready
   - System takes 50 averaged readings
   - Saves ALL calibration data to EEPROM
   - Display shows: "CALIBRATED! / Saved to memory"
   - Returns to normal mode after 2 seconds

**What it does:**
```cpp
stopB_raw = average_of_50_readings();
// Links this raw reading to STOP_B_ALTITUDE (105.0°)
// Saves: zeroOffset, stopA_raw, stopB_raw to EEPROM
// Sets calibration flag
```

---

#### Calibration Complete! ✅

The system is now fully calibrated and will:
- Display accurate altitude angles
- Remember calibration across power cycles
- Automatically apply two-point linear mapping
- Only need session sync from now on

**Calibration data stored (v2.1):**
```
Reference gravity vector: [X, Y, Z components when level]
Zero offset:  0.0
Stop A raw:   [raw angle at ~30°]  → 30.0°
Stop B raw:   [raw angle at ~105°] → 105.0°
Flag: 0xAA (calibrated)
```

Total EEPROM storage: 29 bytes

---

## Session Sync Procedure

**When to use:** At the start of each observing session

**Why needed:** Small drift from temperature, battery voltage, vibration

### Quick Sync Steps

1. **Move to either Stop A or Stop B**
   - Any stop works
   - Ensure firmly at mechanical stop

2. **Short press button**
   - System detects which stop automatically
   - Applies correction factor
   - Display shows: "SYNCED / At Stop A" (or B)
   - Ready to observe!

**What it does:**
```cpp
// Measures current raw angle
// Compares to stopA_raw and stopB_raw
// Determines which is closer
// Applies small offset correction
```

**Duration:** ~10 seconds total

**Frequency:** Once per session is usually enough

---

## Understanding the Calibration Math

### Two-Point Linear Calibration

The system maps raw sensor readings to real altitude angles:

```
Real Altitude = Scale × (Raw Reading - Offset)

Where:
  Scale = (StopB_altitude - StopA_altitude) / (StopB_raw - StopA_raw)
        = (105° - 30°) / (StopB_raw - StopA_raw)
        = 75° / (StopB_raw - StopA_raw)
```

**Example:**

If after calibration:
- `stopA_raw = 28.5°`
- `stopB_raw = 103.2°`

Then:
```
Scale = 75° / (103.2 - 28.5) = 75° / 74.7° = 1.004
```

For a raw reading of `50.0°`:
```
Altitude = 30° + ((50.0 - 28.5) × 1.004)
         = 30° + (21.5 × 1.004)
         = 30° + 21.6°
         = 51.6°
```

This compensates for:
- Non-linear accelerometer response
- Mounting angle errors
- Mechanical geometry differences

---

## Calibration Accuracy

### Expected Accuracy

| Condition | Accuracy | Notes |
|-----------|----------|-------|
| After full calibration | ±0.5° | Best case |
| After session sync | ±1.0° | Typical |
| Without sync | ±2-3° | Temperature drift |
| No calibration | ±5-10° | Mounting errors dominate |

### Factors Affecting Accuracy

**Good for accuracy:**
- ✅ Rigid sensor mounting
- ✅ Short sensor cable
- ✅ Stable temperature
- ✅ Accurate bubble level
- ✅ Precise mechanical stops
- ✅ Fresh calibration

**Bad for accuracy:**
- ❌ Flex in mounting bracket
- ❌ Temperature changes >10°C
- ❌ Worn mechanical stops
- ❌ Inaccurate bubble level
- ❌ Vibration during calibration
- ❌ Old calibration (>1 year)

---

## Troubleshooting Calibration Issues

### Problem: Readings Don't Match Expected Altitude

**Symptoms:**
- Display shows 45° but telescope is clearly at 60°
- Linear error (consistently off by same amount)

**Causes:**
1. Stop positions in code don't match reality
2. Calibration done at wrong stops
3. Sensor moved after calibration

**Solutions:**
1. Measure actual stop angles with inclinometer
2. Update `STOP_A_ALTITUDE` and `STOP_B_ALTITUDE` in code
3. Re-run full calibration
4. Verify sensor hasn't moved

---

### Problem: Readings Drift Over Time

**Symptoms:**
- Accurate after calibration
- Gradually becomes less accurate
- Off by 2-5° after 30 minutes

**Causes:**
1. Temperature change (most common)
2. Battery voltage dropping
3. Sensor mounting flex

**Solutions:**
1. Run session sync more frequently
2. Let system warm up before calibrating
3. Add insulation around sensor
4. Check mounting rigidity

---

### Problem: Non-Linear Errors

**Symptoms:**
- Accurate at stops
- Inaccurate in middle of range
- Error varies with position

**Causes:**
1. Mechanical geometry effects
2. Accelerometer non-linearity
3. Extreme temperature variations

**Solutions:**
1. v2.1's rotation-invariant calculation eliminates most mounting angle errors
2. Two-point calibration handles ±15° mechanical variations
3. If severe: check for mechanical flex in telescope mount
4. Consider adding third calibration point (advanced)

---

### Problem: Unstable During Calibration

**Symptoms:**
- Raw angle jumps around
- Can't get stable reading
- Calibration values vary wildly

**Causes:**
1. Loose sensor mounting (most common)
2. Vibration or wind
3. Cable pulling on sensor
4. I²C communication errors

**Solutions:**
1. Reinforce mounting bracket
2. Calibrate indoors or in calm conditions
3. Add cable strain relief
4. Increase filter alpha value
5. Check I²C wiring

---

## Advanced Calibration Options

### Changing Configuration

If you need to customize calibration parameters:

1. Open `config.h` in the telescope_altimeter folder
2. Find configuration constants:
```cpp
// Number of samples during calibration
#define NUM_CALIBRATION_READINGS 50

// Smoothing filter
#define ALPHA 0.2
```
3. Change values as needed
4. Re-upload code
5. Run full calibration again

**Note:** In v2.1, stop positions are determined during calibration, not hardcoded. The system simply records the raw angles when you calibrate at the stops.

---

### Three-Point Calibration (Future Enhancement)

For even better accuracy, you could add a third calibration point:

**Benefits:**
- Compensates for non-linear accelerometer response
- Better accuracy across full range
- Handles larger mounting errors

**Implementation:**
- Add horizontal position (0°)
- Add both mechanical stops
- Use quadratic interpolation instead of linear

**Code changes needed:**
- Add `ADDR_ZERO_RAW` EEPROM storage
- Modify calculation to use three points
- Update calibration procedure

---

## Calibration Best Practices

### Do's ✅

1. **Calibrate in similar conditions to usage**
   - Same temperature
   - Same battery state
   - Same location

2. **Wait for stabilization**
   - Let readings settle 3-5 seconds
   - No movement during calibration
   - Check display shows stable values

3. **Use accurate reference**
   - Good quality bubble level
   - Mechanical stops firmly engaged
   - Verify position visually

4. **Document your calibration**
   - Note date and conditions
   - Record stop positions
   - Keep track in observing log

5. **Regular maintenance**
   - Session sync each time
   - Full recalibration yearly
   - After any sensor movement

### Don'ts ❌

1. **Don't rush calibration**
   - Take your time at each step
   - Ensure stability before pressing button
   - Verify results make sense

2. **Don't calibrate in poor conditions**
   - High wind
   - Temperature extremes
   - Poor lighting (can't see level)
   - Unstable telescope mounting

3. **Don't skip steps**
   - All three steps are required
   - Session sync needs prior calibration
   - Zero point is critical

4. **Don't calibrate with loose mounting**
   - Check all screws tight
   - Verify no flex in bracket
   - Ensure sensor secure

5. **Don't ignore calibration errors**
   - If values seem wrong, recalibrate
   - Check Serial Monitor for debug info
   - Verify mechanical stops are correct

---

## Calibration Data Management

### Viewing Calibration Data

Open Serial Monitor (115200 baud) during startup to see:

```
Calibration loaded:
  Zero offset: 0.00
  Stop A raw: 28.67
  Stop B raw: 103.45
  Tube axis: (0.123, 0.985, -0.122)
```

The tube axis shows the reference gravity vector components stored during zero calibration.

### Clearing Calibration

To force recalibration:

**Option 1:** Long press button → Run through all steps

**Option 2:** Code modification:
```cpp
// In setup(), add this temporarily:
EEPROM.put(ADDR_CALIBRATED_FLAG, (byte)0x00);
EEPROM.commit();
// Upload, run once, then remove this code
```

### Backing Up Calibration

Note down values from Serial Monitor, then manually restore if needed:

```cpp
// Add to setup() temporarily:
zeroOffset = 2.34;    // Your backed up value
stopA_raw = 28.67;    // Your backed up value
stopB_raw = 103.45;   // Your backed up value
saveCalibration();    // Write to EEPROM
```

---

## Next Steps

After successful calibration:

1. ✅ Test at various positions between stops
2. ✅ Verify readings match visual estimates
3. ✅ Practice session sync procedure
4. ✅ Take system out for first observing session!
5. ✅ Keep calibration log in observing notes

**Enjoy accurate altitude readings!** 🔭✨
