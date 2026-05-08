# Brightness Unification, Gamma Alignment & Animation Bug Fixes
## Session Notes, Lessons Learned, Architecture Decisions

**Branch:** `dev/firmware-refactor`  
**Files modified:** `LLU_V2_Common/LLU_V2_LightControl.c`, `MPU_V2_Basic/Src/stm32f4xx_it.c`, `LLU_V2_Common/LLU_V2_LightControl.h`

---

## What We Accomplished

### 1. Manual Mode Brightness Restore on Return from Animation
**Bug:** When returning to Manual mode from any animation mode (Strobe, Rainbow, Fade), the LED went dark and required Up/Down button press to restore brightness.

**Root cause:** `TurnOffPattern()` zeros `u32_LightLevel*` hardware counters. No code re-drove the hardware when re-entering Manual.

**Fix:** In the `MANUAL_MODE_BUTTON_MANUAL` branch of `ProcessMainStateMaschine()`, use a fast non-blocking restore: `PerceptualToRaw()` + `Update*LevelFast_MP()` for the active channel immediately after setting `u8_StateMaschine = SM_MODE_MANUAL`. This replaced the original `DirectLightCtrl()` call which blocked for up to 3 seconds.

**Key insight:** `i32_ActualPerc[]` is the correct source of truth — updated on every Up/Down press in Manual. `i32_ActualPerc_AM[]` is NOT reliable — only updated on channel button press.

---

### 2. Unified Global Brightness Ceiling
**Requirement:** One brightness setting controls all modes. If user sets 50% in Manual, all animation modes respect that 50% ceiling. Changes in any mode propagate to all others.

**Architecture:** `u16_GlobalBrightMax` (0–1000) is the single source of truth.

**Sync points added:**
- **Manual Up/Down** → `u16_GlobalBrightMax = i32_ActualPerc[channel] * 10` (direct assignment, not increment)
- **Animation Up/Down** → `i32_ActualPerc[all channels] = u16_GlobalBrightMax / 10`
- **Manual → Animation transition** → `u16_GlobalBrightMax = i32_ActualPerc[u8_ManualChannel] * 10` at the transition point
- **Animation_Update() every tick** → `g_Fade[ch].u16_BrightnessMax = u16_GlobalBrightMax`
- **UpdateStrobeRawIndices()** → called whenever `u16_GlobalBrightMax` changes

**Critical startup issue:** `u16_GlobalBrightMax` initialises to 1000 but `i32_ActualPerc[]` initialises to 0. Manual Up/Down must use direct assignment, not increment, to keep them in lockstep from the start.

---

### 3. Gamma Curve Unification — All Modes Same Perceptual Curve
**Bug:** Manual mode was visually dimmer than animation modes at the same nominal brightness. At click 7 (70%), Manual produced ~48 lux while Fade produced ~450 lux.

**Root causes (multiple, layered):**

#### 3a. Two competing gamma systems
- Manual used `GammaCorrectIndex()` with `u16_GammaTable_GREEN[101]`
- Fade engine used `PerceptualToRaw()` with `u16_InvGamma_RGBW[1001]`
- Different curves producing different lux at equivalent settings

**Fix:** Replace `GammaCorrectIndex()` in `DirectLightCtrl()` with `PerceptualToRaw()`. Input is `i32_Perc * 10` to convert 0–100 Manual scale to 0–1000 Fade scale. Ramp calls `Update*LevelFast_MP()` directly.

#### 3b. `SetGreenLevel()` silently drops updates
`SetGreenLevel()` only executes if `b_GreenLightLevelUpdated` flag is set by the TIM3 interrupt. At low PWM frequencies the flag may not be ready within the 1ms ramp delay.

**Fix:** Force `b_*LightLevelUpdated = true` before every `Update*LevelFast_MP()` call in `DirectLightCtrl()` ramp loops.

#### 3c. Fade engine double-scaling
`OutputChannel()` applied `u16_Out * u16_GlobalBrightMax / 1000` on top of `PerceptualToRaw()` already incorporating the ceiling — causing double-dimming.

**Fix:** Remove the post-scale from `OutputChannel()`.

#### 3d. `Animation_Update()` resetting BrightnessMax every tick
```c
for (ch = 0; ch < 5; ch++) g_Fade[ch].u16_BrightnessMax = 1000u;
```
Overwrote every brightness sync on every tick.

**Fix:** Replace `1000u` with `u16_GlobalBrightMax`.

#### 3e. Strobe driven from timer interrupt with wrong raw index
Active strobe runs in `stm32f4xx_it.c` (not `Process_Mode_Stroboscope()` which is dead code). It used `i16_LightLevelGreen_AM` from the linear formula, not `PerceptualToRaw()`. `PerceptualToRaw()` is static — not callable from interrupt files.

**Fix:** Pre-compute `u16_StrobeRaw_*` globals via `UpdateStrobeRawIndices()`, updated whenever `u16_GlobalBrightMax` changes. Used in interrupt handler.

#### 3f. IWDG watchdog timeout in ramp loops
`DirectLightCtrl()` ramps up to ~2900 steps at 1ms each = ~2.9 seconds, causing watchdog resets near the 4-second timeout.

**Fix:** Kick IWDG (`IWDG->KR = 0xAAAA`) inside every ramp loop iteration.

---

### 4. RestoreActualMax() Gamma Alignment (Item 1)
**Bug:** `RestoreActualMax()` used the linear formula `i16_G_10perc * percent / PRCNT_SHIFT` to set `i16_LightLevel*_AM`, producing different brightness than `PerceptualToRaw()` at equivalent settings.

**Fix:** Replaced with `PerceptualToRaw((uint16_t)(i32_ActualPerc_AM[ch] * 10), MAX_INDEX, isUV, zone3)` for all channels. Now consistent with Fade engine output.

---

### 5. Animation Bugs Fixed

#### 5a. Rainbow/Fade/Sequential going dark after rapid Down presses
**Root cause:** Rapid Down presses force `i32_PercX10` to 0 with `u8_Dir = 0` (fading down). Channel locks at zero permanently — no code restarts it upward. Also, `u16_GlobalBrightMax` floor was too low (100), causing `PerceptualToRaw()` to output near-zero raw indices.

**Fix:**
- Raised `u16_GlobalBrightMax` floor from 100 to 200 in all animation Down handlers
- Added channel recovery block after each Down handler: if `i32_PercX10 <= 0` and `u8_Dir == 0`, reset `u8_Dir = 1` and `u32_SubStepAccum = 0` so the channel rejoins the animation

#### 5b. Fade cycle time faster at lower brightness (constant duration fix)
**Root cause:** `StepChannel()` used a fixed step size calibrated for the full 0–10000 range regardless of `u16_BrightnessMax`. At 20% ceiling, the 0–2000 range was covered in 20% of `FadeDurationMs`.

**Root problem with naive fix:** `MaxX10 * elapsed / duration` truncates to 0 at low brightness and short elapsed times (e.g. `2000 * 2 / 12000 = 0`).

**Fix:** Fractional accumulator `u32_SubStepAccum` (×1000 scale) in `FadeChannel_t`. Accumulates `MaxX10 * elapsed * 1000 / duration` per tick, only advancing `i32_PercX10` when accumulator ≥ 1000. This avoids integer truncation while maintaining correct proportional timing at any ceiling.

#### 5c. Unequal channel cycle times
Red was 9000ms, UV was 11400ms, others 12000ms — causing different visual durations per color.

**Fix:** All channels set to 12000ms. Future: web app will expose this as a single user-settable parameter.

#### 5d. TurnOffPattern() blocking mode transitions
`TurnOffPattern()` ramped all channels down with 1ms per step — blocking the main loop for up to ~300ms on mode switch, causing button presses to be missed.

**Fix:** Made instant — zeros all hardware counters and calls `Set*Level(0)` immediately. Trade-off: no fade-out animation on mode switch.

---

## Architecture Reference

### Brightness Data Flow

```
User presses Up/Down
        │
        ├─ In Manual ──► i32_ActualPerc[ch] += 10
        │                u16_GlobalBrightMax = i32_ActualPerc[ch] * 10  ← direct assign
        │                g_Fade[*].u16_BrightnessMax = u16_GlobalBrightMax
        │                UpdateStrobeRawIndices()
        │
        └─ In Animation ► u16_GlobalBrightMax += 50 (or -= 50)
                          i32_ActualPerc[all] = u16_GlobalBrightMax / 10
                          g_Fade[*].u16_BrightnessMax = u16_GlobalBrightMax
                          UpdateStrobeRawIndices()

Mode transition (any → animation):
        u16_GlobalBrightMax = i32_ActualPerc[u8_ManualChannel] * 10
        SetActualMax() or b_Restore_AM = true

Mode transition (animation → Manual):
        Fast restore: PerceptualToRaw(i32_ActualPerc[u8_ManualChannel]*10)
                    → Update*LevelFast_MP() — non-blocking, instant

Animation_Update() every tick:
        g_Fade[ch].u16_BrightnessMax = u16_GlobalBrightMax  ← keeps in sync

Hardware output:
        Manual:           DirectLightCtrl() → PerceptualToRaw(i32_Perc*10) → Update*LevelFast_MP()
        Fade/Seq/Rainbow: OutputChannel() → PerceptualToRaw(i32_PercX10/10) → Update*LevelFast_MP()
        Strobe:           stm32f4xx_it.c → Set*Level(u16_StrobeRaw_*)
```

### Key Variables
| Variable | Range | Purpose |
|---|---|---|
| `u16_GlobalBrightMax` | 0–1000 | Single source of truth for brightness ceiling |
| `i32_ActualPerc[5]` | 0–100 | Manual mode per-channel brightness (10-step resolution) |
| `g_Fade[ch].u16_BrightnessMax` | 0–1000 | Fade engine ceiling — set from GlobalBrightMax every tick |
| `g_Fade[ch].u32_SubStepAccum` | 0–999 | Fractional step accumulator for constant-duration timing |
| `u16_StrobeRaw_*` | 0–MAX_INDEX | Pre-computed raw indices for strobe interrupt |
| `i32_ActualPerc_AM[5]` | 0–100 | Animation amplitude — only updated on channel button press, NOT brightness changes |

### FadeChannel_t struct (relevant fields)
```c
typedef struct {
    uint32_t u32_FadeDurationMs;   // ms for full sweep — all channels 12000ms
    uint16_t u16_BrightnessMax;    // perceptual ceiling 0-1000, set from GlobalBrightMax
    uint16_t u16_BrightnessMin;    // perceptual floor 0-1000
    uint32_t u32_LastTickMs;       // uwTick at last update
    int32_t  i32_PercX10;          // current perceptual level × 10
    uint8_t  u8_Dir;               // 1=fading up, 0=fading down
    uint8_t  u8_DitherPhase;       // temporal dithering toggle
    uint32_t u32_SubStepAccum;     // fractional step accumulator ×1000
} FadeChannel_t;
```

---

## Lessons Learned — Do Not Rediscover These

### L1: Two separate brightness systems existed
`i32_ActualPerc[]` (0–100, Manual) and `u16_GlobalBrightMax` (0–1000, animations) were completely independent. Always check both when debugging brightness mismatches.

### L2: `i32_ActualPerc_AM[]` is NOT the same as `i32_ActualPerc[]`
`i32_ActualPerc_AM[]` is only updated on **channel button** press, not on brightness Up/Down. Using `_AM` as brightness source causes stale values.

### L3: `SetGreenLevel()` silently drops updates — use `Update*LevelFast_MP()`
`SetGreenLevel()` requires `b_GreenLightLevelUpdated` flag from the timer interrupt. Force the flag before each call in blocking ramp loops. `UpdateGreenLevelFast_MP()` calls `SetGreenLevel()` internally — flag issue still applies.

### L4: `Process_Mode_Stroboscope()` is dead code
The function has `return;` at line 2. Active strobe runs in `stm32f4xx_it.c` timer interrupt. Never patch `Process_Mode_Stroboscope()` expecting it to affect strobe behaviour.

### L5: `PerceptualToRaw()` is static — not callable from interrupt files
Use pre-computed global variables (`u16_StrobeRaw_*`) updated by `UpdateStrobeRawIndices()` whenever the ceiling changes.

### L6: `Animation_Update()` runs every main loop tick — overwrites BrightnessMax
Any value set in `g_Fade[ch].u16_BrightnessMax` outside `Animation_Update()` will be overwritten next tick. The update must happen inside `Animation_Update()` itself.

### L7: `OutputChannel()` must NOT apply additional brightness scaling
`PerceptualToRaw()` already maps the perceptual ceiling via `u16_BrightnessMax`. Any additional `* u16_GlobalBrightMax / 1000` post-scale causes double-dimming.

### L8: Manual Up/Down must use direct assignment for `u16_GlobalBrightMax`
`u16_GlobalBrightMax` initialises to 1000. Using `+= 50` on Up press when Manual starts at 0% never reduces from 1000. Use `= i32_ActualPerc[ch] * 10` (direct assignment).

### L9: Block comments inside block comments break compilation
Never put `/* */` inside an existing `/* */` block. Use `//` line comments inside outer block comment regions.

### L10: `u32_LightLevel*` is not always zero after animation modes
Rainbow and Sequential drive hardware via `Set*Level()` without updating `u32_LightLevel*`. Always zero the active channel counter before calling `DirectLightCtrl()` on manual restore.

### L11: IWDG must be kicked inside long ramp loops
`DirectLightCtrl()` can take up to ~3 seconds for a full range ramp. IWDG timeout is 4 seconds. Combined with other processing this crosses the threshold. Kick `IWDG->KR = 0xAAAA` every ramp step.

### L12: Measurement methodology for animation brightness
Fade ramps over 12 seconds. Always measure at the visual peak. The lux meter is the ground truth — visual estimation is insufficient for debugging brightness differences.

### L13: `stm32f4xx_it.c` contains non-UTF-8 bytes
Open with `encoding='latin-1'` in Python, write back with same encoding. Standard `open()` raises `UnicodeDecodeError`.

### L14: Naive step scaling truncates to zero at low brightness
`MaxX10 * elapsed / duration` with small MaxX10 and small elapsed truncates to 0 at low brightness levels (e.g. `2000 * 2 / 12000 = 0`). Use a fractional accumulator (×1000 scale) to accumulate sub-unit steps across ticks.

### L15: Fade channels can get stuck dark after rapid Down presses
When `i32_PercX10` reaches 0 with `u8_Dir = 0`, no code naturally restarts the channel upward. After any brightness floor clamp, check all channels and reset `u8_Dir = 1` for any that are stuck at zero-going-down.

### L16: Button debounce requires ~500ms hold
`ProcessButton_Mode()` uses a software debounce state machine requiring `KBD_SM_FIRST_DOWN_TIME` (500ms) hold before registering. Rapid taps will be missed. This is intentional hardware debounce design — do not mistake it for a firmware bug.

### L17: `TurnOffPattern()` was blocking — now instant
The old gradual ramp-down blocked the main loop during mode transitions. Now instant. If a fade-out effect is needed in future, implement it non-blocking (flag + main loop state) rather than as a blocking loop.

### L18: `RestoreActualMax()` linear formula mismatch
The old `i16_G_10perc * percent / PRCNT_SHIFT` formula produces different raw indices than `PerceptualToRaw()` at the same percentage. Always use `PerceptualToRaw()` when setting `i16_LightLevel*_AM` to keep hardware output consistent with the Fade engine.

---

## Known Limitations (Future Work)

- **Manual brightness resolution:** `i32_ActualPerc[]` uses 0–100 (10 steps), animations use 0–1000. Upgrading requires WiFi/ESP32 protocol update (currently sends 0–10 values). Deferred.
- **Brightness not persisted across power cycles:** `u16_GlobalBrightMax` and `i32_ActualPerc[]` initialise from code defaults on boot. No flash/BIOS save implemented. Users will always start at defaults after a true power cycle (without ST-LINK connected).
- **Strobe flag reliability:** Strobe uses `SetGreenLevel(u16_StrobeRaw_GREEN)` which goes through the `b_GreenLightLevelUpdated` flag mechanism. Monitor in production testing.
- **White channel hardware-disabled:** DD1 and VT1 removed after overvoltage incident. Code retained, does not drive hardware.
- **Button debounce 500ms hold:** Intentional. Reducing requires adjusting `KBD_SM_FIRST_DOWN_TIME` in KBD state machine — not done to avoid unintended side effects.
- **`i32_ActualPerc_AM[]` stale:** Still only updated on channel button press. Not causing visible bugs since strobe uses `u16_StrobeRaw_*` directly and Fade uses `OutputChannel()`. Monitor if new code paths use `i32_ActualPerc_AM[]` for output.
