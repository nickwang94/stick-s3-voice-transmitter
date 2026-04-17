## Why

The current project uses USB serial transmission for audio data, which has limitations including bandwidth constraints (115200 baud rate), audio quality loss, and physical cable tethering. WiFi transmission enables higher bandwidth and better audio quality while eliminating physical connection constraints.

## What Changes

- Switch from USB serial transmission to WiFi WebSocket transmission
- Use Arduino WiFi library to establish network connection
- Send audio data to PC via WebSocket binary frames
- Maintain existing UI display and button control functionality

## Capabilities

### New Capabilities

- `wifi-connection`: WiFi network connection management, including provisioning, reconnection, and status monitoring
- `websocket-client`: WebSocket client implementation supporting binary data transmission
- `audio-streaming`: Real-time audio streaming over WiFi
- `ota-config`: Support configuration updates over network

### Modified Capabilities

- `usb-serial-transmission`: Demoted from primary transmission method to debug backup

## Impact

- **Hardware**: M5 StickS3 (ESP32-S3)
- **Firmware**: Arduino framework + WiFi library + WebSocket library
- **PC End**: Requires WebSocket server to receive audio data
- **Dependencies**: WiFi.h, WebSockets.h or similar libraries
- **Network**: Requires 2.4GHz WiFi network environment
