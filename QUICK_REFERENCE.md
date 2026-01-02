# Quick Reference Card

**Print this page and keep it with your telescope!**

**NEW in v2.1:** Sensor can be mounted at ANY rotation around the tube!

---

## 🔧 Hardware Setup

```
MPU6050 → NodeMCU
-----------------
VCC → 3.3V
GND → GND
SCL → D1
SDA → D2

OLED → NodeMCU
--------------
VCC → 3.3V
GND → GND
SCL → D1 (shared)
SDA → D2 (shared)

Button: Built-in on D3
```

---

## 🎯 Quick Start

1. **Power on** → Wait for startup screen
2. **Short press** → Sync at mechanical stop
3. **Ready!** → Altitude displayed in real-time

---

## 🔘 Button Functions

| Press Type | Action |
|------------|--------|
| **Short** (Normal Mode) | Sync at stop |
| **Long** (2 sec) | Enter calibration |
| **Short** (Cal Mode) | Next step |
| **Long** (Cal Mode) | Cancel |

---

## 📐 Calibration (First Time)

1. **Long press** → Enter calibration mode
2. **Level telescope** → Use bubble level → Press
3. **Move to Stop A** (~30°) → Press
4. **Move to Stop B** (~105°) → Press
5. **Done!** → Saved to memory

**Calibrate when:**
- ✅ First installation
- ✅ Sensor moved
- ✅ After 1 year
- ❌ NOT every session

---

## 🔄 Session Sync (Every Time)

1. Move to **Stop A** or **Stop B**
2. **Short press** button
3. Ready to observe!

**Sync when:**
- ✅ Start of session
- ✅ After long break
- ✅ Temperature changed >10°C

---

## 📊 Display

```
┌─────────────────┐
│ ALTITUDE        │  ← Mode indicator
│                 │
│  +45.3°         │  ← Current altitude (big)
│                 │
│ Raw: 46.1°      │  ← Debug info
└─────────────────┘
```

**If shows [UNCAL]:** Need to calibrate first!

---

## ⚠️ Troubleshooting

### System Won't Start
- Check power (5V to VIN)
- Check USB cable
- Verify battery charged

### "MPU6050 FAIL"
- Check wiring (SDA/SCL)
- Verify 3.3V at sensor
- Check connections tight

### Readings Unstable
- Verify rigid mounting
- Check cable not pulling
- Run session sync

### Readings Inaccurate
- Run session sync
- Check at mechanical stops
- May need full recalibration

### Display Blank
- Check 3.3V power
- Verify I²C wiring
- Check OLED address (0x3C)

---

## 🔍 What's Normal

| Condition | Expected |
|-----------|----------|
| Startup time | ~3 seconds |
| Accuracy | ±1° typical |
| Update rate | 10 Hz (smooth) |
| Calibration time | ~5 minutes |
| Sync time | ~10 seconds |
| Battery life | ~20 hours |

---

## 📝 Maintenance

**Weekly:**
- Check mounting tight
- Verify no cable damage
- Session sync at start

**Monthly:**
- Clean display gently
- Check battery charge
- Verify accuracy at stops

**Yearly:**
- Full recalibration
- Check all connections
- Update if drifting

---

## 💾 Data Stored in Memory

✅ Reference gravity vector (3D)
✅ Zero point offset
✅ Stop A position
✅ Stop B position
✅ Calibration valid flag

**Survives power loss!**
**Total: 29 bytes in EEPROM**

---

## 🌡️ Operating Conditions

| Parameter | Range |
|-----------|-------|
| Temperature | -10°C to 40°C |
| Humidity | 0-80% (non-condensing) |
| Power | 4.5V - 5.5V |
| Altitude range | Full telescope movement |

---

## 🔮 Future Features

Coming soon:
- Azimuth measurement
- Push-to functionality
- WiFi connectivity
- Star alignment

---

## 📞 Support

**Serial Monitor:** 115200 baud  
**Debug mode:** Shows raw values  
**I²C devices:** 0x68 (MPU), 0x3C (OLED)

---

## ⚡ Emergency Reset

**Lost calibration?**
1. Long press → Enter cal mode
2. Follow on-screen prompts
3. Takes ~5 minutes

**System frozen?**
1. Unplug power
2. Wait 5 seconds
3. Reconnect power

---

## 🎓 Tips for Best Results

1. 📏 **Calibrate indoors first** (no wind)
2. 🔧 **Check mounting is RIGID** (critical!)
3. ⏱️ **Let system warm up** 2-3 minutes (if cold weather)
4. 🎯 **Use mechanical stops properly**
5. 🔄 **Session sync each time** (10 seconds)
6. 📝 **Keep calibration log** in notes
7. 🌡️ **Calibrate at usage temperature**
8. 🔋 **Check battery before session**

---

## 📐 Code Architecture

**v2.1 Modular Design:**
- `config.h` - Configuration
- `sensor.cpp` - MPU6050 interface
- `calibration.cpp` - Calibration system
- `display.cpp` - OLED display
- `button.cpp` - Button handling
- `telescope_altimeter.ino` - Main

**Benefits:** Clean, maintainable, testable code

---

## 📅 Calibration Log

| Date | Temp | Notes |
|------|------|-------|
|      |      |       |
|      |      |       |
|      |      |       |

---

**Version 2.1** | Telescope Digital Altimeter
**Designed for:** Skywatcher Classic 200
**Built by:** Pawel 🔭✨

**v2.1 Features:**
- ✨ Rotation-invariant sensor mounting
- 📦 Modular code architecture
- 🎯 Improved accuracy with vector math
- 📚 Updated libraries (MPU6050, U8g2)

---

**Keep this card with your telescope equipment!**
