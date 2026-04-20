# M5 StickS3 Voice Assistant - WiFi Audio Streaming

M5 StickS3 voice assistant with real-time audio streaming to PC via WiFi WebSocket for speech recognition processing.

## Features

- ✅ 16kHz 16-bit high-quality audio capture
- ✅ WiFi WebSocket real-time transmission (~32KB/s)
- ✅ Auto-reconnect on disconnection
- ✅ Visual UI display (status, battery, WiFi signal)
- ✅ Push-to-Talk button (Button A)
- ✅ Real-time speech-to-text with Whisper AI
- ✅ No local audio file storage - instant transcription

## Hardware Requirements

- M5 StickS3 development board
- USB-C data cable
- 2.4GHz WiFi network

## Software Requirements

### StickS3 End (Arduino)
- Arduino IDE 2.0+ / arduino-cli
- M5Unified library
- WebSockets library (by Markus Sattler)
- ESP32 board support

### PC End (Python)
- Python 3.8+
- websockets
- faster-whisper
- Whisper AI model (base/small/medium/large-v3)

## Technology Stack

### Embedded (StickS3)
| Component | Technology |
|-----------|------------|
| Framework | Arduino (ESP32-S3) |
| Audio Capture | M5Unified I2S Microphone |
| Network | WiFi 802.11 b/g/n (2.4GHz) |
| Protocol | WebSocket (Binary) |
| Audio Format | 16kHz 16-bit PCM Mono |

### Server (PC)
| Component | Technology |
|-----------|------------|
| Language | Python 3.14+ |
| WebSocket Server | websockets (asyncio) |
| Speech Recognition | faster-whisper |
| AI Model | OpenAI Whisper (via HuggingFace) |
| Audio Processing | wave, tempfile |

### AI Models Available
| Model | Size | Accuracy | Speed |
|-------|------|----------|-------|
| base | 141MB | ⭐⭐⭐ | Fast |
| small | 464MB | ⭐⭐⭐⭐ | Medium |
| medium | 1.5GB | ⭐⭐⭐⭐⭐ | Slow |
| large-v3 | 3GB | ⭐⭐⭐⭐⭐⭐ | Slowest |

## Installation Steps

### 1. Arduino Environment Setup

1. Install M5Unified library:
   - Arduino IDE → Tools → Manage Libraries → Search "M5Unified" → Install

2. Install WebSockets library:
   - Arduino IDE → Tools → Manage Libraries → Search "WebSockets" → Install (by Markus Sattler)

3. Install ESP32 board support:
   - Arduino IDE → Preferences → Additional Boards Manager URLs
   - Add: `https://espressif.github.io/arduino-esp32/package_esp32_index.json`
   - Tools → Board → Boards Manager → Search "ESP32" → Install

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
   #define WEBSOCKET_HOST "192.168.x.x"
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

### 3. Install PC Python Dependencies

```bash
cd pc_receiver
pip install websockets faster-whisper
```

### 4. Configure WiFi and Server Address

### 5. Compile and Upload

Using arduino-cli:
```bash
arduino-cli compile --fqbn m5stack:esp32:m5stack_sticks3
arduino-cli upload -p /dev/cu.usbmodemXXX --fqbn m5stack:esp32:m5stack_sticks3
```

Or using Arduino IDE:
1. Select board: **M5Stick-S3**
2. Select correct serial port
3. Click upload button

### 6. Start PC Server and Test

```bash
cd pc_receiver
source venv/bin/activate  # If using virtualenv
python wifi_audio_receiver.py
```

**Usage:**
1. StickS3 auto-connects to WiFi and WebSocket
2. Screen displays connection status
3. **Press and hold Button A** to start recording
4. **Release Button A** to stop and transcribe
5. PC outputs transcribed text instantly

## Expected Output

### StickS3 Serial Output:
```
=== M5 StickS3 WiFi Audio Stream ===
Mic initialized
Connecting to WiFi: YourWiFi
..........
WiFi connected!
IP address: 192.168.x.x
Signal strength (RSSI): -65 dBm
Connecting to WebSocket: 192.168.x.x:8080/audio
[WebSocket] Connected
WebSocket connected!
Audio stream task created
Setup complete
[Button A] Started streaming
[Button A] Stopped streaming
```

### PC Output:
```
🧠 Loading Whisper model 'small'...
✅ Model loaded successfully!

============================================================
🎙️  M5 StickS3 WiFi Audio Receiver - Push to Talk
============================================================
📡 Listening on: ws://0.0.0.0:8080

✅ Client connected: ('192.168.x.x', xxxxx)
🎤 Recording...
🎤 Processing...
📝 您好，我在说话，您能听到我说话吗？
🎤 Recording...
🎤 Processing...
📝 今天天气不错
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
| Connecting | "Connecting..." | White | Connecting to WiFi/WS |
| Listening | "Listening" | Blue | Connected, waiting |
| Streaming | "Streaming" | Red | Recording audio |

## Communication Protocol

### WebSocket Messages

| Direction | Type | Content | Description |
|-----------|------|---------|-------------|
| StickS3 → PC | TEXT | `"START_RECORDING"` | Begin recording session |
| StickS3 → PC | BIN | Audio data (PCM) | Raw audio samples |
| StickS3 → PC | TEXT | `"STOP_RECORDING"` | End recording, trigger transcription |

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
| Protocol | Push-to-Talk (START/STOP) |
| Speech Recognition | Whisper AI (fine-tuned) |
| Supported Languages | Chinese (zh), English (en), etc. |

## References

- [Xiaozhi Voice Assistant GitHub](https://github.com/78/xiaozhi-esp32)
- [M5Unified Documentation](https://github.com/m5stack/M5Unified)
- [WebSockets Library](https://github.com/Links2004/arduinoWebSockets)
- [M5 StickS3 Documentation](https://docs.m5stack.com/en/core/StickS3)
- [faster-whisper GitHub](https://github.com/SYSTRAN/faster-whisper)
- [OpenAI Whisper](https://github.com/openai/whisper)
- [HuggingFace Model Hub](https://huggingface.co/Systran)

## License

MIT License
