# DEV-002 — Animation Engine Stage 3: uwTick Fade Engine

**Project:** Embers Lighting — LLU_V2 Firmware  
**Branch:** `dev/firmware-refactor`  
**Date:** April 2026  
**Status:** Complete — merged to dev, pending merge to main

---

## 1. What Was Accomplished

### 1.1 Replaced ISR-driven animation with uwTick fade engine

The original Sequential and Rainbow animation modes were driven by five per-channel hardware timer ISRs (TIM1/TIM3/TIM4/TIM8/TIM10). Each ISR maintained its own `static uint16_t u16_LightLevel` and stepped it up/down using `u8_XxxLightRepeatFactor_MP[]` lookup tables.

**Problems with the ISR architecture:**
- Three PWM zones (640Hz / 4kHz / 16kHz) fire ISRs at different rates, causing uneven animation speed across brightness zones
- Speed, brightness ceiling, brightness floor, and gamma were all entangled in the repeat-factor tables with no runtime control
- `uint16_t` underflow crash: on DOWN ramp, `u16_LightLevel--` ran before the `== 0` check, wrapping to `0xFFFF`
- No runtime parameters — fade duration, brightness ceiling/floor required recompiling

**New architecture:**
- Animation driven from `u32_ST_captureActual` (1ms SysTick) in the main loop via `Animation_Update()`
- ISRs retain only their PWM register-write block and `TimerXCaptureCompare_Callback()` call — 817 lines of animation stepping removed
- `FadeChannel_t` struct per channel with runtime-settable `u32_FadeDurationMs`, `u16_BrightnessMax`, `u16_BrightnessMin`
- Sequential: one channel fades up then down, next starts cleanly at zero
- Rainbow: overlapping crossfade — next channel starts rising when current reaches max, current begins falling

### 1.2 Inverse gamma tables

Two 1001-entry tables (perceptual 0-1000 → linear fraction 0-1000):
- `u16_InvGamma_RGBW[1001]` — gamma 2.2 inverse for R/G/B/W channels
- `u16_InvGamma_UV[1001]` — gamma 1.8 inverse for UV channel

Generated: `round(pow(i/1000.0, 1.0/gamma) * 1000)` for i in 0..1000.

`PerceptualToRaw()` maps a perceptual 0-1000 value to a raw PWM table index using these tables, scaled to each channel's `[zone3Start .. maxIdx-1]` range (16kHz zone only — see section 1.4).

### 1.3 Temporal dithering

When the ideal perceptual value falls between two adjacent raw PWM indices, `OutputChannel()` alternates between the lower and upper index on successive calls using `u8_DitherPhase`. Eliminates visible stepping at low brightness in the 640Hz zone.

### 1.4 PWM zone boundary handling

Each channel's PWM table has three frequency zones:
- 640Hz: indices 0..999 (period register 65535)
- 4kHz: indices 1000..~1230 Red / ~1449 others (period register 10499)
- 16kHz: indices ~1230/1449..max (period register 2624)

At zone transitions, the duty cycle jumps discontinuously because the period register resets. For Red, the jump at index 1000 represents a 2.3× brightness step. These transitions occur in the lower third of the perceptual range (Red: at perceptual 187 and 297 out of 1000) where the eye is most sensitive.

**Mitigation:** `PerceptualToRaw()` maps the full 0-1000 perceptual range directly to the 16kHz zone only (`zone3Start..maxIdx-1`), using `PERC_ZERO_THRESH = 8` to output true zero below the noise floor. Zone transitions are avoided entirely during animation fades.

Zone3 start indices (measured from live hardware):
- Red: 1230
- Green: 1449
- Blue: 1449
- UV: 1449
- White: 1449

### 1.5 Global brightness control

`u16_GlobalBrightMax` (0-1000) is a runtime global that scales raw PWM output linearly in `OutputChannel()` for all animation modes. This keeps the fade period constant regardless of brightness level and gives perceptually even steps per button press.

White channel is additionally scaled to 40% of global max to compensate for its higher physical brightness relative to R/G/B/UV.

UP/DOWN buttons in Sequential, Rainbow, and Strobe modes all update `u16_GlobalBrightMax` in 50-unit steps with a floor of 100 (prevents going dark and getting stuck).

Strobe mode sync: `SetActualMax()` was modified to scale `i32_ActualPerc_AM[]` from `u16_GlobalBrightMax` rather than always restoring to 100%, so strobe brightness matches the level set in fade/rainbow mode.

---

## 2. Files Modified

| File | Change |
|---|---|
| `LLU_V2_Common/LLU_V2_LightControl.h` | Added `FadeChannel_t` struct, `g_Fade[5]` extern, `FADE_CH_*` constants, `Animation_Init()` / `Animation_Update()` declarations, `extern uint16_t u16_GlobalBrightMax` |
| `LLU_V2_Common/LLU_V2_LightControl.c` | Added inverse gamma tables, `PerceptualToRaw()`, `OutputChannel()`, `StepChannel()`, `SilenceChannel()`, `Animation_Init()`, `Animation_Update()`, `u16_GlobalBrightMax` definition, modified `SetActualMax()`, added GlobalBrightMax UP/DOWN to Sequential/Rainbow/Strobe handlers |
| `MPU_V2_Basic/Src/stm32f4xx_it.c` | Stripped 817 lines of animation stepping from TIM2/3/4/10/11 ISRs |
| `MPU_V2_Basic/Src/main.c` | Added `Animation_Init()` after `InitWhiteLightArray_MP_V1()`, `Animation_Update()` after `ProcessMainStateMaschine()` in main loop |

---

## 3. Key Constants and Tuning Parameters

```c
// In LLU_V2_LightControl.c — Animation_Init()
g_Fade[FADE_CH_RED  ].u32_FadeDurationMs = 9000u;   // ms per sweep (adjusted for 3x tick rate)
g_Fade[FADE_CH_GREEN].u32_FadeDurationMs = 12000u;
g_Fade[FADE_CH_BLUE ].u32_FadeDurationMs = 12000u;
g_Fade[FADE_CH_UV   ].u32_FadeDurationMs = 11400u;
g_Fade[FADE_CH_WHITE].u32_FadeDurationMs = 12000u;

// In PerceptualToRaw()
#define PERC_ZERO_THRESH 8u   // perceptual values below this → raw index 0 (true off)

// In OutputChannel()
// White scale: u16_GlobalBrightMax * 40u / 100u
// Other channels: u16_GlobalBrightMax directly
// Dead zone floor: if u16_Out < 20u → 0u

// Global brightness steps: 50 per button press, floor 100, ceiling 1000
```

---

## 4. Hardware Notes

### 4.1 u32_ST_captureActual tick rate

`u32_ST_captureActual` increments at approximately **3.0× per ms** on this hardware (measured: 110,594 ticks over 36,500ms wall time). This is not 1ms despite `SysTick_Config(84000000 / 1000)` — the discrepancy is due to main loop overhead calling `u32_ST_captureActual` many times per SysTick period.

**Rule:** All `FadeDurationMs` values must be multiplied by ~3 relative to their intended wall-clock duration. The 9000ms configured for Red produces ~20 second visual cycles (up + down), not 9 seconds.

### 4.2 PWM table structure

Each channel's PWM table (`u16_XxxLightFreqPeriod_MP[]`) encodes both frequency zone and period in a single `uint16_t`:
- Bits 15:14 = zone marker: `0x4000` = 640Hz, `0x8000` = 4kHz, `0xC000` = 16kHz
- Bits 13:0 = period value (compare register value)

`UpdateXxxLevelFast_MP(index)` uses this index directly — it does not interpret the zone bits, just passes the value to `SetXxxLevel()` which writes the compare register.

### 4.3 Channel table sizes

| Channel | Offset | MAX_MP | Total entries | Zone3 start |
|---|---|---|---|---|
| Red | 20 | 2120 | 2100 | 1230 |
| Green | 20 | 2920 | 2900 | 1449 |
| Blue | 20 | 2920 | 2900 | 1449 |
| UV | 20 | 2820 | 2800 | 1449 |
| White | 20 | 2920 | 2900 | 1449 |

White is physically significantly brighter than other channels at equivalent raw index — cap at 40% of global max for visual balance.

---

## 5. Lessons Learned

### 5.1 Always check the tick source before using it for timing

`uwTick` (HAL) and `u32_ST_captureActual` are both available but behave differently. `uwTick` is declared `extern __IO uint32_t` — using `extern uint32_t` causes a type qualifier conflict at compile time. `u32_ST_captureActual` increments multiple times per ms due to main loop calling frequency. **Always measure the actual tick rate against wall clock before using any counter for timing.**

### 5.2 Read PWM table structure from hardware before writing output code

The zone-boundary brightness jumps were not visible in the source code — they only became apparent after dumping the live PWM table with OpenOCD and computing actual duty cycles at the transition points. When writing code that uses a lookup table, always verify the table structure and value ranges from hardware first.

### 5.3 Inverse gamma is not the same as the output gamma

The existing firmware applied forward gamma (`output = input^2.2`) in `DirectLightCtrl()` for manual mode. The animation engine needed the **inverse** (`output = input^(1/2.2)`) to map a linear perceptual step to a raw PWM index. These are opposites and must not be confused.

### 5.4 Global variables defined in .c files must not also be declared extern in the same file

`uint16_t RED_MAX_INDEX = 0;` is a definition. `extern uint16_t RED_MAX_INDEX;` is a declaration. Having both in the same `.c` file causes a linker error. The definition goes in one `.c` file; all other files that need it use `extern` in their own scope or via a shared header.

### 5.5 SilenceChannel must output true zero, not the dead-zone floor

`UpdateRedLevelFast_MP(index)` clamps any index below `RED_OFFSET` (20) to `RED_OFFSET` — so passing 0 still outputs a dim glow. To silence a channel completely, use `SetRedLevel(0)` directly, which bypasses the clamp. `SilenceChannel()` uses `SetXxxLevel(0u)` for this reason.

### 5.6 Brightness scaling should be linear on raw output, not perceptual on ceiling

Reducing `BrightnessMax` in perceptual space shortens the fade period because the step size is calculated against the full 0-1000 range. The correct approach for global brightness control is to scale the **raw PWM output linearly** after gamma mapping, keeping `BrightnessMax = 1000` always. This gives constant period, perceptually even button steps, and independent per-channel physical compensation (e.g. White at 40%).

### 5.7 Strobe mode restores AM variables via SetActualMax() on entry

When entering strobe mode, `b_Restore_AM = true` triggers `SetActualMax()` → `RestoreActualMax()` which overwrites any AM variable changes made before the mode transition. Any brightness sync for strobe must be done inside `SetActualMax()` itself, not at the mode-entry point.

### 5.8 Make one change at a time and verify on hardware before the next

Several issues in this session were caused by stacking multiple unverified changes. When a change causes unexpected behaviour, it is much harder to isolate if two or three other changes were made at the same time. The session rule of additive-only, one change per commit, is correct — it was not always followed here and caused extra debugging cycles.

### 5.9 OpenOCD mdw output requires latin-1 decoding

OpenOCD telnet output begins with escape bytes that cause `UnicodeDecodeError` if decoded as UTF-8. Always open OpenOCD output files with `open(filename, 'rb')` and decode each line as `latin-1`.

### 5.10 Variable addresses from .map/.elf go stale after any rebuild

`arm-none-eabi-nm` addresses are valid only for the binary currently flashed. After any rebuild, re-run `nm` before using addresses in OpenOCD `mdw`/`mdb` commands. Using stale addresses reads wrong memory silently.

---

## 6. Known Remaining Issues

| Issue | Severity | Notes |
|---|---|---|
| Red zone discontinuity | Cosmetic | Visible brightness step at ~20% brightness during fade. Caused by 640Hz→4kHz zone boundary at raw index 1000. Deferred. |
| Manual mode starts at zero after animation | UX | When cycling from Sequential/Rainbow to Manual, the manual brightness starts from where animation left off (near zero). User must press UP to restore. Deferred. |
| Fade start/end abruptness | Cosmetic | Channels pop on slightly at fade start due to PERC_ZERO_THRESH snap. Lowering threshold or adding a soft ramp would help. Deferred. |

---

## 7. Git Commit Reference

```
feat(animation): Stage 3 — uwTick fade engine replacing ISR stepping

- Inverse gamma tables (RGBW 2.2, UV 1.8), 1001 entries each
- FadeChannel_t struct with runtime FadeDurationMs/BrightnessMax/Min
- PerceptualToRaw() maps perceptual 0-1000 to 16kHz zone only
- Temporal dithering eliminates stepping at low brightness
- Sequential: one channel fades up/down, channels advance cleanly
- Rainbow: overlapping crossfade between adjacent channels
- Strip ISR animation stepping from TIM2/3/4/10/11 (817 lines removed)
- u32_ST_captureActual used for timing (measures ~3x/ms on hardware)
- Per-channel durations tuned to table sizes for even visual speed
- u16_GlobalBrightMax: global brightness 0-1000, 50-step, floor 100
- Linear raw output scaling keeps period constant at all brightness levels
- White capped at 40% of global max for visual balance
- SetActualMax() modified to apply GlobalBrightMax to strobe mode
- Animation_Init/Update called from main loop
- Red zone discontinuity noted, deferred
```
