# M5 StickS3 Voice Assistant - WiFi Audio Streaming

M5 StickS3 voice assistant with real-time audio streaming to PC via WiFi WebSocket for speech recognition processing.

## Features

- ✅ 16kHz 16-bit high-quality audio capture
- ✅ WiFi WebSocket real-time transmission (~32KB/s)
- ✅ Auto-reconnect on disconnection
- ✅ Visual UI display (status, battery, WiFi signal)
- ✅ Press-to-Talk button (Button A)
- ✅ PC auto-saves to WAV files

## Hardware Requirements

- M5 StickS3 development board
- USB-C data cable
- 2.4GHz WiFi network

## Software Requirements

### StickS3 End (Arduino)
- Arduino IDE 2.0+
- M5Unified library
- WebSockets library (by Markus Sattler)

### PC End
- Python 3.8+
- websockets library

## Installation Steps

### 1. Arduino Environment Setup

1. Install M5Unified library:
   - Arduino IDE → Tools → Manage Libraries → Search "M5Unified" → Install

2. Install WebSockets library:
   - Arduino IDE → Tools → Manage Libraries → Search "WebSockets" → Install (by Markus Sattler)

### 2. Configure WiFi and Server Address

1. Copy the example configuration file:

   ```bash
   cp config.h.example config.h
   ```

2. Edit `config.h` in the project directory:

   ```cpp
   // WiFi Configuration
   #define WIFI_SSID      "Your WiFi Name"
   #define WIFI_PASSWORD  "Your WiFi Password"

   // WebSocket Server Address (change to your PC's IP)
   #define WEBSOCKET_HOST "192.168.1.100"
   #define WEBSOCKET_PORT 8080
   ```

3. Get your PC's IP address:

   **Windows:**
   ```cmd
   ipconfig
   ```

   **macOS/Linux:**
   ```bash
   ifconfig
   ```

   Find your local IP address (usually `192.168.x.x`).

> **⚠️ Security Note:** `config.h` contains sensitive data (WiFi password, IP addresses).
> This file is already in `.gitignore` and should NOT be committed to version control.
> The example file `config.h.example` is safe to share and can be used as a template.

### 3. Start PC Receiver Server

```bash
cd pc_receiver
pip install websockets
python wifi_audio_receiver.py
```

You should see output like:
```
============================================================
🎙️  M5 StickS3 WiFi Audio Receiver Server
============================================================
📡 Listening on: ws://0.0.0.0:8080
Press Ctrl+C to stop server
============================================================

StickS3 configuration:
  WEBSOCKET_URL = "192.168.1.100"
  WEBSOCKET_PORT = 8080
```

### 4. Compile and Upload

1. In Arduino IDE, select board: **M5Stick-S3**
2. Select correct serial port
3. Click upload button

### 5. Test

1. After StickS3 starts, it automatically connects to WiFi and WebSocket server
2. Screen displays current status after connection
3. **Press and hold Button A** to start audio capture and transmission
4. **Release Button A** to stop transmission
5. PC saves audio to WAV file after timeout (~1 second)

## Expected Output

### StickS3 Serial Output:
```
=== M5 StickS3 WiFi Audio Stream ===
Mic initialized
Connecting to WiFi: YourWiFi
..........
WiFi connected!
IP address: 192.168.1.105
Signal strength (RSSI): -65 dBm
Connecting to WebSocket: 192.168.1.100:8080/audio
[WebSocket] Connected
WebSocket connected!
Audio stream task created
Setup complete
```

### PC Output:
```
============================================================
🎙️  M5 StickS3 WiFi Audio Receiver Server
============================================================
📡 Listening on: ws://0.0.0.0:8080

✅ Client connected: 192.168.1.105:54321
🎤 Start recording -> recording_20260417_203000.wav
📊 Recording: 1.0s | 32000 bytes | Bitrate: 31.2KB/s
📊 Recording: 2.0s | 64000 bytes | Bitrate: 31.2KB/s
💾 Saved: recording_20260417_203000.wav (10.50s, 336000 bytes)
```

## Screen UI

```
┌─────────────────┐
│   [AI Logo]     │  ← Claude Logo
│                 │
│   Listening...  │  ← Status text (color changes)
│                 │
│   WiFi: -65dBm  │  ← WiFi signal strength
│          [🔋] 80%│  ← Battery icon
└─────────────────┘
```

## Status Description

| Status | Screen Text | Color | Description |
|--------|-------------|-------|-------------|
| Idle | "Idle" | Green | Standby state |
| WiFi... | "WiFi..." | White | Connecting to WiFi |
| WS... | "WS..." | White | Connecting to WebSocket |
| Listening | "Listening" | Blue | Waiting for capture |
| Streaming | "Streaming" | Red | Transmitting audio |

## Troubleshooting

### WiFi Connection Failed
- Check SSID and password are correct
- Ensure StickS3 is within WiFi range
- Make sure using 2.4GHz WiFi (5GHz not supported)

### WebSocket Connection Failed
- Check PC firewall allows port 8080
- Verify PC IP address is correct
- Ensure PC receiver server is running

### Poor Audio Quality
- Check WiFi signal strength (RSSI > -70dBm recommended)
- Ensure PC and StickS3 are on same network
- Try moving closer to router

### Short Battery Life
- WiFi transmission consumes more power, this is normal
- Can connect to USB power for extended use

### Audio Plays Too Fast/Slow
- Adjust `SAMPLE_RATE` in `pc_receiver/wifi_audio_receiver.py`
- M5 StickS3 actual sample rate may vary (default ~15kHz)

## Project Structure

```
sticks3-voice-assistant/
├── config.h.example              # Example configuration file (safe to share)
├── config.h                      # Configuration file (WiFi, WebSocket) - DO NOT COMMIT
├── sticks3-voice-assistant.ino   # Arduino main program
├── pc_receiver/
│   └── wifi_audio_receiver.py    # PC receiver server
├── openspec/
│   └── changes/
│       ├── sticks3-voice-assistant/
│       └── wifi-audio-streaming/
├── README.md
└── .gitignore
```

## Technical Specifications

| Parameter | Value |
|-----------|-------|
| Sample Rate | 16kHz (configurable) |
| Bit Depth | 16-bit |
| Channels | Mono |
| Frame Duration | 60ms |
| Buffer Size | 1024 samples |
| Bitrate | ~256kbps (32KB/s) |
| WiFi | 2.4GHz 802.11 b/g/n |
| WebSocket | Binary frames |

## References

- [Xiaozhi Voice Assistant GitHub](https://github.com/78/xiaozhi-esp32)
- [M5Unified Documentation](https://github.com/m5stack/M5Unified)
- [WebSockets Library](https://github.com/Links2004/arduinoWebSockets)
- [M5 StickS3 Documentation](https://docs.m5stack.com/en/core/StickS3)

## License

MIT License
