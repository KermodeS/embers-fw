# Embers Lighting — Feature & Development Roadmap

> 🔴 = Requires V3 PCB modification

---

## Foundation

1. Complete STM32 firmware refactor, merge dev/firmware-refactor to main

2. Fix duplicate index bug in brightness lookup tables (firmware, doable now)

3. Formalise the UART command protocol between ESP32 and STM32 as a written specification

---

## ESP32 Application Firmware (clean build)

4. WiFi station mode — connects to home router

5. Captive portal provisioning — first-boot setup page, user enters WiFi credentials

6. mDNS device announcement — devices reachable at embers-1.local etc.

7. WebSocket server — real-time bidirectional communication with web UI

8. ESP-NOW inter-device sync — replaces old UDP broadcast, low-jitter sync between devices

9. Hub role — one device discovers peers, relays commands via ESP-NOW

10. UART driver to STM32 — ESP32 sends commands, receives state from STM32

11. Credential and settings persistence — survives power cycles

---

## Web UI / PWA

12. Captive portal setup page — WiFi network selection and password entry

13. Per-channel sliders — R, G, B, UV individual brightness control

14. Global brightness ceiling control

15. Mode selector — Manual, Fade, Strobe, Rainbow, Sequential

16. Animation timing controls — fade duration, strobe rate etc.

17. Multi-device discovery and display — all Embers devices shown in one UI

18. Group control — control all devices simultaneously

19. Per-device individual control

20. Username/password protection — local network access control

21. PWA manifest — add to home screen on iOS/Android, works like native app

22. QR code on device — scans directly to device UI, no typing required

23. Scene saving — user saves favourite channel combinations as named presets

24. Scheduler — run scenes at set times automatically

---

## OTA Updates

25. ESP32 OTA — hub checks your server for firmware updates, downloads and installs automatically

26. 🔴 STM32 OTA via ESP32 — ESP32 flashes STM32 over UART using built-in bootloader, eliminates need for physical ST-Link for field updates

---

## Advanced UI Features

27. Music sync — browser uses device microphone via Web Audio API, sends brightness commands to hub over WebSocket

28. Screen/TV sync — browser samples screen colours, maps to light channels

29. Visual program designer — drag-and-drop timeline or node editor, user designs custom light sequences without coding, stored as JSON

30. Program sharing — users export/import sequence programs as files

---

## Telemetry & Analytics

31. Anonymous usage analytics — which modes/features are used, sent to your server periodically

32. Fleet dashboard — your server-side view of usage patterns across all devices

---

## LED Health Monitoring

33. 🔴 Current sense resistors per channel on V3 PCB — enables actual current measurement per LED channel

34. 🔴 Forward voltage monitoring per channel — tracks LED junction degradation over time

35. LED wear detection algorithm — STM32 compares measured values against baseline, flags degradation

36. Proactive replacement notification — web UI alerts user when a channel is wearing out

37. Replacement revenue path — notification links directly to purchasing a new LED board

---

## Gamma & Brightness Quality

38. 🔴 V3 current-feedback loop — eliminates open-loop PWM, fixes DCM nonlinearity, resolves Red channel discontinuity at hardware level

39. Mathematical gamma table generation — replace hand-crafted lookup tables with CIE 1931 formula, eliminates duplicate index bug class permanently

40. Post-V3 lux calibration session — validate gamma curves with lux meter on new hardware, derive any per-channel offsets

---

## Infrastructure

41. Firmware file server — static host (GitHub Releases or cheap VPS) serving version manifest and firmware binaries for OTA

42. Telemetry backend — serverless function accepting device data, writing to database

43. Privacy policy and opt-out — required for any telemetry collection
