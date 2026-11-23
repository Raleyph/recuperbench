# Recuperbench

**Recuperbench** is firmware for a custom-built workbench used to manufacture parts for heat-recovery ventilation units (recuperators). The machine was repurposed from a 3D printer and uses a **GT2560 Rev A+** controller, two coordinated stepper-driven axes, a 128 × 64 graphical display, and a rotary encoder.

The firmware runs on an **ATmega2560** using the Arduino framework and PlatformIO.

> **Firmware version:** v1.4

## Features

- Automatic Z-axis homing during startup.
- Simple, encoder-controlled machining workflow.
- Synchronized Z- and C-axis step generation using a Timer1 interrupt.
- Adjustable machining speed, displayed as a 10–100% setting.
- Save the selected speed to EEPROM and restore it on startup.
- Graphical status display with a startup splash screen.
- Z-axis travel limits monitored through two endstop inputs.

## Hardware

The original machine is described in the source as using:

| Component | Specification |
| --- | --- |
| Controller | GT2560 Rev A+ / ATmega2560 |
| Display | MKS Mini 12864KBA v1.2 (ST7920-compatible interface in firmware) |
| Motors | 42SHD0034-20B stepper motors: two on Z, one on C |
| Lead screws | T8 |
| Input | Rotary encoder with push button |
| Endstops | Z-axis minimum and maximum |

The Z axis provides linear travel; the C axis rotates in a fixed step ratio relative to the Z axis during machining. The firmware uses one set of Z-axis STEP/DIR/ENABLE signals; check the machine's wiring and drivers for the two physical Z motors.

### Pin configuration

All numbers below are **ATmega2560 Arduino digital pin numbers**, not physical connector positions.

| Function | Pin |
| --- | ---: |
| Z STEP | 37 |
| Z DIR | 39 |
| Z ENABLE | 35 |
| Z MIN endstop | 30 |
| Z MAX endstop | 32 |
| C STEP | 25 |
| C DIR | 23 |
| C ENABLE | 27 |
| Encoder A | 40 |
| Encoder B | 42 |
| Encoder button | 19 |
| LCD CLK | 16 |
| LCD DATA | 17 |
| LCD CS | 20 |

These mappings are defined in `lib/config/include/Pins.h`.

**Endstop warning:** The inputs are configured with `INPUT_PULLUP`, but the current implementation treats a **HIGH** reading as an activated endstop. Verify electrical polarity and behavior on your hardware before enabling motor movement. Do not assume standard active-low endstop wiring will work without modification.

## Build and upload

### Prerequisites

- [PlatformIO](https://platformio.org/) (CLI or IDE extension)
- A compatible ATmega2560-based controller
- A USB connection to the controller

The required libraries are installed automatically from `platformio.ini`:

- [EncButton](https://github.com/GyverLibs/EncButton) `^3.7.4`
- [U8g2](https://github.com/olikraus/u8g2) `^2.36.15`

### Commands

From the repository root:

```bash
# Build the firmware
pio run -e recuperbench

# Upload it to the controller
pio run -e recuperbench -t upload

# Open the serial monitor (9600 baud)
pio device monitor -b 9600
```

If PlatformIO cannot detect the correct upload device, provide the relevant port in your local configuration or use the CLI's upload-port option.

The project configuration specifies `atmelavr`, `megaatmega2560`, Arduino, and GNU C++17. The serial interface is initialized, but this version does not implement a serial command console.

## Operating the machine

The firmware uses the following sequence:

```text
  INIT ──(Z at home)──> READY ──(encoder click)──> MACHINING
                       ^                              |
                       |                        (Z at end)
                       |                              v
                     HOMING <──(encoder click)── PART_REMOVAL
                       |
                  (Z at home)
                       └──────────────────────> READY
```

1. **INIT:** The Z axis returns to its home endstop if not already there; the C-axis driver is disabled.
2. **READY:** The machine waits for an encoder click to begin machining. Speed can be adjusted with the rotary encoder.
3. **MACHINING:** The C-axis driver is enabled, the C axis is set to the forward direction, and the Z axis travels towards its end limit. The C step pulses are synchronized with Z movement.
4. **PART REMOVAL:** On reaching the Z end limit, C movement is stopped and its driver disabled. Remove the completed part, then click the encoder.
5. **HOMING:** The Z axis travels back to home. Once the home endstop is detected, the machine returns to READY.

### Encoder controls

| Action | Behavior |
| --- | --- |
| Rotate | Adjust Z-axis step timing (one speed increment for every two registered turn events) |
| Click in READY | Start machining, provided Z is at home |
| Click in PART REMOVAL | Start homing |
| Hold in READY | Store the current speed setting in EEPROM, provided Z is not moving |

The LCD displays the current state, the selected speed percentage, and the stored speed percentage (`ST.SPEED`). It redraws approximately every 300 ms, after a 2-second splash screen.

## Motion and speed configuration

Motion parameters are in `lib/config/include/Kinematics.h`:

```cpp
#define C_HOME_DIR  FORWARD
#define C_END_DIR   BACKWARD

#define Z_HOME_DIR  BACKWARD
#define Z_END_DIR   FORWARD

static constexpr uint8_t STEP_INTERVAL_MIN = 2;
static constexpr uint8_t STEP_INTERVAL_MAX = 11;

static constexpr uint16_t Z_STEPS = 25; // Steps per 50 mm
static constexpr uint16_t C_STEPS = 4;  // Steps per 360 deg.
```

The **configured synchronization ratio is 4 C-axis steps per 25 Z-axis steps**, implemented by an accumulator-based `AxisSynchronizer`. These are firmware configuration values; the comments describe nominal mechanical quantities, not a verified physical calibration.

Timer1 operates in CTC mode with an 8× prescaler and `OCR1A = 99`. At a 16 MHz MCU clock, the interrupt rate is 20 kHz. Z step pulses change level after a configured number of timer interrupts, so lower `ticksPerStep` values correspond to higher speed. The UI maps settings **2 → 100%** through **11 → 10%**; the percentage is a relative setting, not a measured motor RPM or axis feed rate.

The saved speed is a `uint16_t` at EEPROM address `0`. An out-of-range stored value defaults to `STEP_INTERVAL_MIN` on startup.

## Project structure

```text
recuperbench/
├── platformio.ini
├── src/
│   └── main.cpp                   # Arduino setup() and loop()
├── lib/
│   ├── app/
│   │   ├── include/
│   │   │   ├── System.h
│   │   │   └── SystemState.h
│   │   └── src/System.cpp         # Initialization, FSM, EEPROM and inputs
│   ├── config/include/
│   │   ├── Kinematics.h           # Axis directions, timing and step ratio
│   │   ├── Pins.h                 # GPIO assignments
│   │   └── UI.h                   # Firmware label and version
│   ├── drivers/
│   │   ├── include/
│   │   │   ├── LCD.h
│   │   │   └── StepperAxis.h
│   │   └── src/
│   │       ├── LCD.cpp            # ST7920 display rendering
│   │       └── StepperAxis.cpp    # Motor control and limit detection
│   └── services/
│       ├── include/AxisSynchronizer.h
│       └── src/AxisSynchronizer.cpp
├── LICENSE
└── README.md
```

## Important limitations and safety notes

This is machine-specific firmware, **not a certified motion controller or safety system**.

- **Test endstop polarity first.** As implemented, a HIGH input is considered triggered even though internal pull-ups are enabled.
- **No independent emergency-stop logic is implemented in the firmware.** Provide a suitable hardware emergency stop and motor power isolation.
- **C-axis travel is not endstop-protected.** The C axis is configured with `NO_PIN` for both limits, and synchronized pulses are issued directly by the ISR.
- **No acceleration or deceleration profiles are implemented.** Movement starts and stops using the configured fixed-rate timing.
- **The default motion and step-ratio settings are specific to the original machine.** Review direction, gearing, microstepping, endstops, and travel limits before using different hardware.
- **Speed adjustment and EEPROM persistence are intentionally simple.** There is no dedicated parameter calibration or configuration menu.

Always test with motors mechanically disengaged or at a safely controlled low-energy state before operating the assembled workbench.

## License

Released under the [MIT License](LICENSE).

Copyright © 2025 **Raleyph**

Designed by [Raleyph](https://github.com/Raleyph)
