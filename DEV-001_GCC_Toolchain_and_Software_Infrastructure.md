# DEV-001 — GCC Toolchain & Software Infrastructure Guide
*Embers Lighting — LLU_V2 Firmware*
*Companion to: embers_git_reference.md*

---

## 1. Context

The factory firmware was built using **IAR EWARM** — a paid Windows-only IDE. No IAR license is available. This guide documents the **GCC-based replacement toolchain** established on Pop!_OS Linux, covering installation, build system, flash/debug workflow, and diagnostic commands used in active development sessions.

All development happens on:
- **OS:** Pop!_OS Linux (Ubuntu-based)
- **Working directory:** `~/embers_fw/`
- **Primary target:** `MPU_V2_Basic/` (STM32F411 @ 84 MHz)

The IAR project files (`.ewp`, `.eww`) remain in the repository as read-only reference. They are not used in the build.

---

## 2. Installed Toolchain

### Components

| Tool | Purpose | Install command |
|------|---------|----------------|
| `arm-none-eabi-gcc` | Cross-compiler for ARM Cortex-M4 | `sudo apt install gcc-arm-none-eabi` |
| `arm-none-eabi-objcopy` | Converts .elf → .bin for flashing | Included with above |
| `arm-none-eabi-size` | Reports flash/RAM usage | Included with above |
| `arm-none-eabi-addr2line` | Maps PC addresses to C source lines | Included with above |
| `arm-none-eabi-nm` | Lists symbols and their addresses | Included with above |
| `openocd` | On-chip debugger — flash and debug via ST-Link | `sudo apt install openocd` |

### Verify installation

```bash
arm-none-eabi-gcc --version
openocd --version
```

---

## 3. Build System

The Makefile lives at:
```
~/embers_fw/MPU_V2_Basic/Makefile
```

Build outputs go to:
```
~/embers_fw/MPU_V2_Basic/build/
  MPU_V2_Basic.elf   ← debug symbol file (used for addr2line, nm)
  MPU_V2_Basic.bin   ← raw binary (used for flashing)
```

### Standard build commands

```bash
cd ~/embers_fw/MPU_V2_Basic

# Build everything
make

# Build and show memory usage
make size

# Clean build artifacts
make clean

# Clean and rebuild from scratch
make clean && make
```

### After any source file edit, always rebuild before flashing:
```bash
make clean && make
```
Skipping this means you are flashing stale firmware. This was the root cause of an entire lost session where source edits had no effect.

---

## 4. Hardware Connection

### ST-Link V2 → MPU_V2_Basic SWD pins

| ST-Link pin | Board pin |
|-------------|-----------|
| SWDIO | PA13 |
| SWDCLK | PA14 |
| GND | GND |
| 3.3V | (power reference only — board powered separately) |

The board requires **12V on the barrel connector** for LED drivers to operate. USB alone is insufficient for LED output but is enough for MCU operation and flashing.

### Verify ST-Link is detected

```bash
openocd -f interface/stlink.cfg -f target/stm32f4x.cfg -c "init; exit"
```

Expected output includes:
```
STLINK V2J29M18 (API v2) VID:PID 0483:374B
stm32f4x.cpu: hardware has 6 breakpoints, 4 watchpoints
```

The recurring warning `target voltage may be too low for reliable debugging` is a known quirk with this ST-Link V2 unit. It does not prevent correct operation — communication succeeds regardless.

---

## 5. Flashing

### Flash current build (after `make`)

```bash
openocd -f interface/stlink.cfg -f target/stm32f4x.cfg \
  -c "program ~/embers_fw/MPU_V2_Basic/build/MPU_V2_Basic.bin verify reset exit 0x08000000"
```

### Flash and verify only (no reset)

```bash
openocd -f interface/stlink.cfg -f target/stm32f4x.cfg \
  -c "program ~/embers_fw/MPU_V2_Basic/build/MPU_V2_Basic.bin verify exit 0x08000000"
```

---

## 6. Restoring Factory Firmware (CRITICAL)

The original factory binary is backed up as a raw binary at:
```
~/mpu_backup_original.bin
```

> **Note:** The file may have been saved with a `.elf` extension during initial backup. It is a raw binary regardless of the extension. Use the `.bin` form of the flash command.

### Restore command — exact syntax required

```bash
openocd -f interface/stlink.cfg -f target/stm32f4x.cfg \
  -c "init; halt; flash write_image erase ~/mpu_backup_original.bin 0x08000000 bin; verify_image ~/mpu_backup_original.bin 0x08000000 bin; reset run; exit"
```

**The `bin` keyword at the end of `flash write_image` is mandatory.** Without it, OpenOCD attempts to parse the file as an ELF and fails silently or writes garbage.

The `0x08000000` address is the STM32F4 flash origin — always correct for this target.

---

## 7. Diagnostic Commands

These commands were developed and validated during active debugging sessions. All use single-quoted `-c` blocks to prevent bash from expanding Tcl variables.

### Check where the firmware is executing (3 snapshots)

```bash
openocd -f interface/stlink.cfg -f target/stm32f4x.cfg \
  -c 'init; halt; reg pc; resume; sleep 500; halt; reg pc; resume; sleep 500; halt; reg pc; exit'
```

If all three PC values are identical or very close → firmware is stuck in a tight loop.
If they are scattered across different addresses → firmware is running normally.

### Resolve a PC address to a C function name

```bash
arm-none-eabi-addr2line -e ~/embers_fw/MPU_V2_Basic/build/MPU_V2_Basic.elf -f 0x08001cc6
```

Replace `0x08001cc6` with the PC value from OpenOCD. Returns the function name and source line. Requires the `.elf` file (not `.bin`).

### Find a variable's address

```bash
arm-none-eabi-nm ~/embers_fw/MPU_V2_Basic/build/MPU_V2_Basic.elf | grep "VariableName"
```

Returns address and section. `B` = BSS (zeroed RAM), `D` = data (initialised RAM).

### Read a variable's live value from running firmware

```bash
openocd -f interface/stlink.cfg -f target/stm32f4x.cfg \
  -c 'init; halt; mem2array val 8 0x2000aea8 1; echo "Value:$val(0)"; exit'
```

Replace `0x2000aea8` with the address from `arm-none-eabi-nm`. The `8` means 8-bit read; use `16` for uint16_t variables.

### Write a value to a variable in running firmware

```bash
openocd -f interface/stlink.cfg -f target/stm32f4x.cfg \
  -c 'init; halt; mwb 0x2000adcc 1; resume; exit'
```

`mwb` = memory write byte. `mwh` = half-word (16-bit). `mww` = word (32-bit).

### Read multiple variables in one session

```bash
openocd -f interface/stlink.cfg -f target/stm32f4x.cfg \
  -c 'init; halt; mem2array btn 8 0x2000aea8 1; mem2array sm 8 0x200036fa 1; echo "ManualButton:$btn(0) StateMachine:$sm(0)"; exit'
```

### Write value, wait, then read result

```bash
openocd -f interface/stlink.cfg -f target/stm32f4x.cfg \
  -c 'init; halt; mwb 0x2000adcc 1; resume; sleep 1000; halt; mem2array result 16 0x200036de 1; echo "RedLevel:$result(0)"; exit'
```

---

## 8. Confirmed Variable Addresses

These addresses are valid for the current build. **Re-run `arm-none-eabi-nm` after any rebuild** — addresses change when source is modified.

| Variable | Address | Type | Description |
|----------|---------|------|-------------|
| `u8_ManualButton` | `0x2000aea8` | uint8 | Button command injected into state machine |
| `u8_ManualChannel` | `0x200036f8` | uint8 | Active LED channel |
| `RedLevel` | `0x200036de` | uint16 | Current red channel brightness level |

---

## 9. Known Firmware Behaviour (Confirmed by Debugging)

### I2C / STM8 co-processor dependency

Diagnosis confirmed via `arm-none-eabi-addr2line` on captured PC values:

- The main loop is **not hard-stuck in init** — it runs and TIM1 interrupts fire normally
- However, `Handle_I2C_Master_TransmitReceive` consumes the main loop at runtime, waiting indefinitely for the STM8 IR co-processor at I2C address `0x50`
- `ProcessMainStateMaschine` never executes — buttons and LEDs are non-functional as a result
- **Fix:** compile-time `#define DISABLE_I2C_MASTER` guard around I2C calls (tracked as active work item)

### IRQ 27 — TIM1_CC_IRQHandler

External Interrupt 27 is TIM1 capture/compare. It fires continuously and is functioning correctly. Seeing the CPU halted inside it is normal.

### Target voltage warning

OpenOCD consistently reports `target voltage may be too low for reliable debugging` (~0.01V). This is a measurement artefact of this specific ST-Link V2 unit and does not affect flash or debug reliability. All operations succeed normally.

---

## 10. Important Rules

1. **Always rebuild before flashing.** `make clean && make` — never flash a stale binary.
2. **Always use single quotes** around OpenOCD `-c` blocks. Double quotes allow bash to expand `$variable` before OpenOCD sees it, silently breaking Tcl variable reads.
3. **`.bin` keyword is mandatory** in `flash write_image` when flashing raw binaries. Omitting it corrupts the flash operation.
4. **`arm-none-eabi-addr2line` requires the `.elf`**, not the `.bin`. The `.elf` contains debug symbols; the `.bin` does not.
5. **Re-run `arm-none-eabi-nm`** after every rebuild to get current variable addresses. They shift with any source change.
6. **Never edit USB source files.** The Samsung USB drive is read-only reference. All edits in `~/embers_fw/`.
7. **Commit before flashing experimental changes.** Always have a clean git state to roll back to.

---

## 11. Quick Reference Card

```
BUILD:    cd ~/embers_fw/MPU_V2_Basic && make clean && make
FLASH:    openocd -f interface/stlink.cfg -f target/stm32f4x.cfg \
            -c "program build/MPU_V2_Basic.bin verify reset exit 0x08000000"
RESTORE:  openocd -f interface/stlink.cfg -f target/stm32f4x.cfg \
            -c "init; halt; flash write_image erase ~/mpu_backup_original.bin 0x08000000 bin; reset run; exit"
PC NOW:   openocd -f interface/stlink.cfg -f target/stm32f4x.cfg \
            -c 'init; halt; reg pc; exit'
SYMBOL:   arm-none-eabi-nm ~/embers_fw/MPU_V2_Basic/build/MPU_V2_Basic.elf | grep "SymbolName"
ADDR2FN:  arm-none-eabi-addr2line -e build/MPU_V2_Basic.elf -f 0xADDRESS
READ VAR: openocd ... -c 'init; halt; mem2array v 8 0xADDRESS 1; echo "$v(0)"; exit'
WRITE:    openocd ... -c 'init; halt; mwb 0xADDRESS VALUE; resume; exit'
```
