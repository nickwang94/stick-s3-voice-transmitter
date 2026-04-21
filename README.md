# Stick S3 - AI Vibe Coding Voice Assistant

Stick S3 is an AI Vibe Coding assistant that enables voice input to any AI terminal using the M5 StickS3 hardware. Speak naturally to AI coding assistants like Claude, ChatGPT, and more - hands-free coding efficiency.

## Project Architecture

This project consists of three parts:

```
┌─────────────────┐     WiFi WebSocket     ┌─────────────────┐
│   M5 StickS3    │ ──────────────────────▶ │  Python Backend │
│   (Hardware)    │                        │   (PC/Mac)      │
│                 │                        │                 │
│ • Voice capture │                        │ • WebSocket srv │
│ • Button control│                        │ • Whisper AI    │
│ • WiFi transfer │                        │ • Speech-to-text│
└─────────────────┘                        └─────────────────┘
                                                    │
                                                    ▼
                                         ┌─────────────────┐
                                         │  AI Terminal    │
                                         │ (Claude/VSCode) │
                                         └─────────────────┘
```

### 1. Stick S3 Hardware

- **MCU**: M5 StickS3 (ESP32-S3)
- **Audio**: Built-in I2S microphone, 16kHz 16-bit high-quality recording
- **Display**: 0.96" color screen showing status, WiFi signal, battery
- **Control**: Button A for Push-to-Talk recording
- **Network**: 2.4GHz WiFi, real-time WebSocket transmission

### 2. Python Backend

- **WebSocket Server**: Receives audio stream
- **Whisper AI**: Speech recognition using faster-whisper
- **Real-time Transcription**: No audio file storage needed

### 3. Workflow

1. User presses and holds Button A on Stick S3
2. Audio streams to PC via WiFi WebSocket
3. Python backend transcribes audio using Whisper AI
4. Transcribed text outputs to terminal, ready to paste into AI chat

## Hardware Setup

### Requirements

- M5 StickS3 development board
- USB-C data cable
- 2.4GHz WiFi network

### Flashing Steps

#### 1. Install Arduino Environment

**Option A: Arduino IDE**

1. Download and install [Arduino IDE 2.0+](https://www.arduino.cc/en/software)
2. Add ESP32 board support:
   - File → Preferences → Additional Boards Manager URLs
   - Add: `https://espressif.github.io/arduino-esp32/package_esp32_index.json`
   - Tools → Board → Boards Manager → Search "ESP32" → Install
3. Install libraries:
   - Tools → Manage Libraries → Search "M5Unified" → Install
   - Tools → Manage Libraries → Search "WebSockets" → Install (by Markus Sattler)

**Option B: arduino-cli**

```bash
# Install arduino-cli
brew install arduino-cli  # macOS
# Or follow official docs: https://arduino.github.io/arduino-cli/latest/installation/

# Initialize config
arduino-cli config init

# Add ESP32 board URL
arduino-cli config add board_manager.additional_urls https://espressif.github.io/arduino-esp32/package_esp32_index.json

# Install M5StickS3 core
arduino-cli core install m5stack:esp32

# Install libraries
arduino-cli lib install M5Unified
arduino-cli lib install "WebSockets"
```

#### 2. Configure WiFi and Server Address

1. Copy the configuration file:
   ```bash
   cd Arduino/sticks3-voice-assistant
   cp config.h.example config.h
   ```

2. Edit `config.h`:
   ```cpp
   // WiFi Configuration
   #define WIFI_SSID      "Your WiFi Name"
   #define WIFI_PASSWORD  "Your WiFi Password"

   // WebSocket Server Address (change to your PC's IP)
   #define WEBSOCKET_HOST "192.168.x.x"
   #define WEBSOCKET_PORT 8080
   ```

3. Get your PC's IP address:
   ```bash
   # macOS/Linux
   ifconfig

   # Windows
   ipconfig
   ```

#### 3. Compile and Upload

**Using Arduino IDE:**
1. Open `sticks3-voice-assistant.ino`
2. Select board: **M5Stick-S3**
3. Select correct serial port
4. Click upload button

**Using arduino-cli:**
```bash
# List available ports
arduino-cli board list

# Compile and upload (replace /dev/cu.usbmodemXXX with your actual port)
arduino-cli compile --fqbn m5stack:esp32:m5stack_sticks3
arduino-cli upload -p /dev/cu.usbmodemXXX --fqbn m5stack:esp32:m5stack_sticks3
```

## Python Backend Deployment

### 1. Install Dependencies

```bash
cd pc_receiver

# Create virtual environment (recommended)
python -m venv venv
source venv/bin/activate  # macOS/Linux
# or venv\Scripts\activate  # Windows

# Install dependencies
pip install websockets faster-whisper
```

### 2. Configure WiFi Settings

Edit `pc_receiver/wifi_audio_receiver.py` if needed:

```python
# Modify listen address or port
HOST = "0.0.0.0"  # Listen on all interfaces
PORT = 8080       # WebSocket port
```

### 3. Start the Server

```bash
python wifi_audio_receiver.py
```

First run will automatically download the Whisper AI model (~500MB). Available models:
- `base`: 141MB, fast
- `small`: 464MB, balanced (default)
- `medium`: 1.5GB, high accuracy
- `large-v3`: 3GB, highest accuracy

### 4. Usage

1. Stick S3 auto-connects to WiFi and WebSocket
2. Screen displays connection status
3. **Press and hold Button A** to start recording
4. **Release Button A** to stop and transcribe
5. Terminal outputs transcribed text instantly

## Expected Output

### Stick S3 Serial Output:
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

### PC Terminal Output:
```
🧠 Loading Whisper model 'small'...
✅ Model loaded successfully!

============================================================
🎙️  Stick S3 WiFi Audio Receiver - Push to Talk
============================================================
📡 Listening on: ws://0.0.0.0:8080

✅ Client connected: ('192.168.x.x', xxxxx)
🎤 Recording...
🎤 Processing...
📝 Hello, I am speaking, can you hear me?
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

## Technical Specifications

| Parameter | Value |
|-----------|-------|
| Sample Rate | 16kHz |
| Bit Depth | 16-bit |
| Channels | Mono |
| Frame Duration | 60ms |
| Buffer Size | 1024 samples |
| Bitrate | ~256kbps (32KB/s) |
| WiFi | 2.4GHz 802.11 b/g/n |
| Protocol | WebSocket binary frames |
| Supported Languages | Chinese, English, 90+ languages |

## Troubleshooting

### WiFi Connection Failed
- Check SSID and password are correct
- Ensure StickS3 is within WiFi range
- Make sure using 2.4GHz WiFi (5GHz not supported)

### WebSocket Connection Failed
- Check PC firewall allows port 8080
- Verify PC IP address is correct
- Ensure Python server is running

### Poor Audio Quality
- Check WiFi signal strength (RSSI > -70dBm recommended)
- Ensure PC and StickS3 are on same network
- Try moving closer to router

### Short Battery Life
- WiFi transmission consumes more power, this is normal
- Can connect to USB power for extended use

## Project Structure

```
sticks3-voice-assistant/
├── config.h.example              # Example configuration (safe to share)
├── config.h                      # Configuration (WiFi, password) - DO NOT COMMIT
├── sticks3-voice-assistant.ino   # Arduino main program
├── pc_receiver/
│   ├── wifi_audio_receiver.py    # WebSocket server
│   └── transcribe.py             # Whisper transcription module
├── openspec/
│   └── changes/
│       ├── sticks3-voice-assistant/
│       └── wifi-audio-streaming/
├── README.md
└── .gitignore
```

## References

- [M5Unified Documentation](https://github.com/m5stack/M5Unified)
- [WebSockets Library](https://github.com/Links2004/arduinoWebSockets)
- [M5 StickS3 Documentation](https://docs.m5stack.com/en/core/StickS3)
- [faster-whisper](https://github.com/SYSTRAN/faster-whisper)
- [OpenAI Whisper](https://github.com/openai/whisper)

## License

MIT License
