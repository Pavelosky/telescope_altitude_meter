# Wiring and Configuration Guide (v2.1)

## Complete Wiring Diagram

```
                           NodeMCU ESP8266
                          ┌─────────────────┐
                          │                 │
                          │   ┌─────────┐   │
                          │   │ ESP8266 │   │
                          │   │   SoC   │   │
                          │   └─────────┘   │
                          │                 │
    MPU6050               │                 │        OLED Display
    ┌────────┐           │                 │        ┌────────────┐
    │        │           │                 │        │            │
    │   SCL  ├───────────┤ D1 (GPIO5)      │        │   ┌────┐   │
    │   SDA  ├───────────┤ D2 (GPIO4)      ├────────┤   │OLED│   │
    │   VCC  ├───────────┤ 3.3V            │        │   └────┘   │
    │   GND  ├───────────┤ GND             ├────────┤            │
    │        │           │                 │        └────────────┘
    └────────┘           │                 │
                         │                 │
                         │ D3 (GPIO0) ──┐  │
                         │              │  │
                         │           [BUTTON]
                         │              │  │
                         │            GND  │
                         │                 │
                         │ VIN (5V) ◄──────┤ USB or Battery (5V)
                         │ GND ◄───────────┤ Battery GND
                         │                 │
                         └─────────────────┘
```

## Pin Connections Table

| Component | Pin | NodeMCU Pin | GPIO | Function |
|-----------|-----|-------------|------|----------|
| MPU6050   | VCC | 3.3V        | -    | Power (3.3V only!) |
| MPU6050   | GND | GND         | -    | Ground |
| MPU6050   | SCL | D1          | GPIO5 | I²C Clock |
| MPU6050   | SDA | D2          | GPIO4 | I²C Data |
| OLED      | VCC | 3.3V        | -    | Power (3.3V only!) |
| OLED      | GND | GND         | -    | Ground |
| OLED      | SCL | D1          | GPIO5 | I²C Clock (shared) |
| OLED      | SDA | D2          | GPIO4 | I²C Data (shared) |
| Button    | -   | D3          | GPIO0 | Built-in FLASH button |
| Power     | -   | VIN         | -    | 5V input from USB/battery |

## I²C Device Addresses

- **MPU6050:** 0x68 (default)
- **OLED Display:** 0x3C (most common) or 0x3D

**Note:** The U8g2 library used in v2.1 auto-detects the display address, but if you have issues, check the display initialization in `display.cpp`.

## Power Supply Options

### Option 1: USB Power Bank (Recommended)
- **Pros:** Reliable, rechargeable, easy
- **Cons:** Cable management
- **Connection:** USB cable to NodeMCU micro-USB port

### Option 2: 18650 Battery Pack
- **Pros:** Compact, lightweight
- **Cons:** Need voltage regulator (5V output)
- **Connection:** 5V → VIN, GND → GND

### Option 3: AA Battery Pack (4×AA)
- **Pros:** Common batteries, ~6V output
- **Cons:** Need voltage regulator to 5V
- **Connection:** Through 5V regulator → VIN

### Power Consumption

| Component | Current Draw | Notes |
|-----------|--------------|-------|
| ESP8266   | ~80 mA       | During active operation |
| MPU6050   | ~3.6 mA      | Normal mode |
| OLED      | ~20 mA       | Full brightness |
| **Total** | **~104 mA**  | Typical operation |

**Battery Life Estimates:**
- 2500 mAh power bank: ~24 hours
- 2000 mAh 18650 cells: ~19 hours
- 4×AA (2000 mAh): ~19 hours

## Cable Recommendations

### MPU6050 Cable
- **Length:** <30 cm (shorter is better for noise immunity)
- **Type:** 4-wire (I²C + power)
- **Gauge:** 24-26 AWG
- **Shielding:** Optional but recommended for very long runs

### OLED Cable
- **Length:** <50 cm (can be longer than sensor cable)
- **Type:** 4-wire (I²C + power)
- **Gauge:** 24-26 AWG

### Power Cable
- **Length:** As needed for battery placement
- **Gauge:** 22-24 AWG (handles higher current)

## I²C Pull-up Resistors

Most NodeMCU boards and sensor modules have built-in pull-up resistors (typically 10kΩ).

**If you experience I²C communication issues:**
- Add external 4.7kΩ pull-up resistors from SDA to 3.3V
- Add external 4.7kΩ pull-up resistors from SCL to 3.3V

**Do NOT use pull-ups if:**
- Both devices already have them (risk of too-strong pull-up)
- Communication works fine without them

## MPU6050 Mounting Orientation

### v2.1 Feature: Rotation-Invariant Mounting!

**Big Change:** In v2.1, the sensor can be mounted at **ANY rotational angle** around the telescope tube!

```
         Telescope Tube (OTA)
   ════════════════════════════════

      ┌─────────┐
      │ MPU6050 │  ← Sensor PCB can be at ANY angle
      │         │
      │    ?↑   │  ← Orientation doesn't matter!
      │    │    │
      │  ?←┼→?  │
      │    ↓    │
      └─────────┘
```

**What matters:**
- ✅ Sensor is rigidly attached to telescope tube
- ✅ No flex or movement in mounting
- ❌ Exact axis alignment is NO LONGER critical

**How it works:**
During zero calibration, the system captures the complete 3D gravity vector and uses vector mathematics to calculate altitude regardless of sensor rotation around the tube.

### Mounting Methods

#### Method 1: 3D Printed Bracket (Recommended)
- Custom fit to tube diameter
- Rigid mounting
- Easy to align
- Can include cable management

#### Method 2: Metal Bracket
- Very rigid
- Can use pipe clamps
- May need padding to prevent slip

#### Method 3: Strong Adhesive
- **Use:** VHB tape or epoxy
- **Pros:** Clean, lightweight
- **Cons:** Difficult to remove/adjust
- **Warning:** Ensure no flex at all

### Critical Mounting Requirements

✅ **Must Have:**
- Zero flex or movement
- Rigid attachment to OTA
- Protected from bumps
- Cable strain relief

❌ **Avoid:**
- Mounting near heat sources
- Mounting on movable parts
- Mounting with tape that can peel
- Long, unsupported sensor cables

## OLED Display Mounting

### Placement Options

1. **On rocker box side panel**
   - Most visible
   - Easy to read while observing
   - Near button access

2. **On front board**
   - Very accessible
   - Clear sightline
   - May need longer cable

3. **Near eyepiece**
   - Convenient while observing
   - Good for quick checks
   - Requires careful cable routing

### Mounting Method
- 3D printed bracket with screw mounts
- Small standoffs to protect screen
- Consider adding a protective cover for outdoor use

## Button Options

### Option 1: Built-in Button (Default)
- Uses NodeMCU's onboard FLASH button (D3/GPIO0)
- **Pros:** No extra wiring needed
- **Cons:** Must be near NodeMCU

### Option 2: External Button
- Connect external button to D3 and GND
- **Pros:** Can mount anywhere convenient
- **Cons:** Two extra wires
- **Wiring:** Button between D3 and GND (uses internal pull-up)

## Troubleshooting Wiring Issues

### I²C Not Working

**Symptoms:**
- "MPU6050 FAIL" error
- Display not initializing
- Devices not found in I²C scan

**Solutions:**
1. Verify 3.3V on VCC pins (measure with multimeter)
2. Check SDA/SCL aren't swapped
3. Ensure good solder connections
4. Try shorter cables
5. Add external pull-up resistors (4.7kΩ)
6. Run I²C scanner (included in test_orientation.ino)

### Power Issues

**Symptoms:**
- System resets randomly
- Display flickers
- Sensor readings unstable

**Solutions:**
1. Use thicker power wires (22 AWG minimum)
2. Add 100µF capacitor near NodeMCU VIN
3. Check battery voltage (should be 4.5-5.5V at VIN)
4. Reduce OLED brightness if needed

### Noise / Unstable Readings

**Symptoms:**
- Altitude jumps around
- Readings don't stabilize
- Interference on display

**Solutions:**
1. Shorten sensor cable
2. Add ferrite bead on sensor cable
3. Keep power and signal cables separated
4. Ensure rigid sensor mounting
5. Increase filter alpha value in `config.h`:
   ```cpp
   #define ALPHA 0.1  // Lower = smoother
   ```

## Testing Checklist

Before mounting permanently:

- [ ] I²C devices detected (0x68 and 0x3C)
- [ ] MPU6050 reads acceleration data
- [ ] OLED displays startup screen correctly
- [ ] Button responds to presses (short and long)
- [ ] ~~Y-axis responds most to tilt~~ (Not needed in v2.1!)
- [ ] No loose connections
- [ ] Power supply provides stable 5V
- [ ] All components secured properly
- [ ] Cables have strain relief
- [ ] System runs for 10+ minutes without issues
- [ ] All required files uploaded (see modular structure)

## Next Steps

1. **Test on breadboard** - Verify all connections work
2. **Upload all files** - Main .ino plus all .h and .cpp modules
3. **Test full functionality** - Check display, sensor, button
4. **Design/print mounting brackets** - Plan permanent installation
5. **Mount sensor on telescope** - Rigid attachment is critical (orientation flexible!)
6. **Mount display** - Easily visible position
7. **Run calibration** - Set zero and mechanical stops
8. **Test in field** - Verify performance during observation

---

**v2.1 Advantage:** No need to worry about perfect sensor alignment! Just make sure it's rigidly attached to the telescope tube, and the calibration will handle the rest.
