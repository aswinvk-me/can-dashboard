# CAN Bus Vehicle Dashboard System

A multi-ECU embedded system that simulates a vehicle dashboard using CAN bus communication between three PIC18 microcontrollers. Built with MPLAB X IDE and the XC8 compiler.

---

## Overview

This project implements a real-time vehicle dashboard using three ECUs (Electronic Control Units) that communicate over a CAN bus network. Each ECU handles a distinct role — sensing, input handling, and display — mimicking the architecture found in real automotive systems.

```
 ┌──────────────┐        CAN Bus        ┌──────────────┐
 │    ECU 1     │◄─────────────────────►│    ECU 3     │
 │  Speed+Gear  │                       │   Display    │
 └──────────────┘                       │  (16x2 LCD)  │
                                        └──────┬───────┘
 ┌──────────────┐                              │
 │    ECU 2     │◄─────────────────────────────┘
 │  RPM+Indicator│
 └──────────────┘
```

---

## ECU Descriptions

### ECU 1 — Speed & Gear Node (`candash_ecu1.X`)
- Reads **vehicle speed** from a potentiometer via the ADC (Channel 4)
- Reads **gear input** from a digital keypad (3 switches: Up / Down / Reverse)
- Supports gears: **G0 (Neutral), G1–G5, Reverse (R), Custom (_C)**
- Transmits speed and gear over CAN using message IDs `0x10` and `0x20`

### ECU 2 — RPM & Indicator Node (`cardash_ecu2.X`)
- Reads **engine RPM** from a potentiometer via ADC (Channel 4)
- Reads **turn indicator / hazard** input from a 4-switch keypad
  - Switch 1 → Left indicator
  - Switch 2 → Right indicator
  - Switch 3 → Hazard (both)
  - Switch 4 → Off
- Transmits RPM and indicator state over CAN using message IDs `0x30` and `0x40`

### ECU 3 — Dashboard Display Node (`cardash_ecu3.X`)
- Receives all CAN messages and renders data on a **16×2 Character LCD**
- Uses **Timer0 interrupt** for indicator blink logic (independent of main loop)
- Controls **two GPIO LEDs** (RB0, RB7) for left and right indicator lights
- LCD layout:
  ```
  Line 1: SPD RPM  GR IND
  Line 2: 085 4200 G3 ->
  ```

---

## CAN Message ID Map

| Message ID | Data        | Source |
|------------|-------------|--------|
| `0x10`     | Speed (0–99 km/h) | ECU 1 |
| `0x20`     | Gear (0–7)  | ECU 1 |
| `0x30`     | RPM (raw, scaled ×60) | ECU 2 |
| `0x40`     | Indicator (0=Off, 1=Left, 2=Right, 3=Hazard) | ECU 2 |

---

## Hardware

| Component | Details |
|-----------|---------|
| Microcontroller | PIC18F series (with ECAN module) |
| Clock | 20 MHz crystal |
| CAN Pins | RB2 (TX), RB3 (RX) |
| ADC Input | Channel 4 (potentiometer) |
| Keypad | 3×4 matrix keypad (PORTB) |
| LCD | 16×2 Character LCD — PORTD (data), PORTC (control) |
| Indicators | RB0 (Left LED), RB7 (Right LED) |

---

## Software & Tools

| Tool | Details |
|------|---------|
| IDE | MPLAB X IDE |
| Compiler | XC8 (Microchip) |
| Language | Embedded C |
| CAN Peripheral | ECAN module (Mode 0) |
| Baud Rate Config | `BRGCON1=0xE1`, `BRGCON2=0x1B`, `BRGCON3=0x03` (8 MHz Fosc) |

---

## Project Structure

```
candash_ecu1.X/          # ECU 1 — Speed & Gear
├── main.c
├── can.c / can.h         # ECAN transmit/receive driver
├── adc.c / adc.h         # ADC driver
├── digital_keypad.c/.h   # Keypad scanner (state-change detection)
├── message_id.h          # Shared CAN message ID definitions
├── Makefile
└── nbproject/            # MPLAB X project config

cardash_ecu2.X/          # ECU 2 — RPM & Indicators
├── main.c
├── can.c / can.h
├── adc.c / adc.h
├── digital_keypad.c/.h
├── ecu2.c / ecu2.h       # Peripheral init helpers
├── message_id.h
├── Makefile
└── nbproject/

cardash_ecu3.X/          # ECU 3 — Display Node
├── main.c
├── can.c / can.h
├── clcd.c / clcd.h       # Character LCD driver
├── message_id.h
├── Makefile
└── nbproject/
```

---

## How to Build

1. Open each `.X` folder as a project in **MPLAB X IDE**
2. Select your target PIC18 device in project properties
3. Build using the **XC8 compiler**
4. Flash the generated `.hex` file to each respective MCU

---

## Key Implementation Details

- **ECAN in Mode 0** — uses legacy RXB0/TXB0 registers for simplicity
- **Filters disabled** on ECU 1 and ECU 2 (transmit-only); ECU 3 receives all IDs by accepting any frame (`RXM1:RXM0 = 11`)
- **ADC scaling** — raw 10-bit ADC result (0–1023) divided by 10.23 to map to 0–99 range
- **Indicator blink** — Timer0 overflow ISR toggles a `blink` flag every ~10,000 ticks; the main loop checks this flag before updating the LCD and LEDs
- **Gear encoding** — 0=Neutral, 1–5=Gears, 6=Reverse, 7=Custom; stored as index into a string array for direct LCD rendering

---

## .gitignore

```
build/
dist/
*.hex
*.elf
*.map
*.o
*.d
*.lst
```

---

## Author

**Aswin**  
Embedded Systems Project — March 2026
