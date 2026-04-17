## 1. Environment Setup

- [ ] 1.1 Install Arduino WebSockets library (marksuite/WebSockets)
- [ ] 1.2 Install Arduino WiFi library (ESP32 built-in)
- [ ] 1.3 Configure WiFi SSID and password
- [ ] 1.4 Configure WebSocket server address

## 2. WiFi Connection Implementation

- [ ] 2.1 Add WiFi.h header
- [ ] 2.2 Implement WiFi connection function
- [ ] 2.3 Implement WiFi disconnection detection
- [ ] 2.4 Implement auto-reconnect logic
- [ ] 2.5 Add WiFi status indication (serial logs)

## 3. WebSocket Client Implementation

- [ ] 3.1 Add WebSocketsClient.h header
- [ ] 3.2 Initialize WebSocket client
- [ ] 3.3 Implement WebSocket connection function
- [ ] 3.4 Implement WebSocket event callbacks (connected, disconnected, error)
- [ ] 3.5 Implement auto-reconnect on disconnection

## 4. Audio Streaming Implementation

- [ ] 4.1 Modify loop() function to use M5.Mic for audio capture
- [ ] 4.2 Send binary audio data via WebSocket
- [ ] 4.3 Implement audio buffer management (128 samples)
- [ ] 4.4 Implement transmission rate control
- [ ] 4.5 Keep serial port for debug logs

## 5. UI Updates

- [ ] 5.1 Add WiFi signal strength display
- [ ] 5.2 Add WebSocket connection status display
- [ ] 5.3 Update status text (Idle/Listening/Connecting)

## 6. PC End WebSocket Server

- [ ] 6.1 Create Python WebSocket server script
- [ ] 6.2 Implement audio data reception
- [ ] 6.3 Implement WAV file saving
- [ ] 6.4 Add real-time statistics display

## 7. Testing and Validation

- [ ] 7.1 Test WiFi connection stability
- [ ] 7.2 Test WebSocket connection and reconnection
- [ ] 7.3 Test audio transmission quality
- [ ] 7.4 Test end-to-end latency
- [ ] 7.5 Verify battery life impact

## 8. Documentation

- [ ] 8.1 Update README.md with WiFi configuration instructions
- [ ] 8.2 Add PC end server usage instructions
- [ ] 8.3 Document common issues and troubleshooting
