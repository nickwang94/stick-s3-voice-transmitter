## Context

The current project uses USB serial communication for audio transmission, limited to 115200 baud rate with actual throughput around 10KB/s, resulting in limited audio quality. ESP32-S3 has built-in WiFi module supporting WebSocket protocol, enabling higher quality audio streaming over wireless network.

**Constraints:**
- Using Arduino framework
- Must be compatible with existing UI and button logic
- PC end requires WebSocket server to receive audio

## Goals / Non-Goals

**Goals:**
- Use WiFi WebSocket to transmit audio data to PC
- Maintain 16kHz sample rate, 16-bit depth audio quality
- Implement stable network connection and auto-reconnect
- PC WebSocket server can receive and save audio

**Non-Goals:**
- No offline speech recognition support (still requires PC processing)
- No multi-device simultaneous connection support
- No audio compression encoding (uses raw PCM)
- No 5GHz WiFi support (2.4GHz only)

## Decisions

### 1. WebSocket Library Selection

**Decision:** Use `WebSocketsClient` library (marksuite/WebSockets)

**Rationale:**
- Can be installed via Arduino Library Manager
- Supports ESP32/ESP8266
- Supports binary data transmission
- Active community, comprehensive documentation

**Alternative:** Use ESP32 built-in `esp_websocket_client.h`
- Disadvantage: Requires ESP-IDF framework, complex to use with Arduino

### 2. Audio Data Format

**Decision:** Send raw PCM data via WebSocket binary frames

**Rationale:**
- Consistent with existing serial format, minimal PC changes
- No codec overhead, reduced latency
- WiFi bandwidth sufficient (~256kbps)

### 3. Network Configuration Method

**Decision:** Hardcode WiFi SSID and password in source code

**Rationale:**
- Simplified implementation, no additional provisioning flow
- Suitable for development phase

**Alternative:** SmartConfig or Web provisioning
- Advantage: More flexible
- Disadvantage: Increased implementation complexity, leave for future iteration

### 4. Connection Management

**Decision:** Poll WebSocket connection status in loop(), auto-reconnect on disconnection

**Rationale:**
- Compatible with existing button detection logic
- Simple implementation, low resource usage

## Risks / Trade-offs

| Risk | Mitigation |
|------|------------|
| WiFi signal instability causing disconnection | Implement auto-reconnect mechanism |
| Network latency causing audio stuttering | Use small buffer, tolerate minor packet loss |
| Battery life degradation | WiFi power consumption higher than serial, accept trade-off |
| Router compatibility | Only test common routers, users handle special cases |
| PC IP address changes | Users need to manually update IP in code |
