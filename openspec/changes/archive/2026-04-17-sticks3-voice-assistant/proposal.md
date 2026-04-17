## Context

M5 StickS3 is a compact ESP32-S3 development board with integrated ES8311 audio codec, microphone, and 1.14-inch color display. The project goal is to implement audio capture and transmit to PC via USB serial for further processing.

**Constraints:**
- Using Arduino framework and M5Unified library
- 115200 baud rate serial transmission
- 16kHz sample rate, 16-bit sample depth
- Screen visible area 130x240 pixels

## Goals / Non-Goals

**Goals:**
- Use M5.Mic API for audio capture
- Send raw PCM data to PC via USB serial
- Display current status on screen (Idle/Listening)
- Use Button A to start/stop capture
- PC saves received data as WAV file

**Non-Goals:**
- On-board speech recognition (recognition done on PC)
- WiFi/Bluetooth wireless transmission
- Audio compression encoding (uses raw PCM)
- Multi-button control or complex UI

## Decisions

### 1. Audio Capture API Selection

**Decision:** Use M5Unified's `M5.Mic` API instead of direct I2S driver manipulation

**Rationale:**
- M5Unified handles complex ES8311 Codec initialization
- Avoids pin conflicts with display driver
- Simplifies code, improves maintainability

**Alternative:** Use ESP-IDF's `i2s_driver_install()` directly
- Disadvantage: Requires manual Codec configuration, prone to conflicts with display driver

### 2. Data Transmission Method

**Decision:** Use serial port to send raw PCM data

**Rationale:**
- Simple and direct, no codec overhead
- 115200 baud rate sufficient for 16kHz audio
- Flexible PC-side processing

**Alternative:** Transmit after Opus encoding
- Advantage: Lower bandwidth
- Disadvantage: Requires encoder integration, increased complexity

### 3. UI Design

**Decision:** Minimal UI - status text + Logo + battery

**Rationale:**
- Limited screen size (130x240)
- Reduce refresh frequency, minimize interference with audio capture

## Risks / Trade-offs

| Risk | Mitigation |
|------|------------|
| Serial transmission latency causing audio stuttering | Use small buffer (128 samples) for continuous reading |
| Low battery causing performance degradation | Display battery icon, low battery warning |
| Button response delay | Prioritize button detection in loop() |
| Raw PCM high bandwidth usage | Can upgrade to Opus encoding in future |
