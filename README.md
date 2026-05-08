# Embers Lighting — LLU_V2 Firmware

STM32F411 firmware for the Embers UV art lighting system.
5-channel LED controller (R, G, B, UV, White) with WiFi via ESP32.

## Project Structure

| Directory | Description |
|-----------|-------------|
| `MPU_V2_Basic/` | Mid Power Unit — primary development target |
| `LPU_V2_Basic/` | Low Power Unit variant |
| `HPU_V2_Basic/` | High Power Unit variant |
| `LLU_V2_Common/` | Shared source library (LightControl, BIOS, ESP32, IRDA, KBD) |
| `TIM_PWMOutput/` | Timer/PWM reference project |

## Hardware

- MCU: STM32F411 @ 84 MHz
- WiFi: ESP32-WROOM-32
- LED channels: Red, Green, Blue, UV, White
- Driver: Discrete buck converters (MOSFET + inductor + current sense)
- Toolchain: IAR EWARM

## Branch Strategy

- `main` — clean, hardware-validated releases only
- `dev/firmware-refactor` — active development

## Known Issues (baseline firmware)

- No watchdog timer configured
- Blocking I2C loops without timeout
- Duplicate index writes in brightness lookup tables
- I2C2/I2C3 mismatch in receive path
- ADC current monitoring disabled
