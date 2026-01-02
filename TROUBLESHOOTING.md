# Troubleshooting Guide (v2.1)

Complete solutions for common issues with the telescope digital altimeter.

---

## 🔍 Diagnostic Tools

### Serial Monitor (Essential!)

**How to access:**
1. Connect NodeMCU via USB
2. Open Arduino IDE
3. Tools → Serial Monitor
4. Set baud rate to **115200**

**What you'll see (v2.1):**
```
=== Telescope Altimeter v2.1 ===
MPU6050 initialized successfully
Display initialized successfully
Calibration loaded:
  Zero offset: 0.00
  Stop A raw: 28.67
  Stop B raw: 103.45
  Tube axis: (0.123, 0.985, -0.122)
Setup complete!
Ready to measure altitude.
```

### I²C Scanner

Included in `test_orientation.ino`:
```
Scanning I2C bus...
Found device at 0x68 (MPU6050)
Found device at 0x3C (OLED Display)
Found 2 device(s)
```

---

## ⚠️ Startup Problems

### System Won't Power On

**Symptoms:**
- No display
- No LED activity
- Completely dead

**Check:**
1. ✅ Power supply connected?
2. ✅ Battery charged? (>4.5V)
3. ✅ USB cable good? (try different cable)
4. ✅ NodeMCU LED lit when powered?
5. ✅ Polarity correct? (VIN=+, GND=-)

**Solutions:**
- Use multimeter to verify 5V at NodeMCU VIN
- Try different power source
- Check for blown fuse in battery pack
- Verify NodeMCU not damaged

---

### "MPU6050 FAIL" Error

**Symptoms:**
- Display shows: "ERROR: MPU6050 FAIL"
- Serial: "ERROR: MPU6050 initialization failed!"
- System halts

**Most common causes:**
1. ❌ Wiring error
2. ❌ No power to sensor
3. ❌ Wrong I²C pins
4. ❌ Defective sensor

**Diagnostic steps:**

**Step 1:** Check power
```
Measure voltage at MPU6050:
- VCC pin: Should be 3.3V
- GND pin: Should be 0V
```

**Step 2:** Check I²C wiring
```
MPU6050 → NodeMCU
SCL     → D1 (GPIO5)
SDA     → D2 (GPIO4)
```
Are they swapped? Reversed?

**Step 3:** Run I²C scanner
- Upload `test_orientation.ino`
- Open Serial Monitor
- Should find device at 0x68

**Step 4:** Check for short circuits
- Disconnect sensor
- Measure resistance VCC to GND
- Should be >1kΩ (not short)

**Solutions:**
- ✅ Fix wiring errors
- ✅ Ensure 3.3V supply (NOT 5V!)
- ✅ Reseat connections
- ✅ Try different MPU6050 module
- ✅ Check solder joints on sensor

---

### Display Not Working

**Symptoms:**
- Display stays blank
- Backlight may be on but no text
- Serial says "SSD1306 allocation failed"

**Check:**
1. ✅ Display gets 3.3V?
2. ✅ I²C wiring correct?
3. ✅ Right display address?

**Solutions:**

**Fix 1:** Verify I²C address
```cpp
// Try changing in code:
#define SCREEN_ADDRESS 0x3C  // or try 0x3D
```

**Fix 2:** Run I²C scanner
- Should find display at 0x3C or 0x3D
- If not found, wiring issue

**Fix 3:** Check compatibility
- Must be SSD1306 controller
- 128×64 pixels
- I²C interface (not SPI!)

**Fix 4:** Power issue
- Measure 3.3V at display VCC
- Some displays need 5V (check specs)
- Try powering from VIN instead

---

### System Boots Then Freezes

**Symptoms:**
- Shows startup screen
- Freezes after a few seconds
- No updates to display

**Causes:**
- I²C communication failure
- Memory corruption
- Watchdog timer reset

**Solutions:**
1. Check for I²C conflicts
2. Add delay in main loop if missing
3. Verify EEPROM not corrupted:
```cpp
// Add to setup() temporarily:
EEPROM.put(ADDR_CALIBRATED_FLAG, (byte)0x00);
EEPROM.commit();
// Forces fresh start
```
4. Re-upload code from scratch

---

## 📊 Reading Problems

### Altitude Readings Unstable/Jumping

**Symptoms:**
- Display shows rapidly changing values
- Values jump ±5° randomly
- Can't get stable reading

**Primary cause:** **SENSOR NOT RIGID**

**Diagnostic test:**
1. Hold telescope perfectly still
2. Watch display for 10 seconds
3. Should vary by <0.5°

If varies more:

**Fix 1:** Check mounting
- Physically wiggle sensor
- Should have ZERO movement
- Check all screws tight
- Verify bracket not flexing

**Fix 2:** Reduce noise
- Shorten sensor cable to <30cm
- Keep away from motors/servos
- Add ferrite bead on cable
- Route away from power wires

**Fix 3:** Increase filtering
```cpp
// In config.h, change:
#define ALPHA 0.1  // Was 0.2, more smoothing
```
Lower = smoother, slower response

**Fix 4:** Check I²C communication
- Run I²C scanner
- Verify clean 3.3V power
- Check for loose connections

---

### Readings Consistently Wrong

**Symptoms:**
- Shows 45° but clearly at 60°
- Linear offset (always off by X°)
- Doesn't match visual estimate

**Causes:**
1. Not calibrated
2. Wrong stop positions in code
3. Sensor moved after calibration
4. Wrong axis being measured

**Solutions:**

**Check 1:** Is calibrated?
- Display should NOT show [UNCAL]
- Serial should show calibration data
- If not: Long press → Full calibration

**Check 2:** Test at known positions
- Move to Stop A (mechanical stop)
- Note reading
- Should match where you calibrated Stop A
- If way off: Recalibrate

**Check 3:** Verify sensor hasn't moved
- Check mounting bracket tight
- Look for any rotation or flex
- If moved: Must recalibrate

**Check 4 (v2.1):** Sensor orientation is now flexible
- v2.1 automatically handles any mounting angle
- No need to worry about axis alignment
- Just ensure sensor is rigidly attached to tube

---

### Readings Drift Over Time

**Symptoms:**
- Accurate initially
- Becomes inaccurate after 15-30 min
- Drifts 2-5° off

**Causes:**
1. Temperature change
2. Battery voltage drop
3. Sensor mounting flex
4. Calibration aging

**Solutions:**

**Short term:**
- Run session sync every 30 minutes
- Let system warm up before use
- Keep temperature stable

**Long term:**
- Add insulation around sensor
- Use regulated power supply
- Reinforce mounting
- Recalibrate more frequently

**Test for thermal drift:**
1. Calibrate at room temp
2. Take outside (cold)
3. Note error magnitude
4. This is your expected drift

---

### Non-Linear Errors

**Symptoms:**
- Accurate at stops
- Inaccurate in middle (e.g. 45-60°)
- Error varies with position

**Causes:**
- Sensor mounting angle >15° off
- Mechanical geometry effects
- Accelerometer non-linearity

**Solutions:**

**If sensor angle >15° off:**
- Remount sensor more accurately
- Y-axis must be parallel to tube

**If mechanical issues:**
- Check telescope mount doesn't flex
- Verify altitude bearing smooth
- Check no binding in movement

**If accelerometer non-linear:**
- Two-point calibration should handle
- Consider three-point calibration
- May need different sensor (rare)

---

## 🔘 Button Issues

### Button Not Responding

**Symptoms:**
- Press button, nothing happens
- No sync, no calibration entry
- System ignores button

**Check:**
1. Using correct button? (D3 on NodeMCU)
2. Button actually working? (measure continuity)
3. Code uploaded correctly?

**Test button:**
```cpp
// Upload simple test:
void setup() {
  Serial.begin(115200);
  pinMode(D3, INPUT_PULLUP);
}
void loop() {
  Serial.println(digitalRead(D3));
  delay(100);
}
// Should print 1 (HIGH) normally
// Should print 0 (LOW) when pressed
```

**Solutions:**
- Use built-in FLASH button
- Or wire external button D3-to-GND
- Check internal pullup enabled
- Verify no wiring errors

---

### Button Too Sensitive / False Triggers

**Symptoms:**
- Single press detected as multiple
- Random triggers when not touched
- Calibration jumps steps

**Causes:**
- Button bounce
- Electrical noise
- Touch sensitivity

**Solutions:**

**Increase debounce:**
```cpp
#define DEBOUNCE_DELAY 100  // Was 50, more filtering
```

**Add capacitor:**
- 0.1µF across button pins
- Reduces electrical noise

**Check wiring:**
- Button wires not near power
- Good ground connection
- No loose connections

---

## 🔋 Power Problems

### Battery Drains Quickly

**Symptoms:**
- Full battery lasts <5 hours
- Expected 20+ hours
- System shuts off unexpectedly

**Check power consumption:**
- ESP8266: ~80mA
- MPU6050: ~4mA
- OLED: ~20mA
- **Total: ~100mA expected**

**If using more:**

**Reduce OLED brightness:**
- Most displays auto-adjust
- Check if manually overridden
- Can turn off during observation

**Check for power leaks:**
- Measure current with multimeter
- Should be ~100mA
- If >150mA, find leak

**Battery capacity:**
- 2000mAh → ~20 hours
- 1000mAh → ~10 hours
- Check actual battery rating

---

### System Resets Randomly

**Symptoms:**
- Suddenly reboots
- Returns to startup screen
- Happens intermittently

**Causes:**
1. Brown-out (voltage sag)
2. Power supply inadequate
3. Loose connection
4. Software crash

**Solutions:**

**Fix power:**
- Use thicker power wires
- Add 100µF capacitor at VIN
- Check battery fully charged
- Verify voltage >4.5V under load

**Check connections:**
- All connections tight?
- No intermittent contacts?
- Solder instead of breadboard?

**Enable watchdog:**
```cpp
// Not yet implemented, but can add
ESP.wdtDisable();  // During startup
ESP.wdtEnable(8000);  // 8 sec timeout
ESP.wdtFeed();  // In main loop
```

---

## 🛠️ Calibration Problems

### Can't Complete Calibration

**Symptoms:**
- Calibration freezes
- Button doesn't advance steps
- System stuck in cal mode

**Solutions:**

**Cancel calibration:**
- Long press button (2 sec)
- Returns to normal mode
- Try again

**Check each step:**
- Is telescope actually level? (Step 1)
- At mechanical stop? (Steps 2,3)
- Is button working?

**Force calibration exit:**
- Power cycle system
- Will return to normal mode
- Calibration not saved

---

### Calibration Doesn't Save

**Symptoms:**
- Complete calibration
- After reboot: [UNCAL] shown
- Must recalibrate every power on

**Check EEPROM:**
```cpp
// Verify EEPROM working
EEPROM.begin(512);
EEPROM.write(0, 0xAA);
EEPROM.commit();
byte test = EEPROM.read(0);
Serial.println(test, HEX);  // Should print AA
```

**Solutions:**
- EEPROM.commit() must be called
- Check if EEPROM.begin() succeeds
- Verify ESP8266 not faulty
- May need to reformat EEPROM

---

## 🌡️ Environmental Issues

### Condensation on Sensor

**Problem:** Water droplets on MPU6050

**Prevention:**
- Add desiccant pack near sensor
- 3D print protective cover
- Use conformal coating spray
- Keep in dry storage

**If wet:**
- Power off immediately
- Dry completely (24 hrs)
- Check for corrosion
- Test before using

---

### Extreme Temperature

**Too cold (<-10°C):**
- Battery capacity reduced
- Display may be slow
- OLED contrast poor
- Readings may drift

**Too hot (>40°C):**
- Sensor drift increases
- Calibration changes
- Battery issues
- Display damage risk

**Solutions:**
- Calibrate at usage temperature
- Let system acclimatize
- Use temperature-rated components
- Add insulation

---

## 📱 Software Issues

### Code Won't Upload

**Error: "espcomm_upload_mem failed"**

**Solutions:**
1. Hold FLASH button during upload
2. Try slower upload speed (57600)
3. Use different USB cable
4. Install CH340 drivers (Windows)
5. Check correct board selected

**Error: "Port not found"**

**Solutions:**
1. Install CH340/CP2102 drivers
2. Check Device Manager (Windows)
3. Try different USB port
4. Restart Arduino IDE

---

### Libraries Not Found

**Error: "Adafruit_MPU6050.h: No such file"**

**Solution:**
1. Sketch → Include Library → Manage Libraries
2. Search: "Adafruit MPU6050"
3. Install all Adafruit libraries:
   - Adafruit MPU6050
   - Adafruit SSD1306
   - Adafruit GFX Library
   - Adafruit Unified Sensor

---

## 🔬 Advanced Diagnostics

### Measure I²C Signal Quality

Use oscilloscope:
- SDA and SCL lines
- Should see clean square waves
- ~100kHz or 400kHz frequency
- No ringing or noise

Poor quality = add pull-ups or shorten cables

---

### Memory Usage Check

```cpp
// Add to setup():
Serial.print("Free heap: ");
Serial.println(ESP.getFreeHeap());
// Should be >30kB
```

---

### EEPROM Dump

```cpp
// Add to setup():
Serial.println("EEPROM contents:");
for (int i = 0; i < 16; i++) {
  Serial.print(EEPROM.read(i), HEX);
  Serial.print(" ");
}
Serial.println();
```

---

## 📞 Getting Help

### Information to Provide

When asking for help, include:

1. **Hardware:**
   - NodeMCU version
   - MPU6050 module type
   - OLED model/specs
   - Power supply details

2. **Software:**
   - Arduino IDE version
   - Board package version
   - Library versions
   - Which sketch uploaded

3. **Problem:**
   - Exact error message
   - When it occurs
   - Serial Monitor output
   - What you've already tried

4. **Environment:**
   - Temperature
   - Humidity
   - How long since calibration
   - Recent changes

---

## ✅ System Health Checklist

Run through this periodically:

**Power:**
- [ ] Battery charged >70%
- [ ] Voltage at VIN = 4.5-5.5V
- [ ] No voltage sags
- [ ] Connections tight

**Mechanical:**
- [ ] Sensor mounting rigid
- [ ] No visible damage
- [ ] Cables secure
- [ ] No wear on stops

**Calibration:**
- [ ] Less than 3 months old
- [ ] Tested at both stops
- [ ] Accuracy within ±1°
- [ ] No drift during session

**Software:**
- [ ] No errors in Serial Monitor
- [ ] Display updates smoothly
- [ ] Button responsive
- [ ] EEPROM saving correctly

---

## 🆘 Nuclear Options

**When all else fails:**

### Complete Factory Reset

1. Flash blank sketch to ESP8266
2. Wait 5 seconds
3. Upload main code fresh
4. Force EEPROM clear
5. Run full calibration
6. Test systematically

### Hardware Swap Test

Replace components one at a time:
1. Try different NodeMCU
2. Try different MPU6050
3. Try different OLED
4. Try different power supply

Identifies faulty component.

---

## 📈 Performance Optimization

### For Maximum Accuracy

1. Use metal mounting bracket
2. Calibrate at usage temperature
3. Session sync every 30 minutes
4. Keep sensor cable <20cm
5. Use shielded cable
6. Regulated 5V power supply
7. Add pull-up resistors (4.7kΩ)
8. Conformal coating on sensor

### For Maximum Stability

1. Lower ALPHA to 0.1
2. Increase averaging samples to 100
3. Slower display update (5 Hz)
4. Temperature compensation (advanced)
5. Three-point calibration

---

**Still having issues? Check the README and CALIBRATION_GUIDE for more details!**
