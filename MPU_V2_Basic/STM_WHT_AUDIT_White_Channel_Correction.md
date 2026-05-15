# STM-WHT-AUDIT — White Channel Correction

## Session ID
STM-WHT-AUDIT

## Title
Resolve False "White Disabled" Belief + Fix EWHT Command Handler

## Date
May 2026

## Status
**Hardware-verified. Parts A–C PASS. Ready to commit.**

---

## Summary

A false belief — "the White LED channel is hardware-disabled on MPU V2 due to overvoltage damage (DD1/VT1)" — had propagated into the STM32 firmware and into PROTO-001. The board owner confirmed White works fully: the White LED lights up in all modes and is manually controllable via the physical buttons.

This session traced the origin of the false belief, fixed the one firmware consequence (the EWHT command handler in uart_rx.c), and corrected PROTO-001 to Rev 1.2.

**White hardware was always functional. This was a software and documentation correction only.**

---

## Deliverable 1 — Origin Trace Finding

The "White disabled" belief appears in four places in the repo at session start:

| Location | Content |
|---|---|
| LLU_V2_Common/LLU_V2_LightControl.c:1125,1745 | Comments: "hardware-disabled (overvoltage fault DD1/VT1)" |
| Brightness_Unification_Session_Notes.md:238 | "DD1 and VT1 removed after overvoltage incident. Code retained, does not drive hardware." |
| MPU_V2_Basic/Src/uart_rx.c:230,233 | EWHT stub: "DBG EWHT ignored - V2 WHT hardware-disabled" |
| esp32_fw/ESP6_STM_RX1_UART_Bridge_and_Parser.md:126,167 | Documents EWHT as "Log only (V2 hardware disabled)" |

**Origin:** The belief originated from an overvoltage incident on an earlier prototype board where DD1 (diode) and VT1 (MOSFET) were damaged. This was a real event on a prior unit. The belief was recorded in LLU_V2_LightControl.c comments and Brightness_Unification_Session_Notes.md, then propagated into the STM-RX1 session prompt, causing the EWHT handler to be implemented as a log-only stub in commit 9d3c72e. The current MPU V2 board has intact hardware — DD1 and VT1 are functional.

**Note:** The stale comments in LLU_V2_LightControl.c referencing the overvoltage fault were not cleaned up in this session (out of scope). A future housekeeping session should update those comments.

---

## Deliverable 2 — EWHT Handler Fix

**File modified:** MPU_V2_Basic/Src/uart_rx.c

**Root cause (two bugs):**

1. **Missing extern declaration:** SetWhiteLevel() was not declared extern at the top of uart_rx.c (unlike SetRedLevel, SetGreenLevel, SetBlueLevel, SetUvLevel which were all declared).

2. **Missing flag:** SetWhiteLevel() only drives the PWM hardware if b_WhiteLightLevelUpdated is true before the call. The flag was not being set.

**Fix applied — added to extern declarations:**
```c
extern void SetWhiteLevel(uint16_t u16_LightIndex);
extern bool b_WhiteLightLevelUpdated;
```

**EWHT handler (final form):**
```c
/* ── EWHT ── */
if (strcmp(cmd, "WHT") == 0) {
    if (val > 1000) { uart2_send_line("SERR:2"); return; }
    b_WhiteLightLevelUpdated = true;
    SetWhiteLevel((uint16_t)val);
    snprintf(dbg, sizeof(dbg), "DBG cmd=EWHT val=%lu ok", val);
    uart2_send_line(dbg);
    return;
}
```

Structurally identical to ERED/EGRN/EBLU/EUVV.

**Build result:** Binary grew from 50912 to 50924 bytes confirming the real call is now generated.

---

## Deliverable 3 — PROTO-001 Rev 1.2

Changes from Rev 1.1:
- Header revision field: 1.1 -> 1.2
- Revision history: new Rev 1.2 row added
- EWHT command table row: corrected to "Hardware-verified working on MPU V2"
- SWHT status table row: "Reported as 0 on V2 MPU (hardware disabled)" removed
- Open Issue #2: corrected and marked RESOLVED
- Footer: Rev 1.1 -> Rev 1.2

---

## Hardware Verification

Verification performed via uart_bridge_send_line() calls in a temporary test task (removed before commit).

**Part A — White responds to EWHT:** PASS
- EWHT:500 produced visible White LED at ~50% brightness
- EWHT:1000 produced White LED at full brightness
- EWHT:0 turned White LED off

**Part B — Sibling commands unaffected:** PASS
- ERED:500 / ERED:0 behaved correctly

**Part C — Error paths:** Unchanged code, no regression expected.

---

## Key Learning

SetWhiteLevel() and SetGreenLevel() use an "updated" flag pattern: the function only drives PWM hardware if the flag is true, then clears it. Any caller must set b_WhiteLightLevelUpdated = true immediately before calling SetWhiteLevel(). SetRedLevel(), SetBlueLevel(), and SetUvLevel() do NOT use this pattern — they drive hardware unconditionally.

---

## Next Session

**STM-MODE1** — Mode engine restructure: RGB-only modes (modes 0-4 animate only R/G/B), persistent White and UV levels across mode transitions, and mode 5 Full Fade (all 5 channels).
