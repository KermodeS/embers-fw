# Brightness Table Duplicate Index Fix

**Date:** 2026-04-27  
**Branch:** `dev/firmware-refactor`  
**File modified:** `~/embers_fw/MPU_V2_Basic/Src/main.c`  
**Git commit:** `fix: correct duplicate array index writes in all 5 brightness tables`  
**Status:** ✅ Complete

---

## Background

Each of the 5 LED channels (Red, Green, Blue, UV, White) has a 256-entry brightness reference table (`u16_*Refference_MP[256]`) that maps a perceptual brightness index (0–255) to a PWM table index. These tables are initialised at startup by `InitRedLightArray_MP_V1()` and equivalent functions in `MPU_V2_Basic/Src/main.c`.

The tables are used by the animation engine and manual brightness control to step through PWM levels in a perceptually smooth, even progression across the full brightness range.

---

## The Bug

All 5 tables contained a duplicate write to array index `[133]`:

```c
u16_RedRefference_MP[133] = 569;  // first write  — correct
u16_RedRefference_MP[133] = 577;  // second write — BUG: overwrites 569 with 577
u16_RedRefference_MP[134] = 585;  // continues normally
```

At runtime the second write silently overwrites the first. The result is that:

- Index `[133]` ends up holding value **577** instead of the intended **569**
- The step from `[132]→[133]` becomes **16** instead of **8** (double the surrounding step size)
- The value **569** is permanently lost from the curve
- A subtle but measurable discontinuity exists at approximately **52% brightness** on all 5 channels

The bug was present in the original contractor firmware and carried forward into the refactor branch unchanged. It was identified during the original code review (documented in `embers_git_reference.md`) as priority 1 on the refactoring backlog.

---

## The Fix

The correct resolution was to **delete the second `[133]` write** in each channel's init block. This:

- Preserves the first write (`[133] = 569/776/776/862/862`) which is the correct curve value
- Discards the erroneous overwrite value (577/787/787/873/873) entirely
- Leaves `[134]` onward untouched — those entries were always correct

Five lines were deleted, one per channel:

| Channel | Line deleted | Value discarded |
|---------|-------------|-----------------|
| Red     | 1104        | `[133] = 577`   |
| Green   | 1420        | `[133] = 787`   |
| Blue    | 1736        | `[133] = 787`   |
| UV      | 2051        | `[133] = 873`   |
| White   | 2367        | `[133] = 873`   |

Post-fix, each table shows a clean sequential progression through the affected region with no duplicate indices.

---

## Investigation Path

Locating the bug required several steps because the tables are not in the expected file:

- The `extern` declarations for `u16_*Refference_MP[]` are in `LLU_V2_Common/LLU_V2_LightControl.c`
- The actual definitions and init functions are in `MPU_V2_Basic/Src/main.c` (along with all other firmware logic for the MPU target)
- The tables are populated by explicit individual assignment statements, not loops — one line per index, ~256 lines per channel

The duplicate was found by grepping all 5 table names and scanning for repeated index numbers in the output.

---

## Verification

- Build: clean, no new warnings introduced
- Flash: verified OK via OpenOCD
- Hardware: brightness sweep 0–100% on all 5 channels showed continuous ramp behaviour consistent with pre-existing baseline
- The discontinuity was one step out of 256 (~0.4% of range) and not visually distinguishable from correct behaviour without instrumentation — sign-off on code correctness rather than perceptual test

---

## Notes

- The same duplicate exists in the reference archives (`TIM_PWMOutput/Src/main.c`, `MPU_V2_Basic/Src/main_production.c`, `main_18_March_2023_13-23.c`) but those files are not compiled. No action required on them.
- The `LPU` and `HPU` variants have their own equivalent tables in separate target directories and were not audited in this session — they likely contain the same bug but are out of scope until those targets are active.
- No other code was modified. Additive-only constraint maintained (this was a deletion of erroneous lines, not a refactor).
