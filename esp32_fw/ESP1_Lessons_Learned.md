# ESP-1 Session Notes — Lessons Learned

**Date:** 2026-05-08

---

## What Was Done

- Configured 8MB flash size in sdkconfig via menuconfig
- Set default log level to Info in sdkconfig via menuconfig
- Replaced hello_world_main.c with embers_main.c
- Updated main/CMakeLists.txt to reference embers_main.c
- Confirmed startup banner, MAC address, and 5-second heartbeat on hardware
- Committed and pushed to main (71945cb to f768599)

---

## Lessons Learned

**1. /tmp does not persist between Claude sessions**
Files written to /tmp during a Claude chat are gone in the next chat.
Always write session documents directly to the project directory, not to /tmp.
For future sessions: write the .md file to esp32_fw/ as the last step, before git add.

**2. cat << EOF blocks are fragile in practice**
The heredoc approach breaks if anything in the pasted content confuses the
shell or if the paste gets split. For long files, open nano first and paste
into it directly — no EOF delimiter required.

**3. minicom must already be open before reset to capture the full boot log**
The ESP32 boots faster than minicom connects. If minicom is launched after
power-on, the startup banner scrolls past before the connection is established.
Correct procedure: open minicom first, then reset the board (EN button or
power-cycle the 12V barrel).

**4. project(hello_world) was already corrected by idf.py**
The top-level CMakeLists.txt project() name was updated automatically during
the build process. No manual fix was required. Always cat the file to confirm
before spending time on a fix.

**5. Give commands one at a time**
Multi-command blocks are error-prone in practice. One command per message is
clearer and easier to recover from if something goes wrong.
