# Wiring (Heltec WiFi Kit 32)

This project targets the **Heltec WiFi Kit 32 (ESP32)** with a built-in OLED. Below are default pins used by the firmware (adjust in `src/main.cpp` if your wiring differs).

## Default Pin Map

| Purpose              | ESP32 Pin |
|----------------------|-----------:|
| Soil moisture (ADC)  | 2          |
| Window/reed switch   | 27         |
| Pump relay (output)  | 14         |
| Button A             | 12         |
| Button B             | 13         |
| Status LED           | 25         |

> The OLED on Heltec boards uses I²C internally via the Heltec library—no extra wiring required.

## Relay / Pump Notes

- Use a relay or MOSFET module rated for your pump’s voltage/current.
- Power the pump from a **separate supply**; **do not** draw motor current from the ESP32’s 5V/3.3V rails.
- Share **GND** between the ESP32 and the pump power supply.
- If driving inductive loads, include appropriate **flyback** protection (diode/MOSFET driver per your module).

## Sensor Notes

- Capacitive soil moisture sensors output an **analog voltage**. Calibrate thresholds in code (dry vs moist vs wet) for your soil and container.
- Keep sensor power stable; if noisy, average multiple ADC samples.

## Safety

- Verify polarity/voltage before powering.
- Keep water and electronics physically isolated.
- Strain-relief wires; avoid sharp bends near connectors.
