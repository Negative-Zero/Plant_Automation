# Plant Automation (ESP32 + Heltec OLED)

Microcontroller firmware for a small plant-watering assistant built on the **Heltec WiFi Kit 32 (ESP32)**.  
It reads soil moisture, shows status on the built-in OLED with simple icons, and can drive a pump via relay in **auto** or **manual** mode using onboard buttons.

> Target board: `heltec_wifi_kit_32` (Arduino framework via PlatformIO)

---

## Features

- **Soil moisture sensing** (ADC on ESP32)
- **OLED UI** with status icons (wet/moist/dry × sun/cloud/shade/night)
- **Auto watering** when soil is dry (toggleable from the device)
- **Manual controls** via two buttons (A/B) with debouncing (`ezButton`)
- **Relay control** for a small pump
- **Status LED** feedback
- Lightweight `Sensor` and `Timer` utility classes

---

## Hardware

- **MCU:** Heltec WiFi Kit 32 (ESP32, OLED)
- **Sensors/Actuators:**
  - Capacitive soil moisture sensor (analog out)
  - **Relay** module to switch the pump
  - **Two push buttons** (A/B) for UI control
  - **(Optional)** Window/cover/reed switch input for environmental state
- **Status LED** (external or onboard pin)

> See `docs/pinout-heltec-esp32.jpg` for the board pinout.

### Default pin map (from the code)

| Purpose            | Pin |
|--------------------|----:|
| Soil moisture (ADC)|  2  |
| Window / reed sw.  | 27  |
| Pump relay (OUT)   | 14  |
| Button A           | 12  |
| Button B           | 13  |
| Status LED         | 25  |

> Adjust these in `src/main.cpp` if your wiring differs.

---

## Getting started

### 1) Prerequisites
- [PlatformIO](https://platformio.org/) (VS Code extension or CLI)

### 2) Clone & open
```bash
git clone https://github.com/<you>/plant-automation.git
cd plant-automation
