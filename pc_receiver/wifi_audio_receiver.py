#!/usr/bin/env python3
"""
M5 StickS3 WiFi Audio Stream Receiver with Push-to-Talk Speech Recognition
Receives audio data from StickS3 via WebSocket, transcribes on button release

Protocol:
    - Send "START_RECORDING" when button A is pressed
    - Send audio data (bytes) while button is held
    - Send "STOP_RECORDING" when button A is released
    - Server transcribes and outputs text

Dependencies:
    pip install websockets asyncio faster-whisper

Usage:
    python wifi_audio_receiver.py
"""

import asyncio
import websockets
import wave
import sys
import os
import tempfile
from datetime import datetime
from faster_whisper import WhisperModel

# Audio parameters (must match StickS3 configuration)
SAMPLE_RATE = 15000  # Sample rate 15kHz
CHANNELS = 1         # Mono
SAMPLE_WIDTH = 2     # 16-bit = 2 bytes

# Server configuration
HOST = "0.0.0.0"  # Listen on all network interfaces
PORT = 8080       # WebSocket port

# Speech recognition model
WHISPER_MODEL = "small"
LANGUAGE = "zh"  # Chinese


class AudioBuffer:
    """Buffer for audio data during recording"""
    def __init__(self):
        self.audio_data = bytearray()
        self.start_time = None

    def start(self):
        """Start recording"""
        self.audio_data = bytearray()
        self.start_time = datetime.now()
        print(f"🎤 Recording...")

    def add_audio_data(self, data: bytes):
        """Add audio data"""
        self.audio_data.extend(data)

    def stop(self):
        """Stop recording"""
        print(f"🎤 Processing...")

    def get_duration(self) -> float:
        """Get recording duration (seconds)"""
        if len(self.audio_data) == 0:
            return 0
        return len(self.audio_data) / (SAMPLE_RATE * CHANNELS * SAMPLE_WIDTH)

    def get_audio(self) -> bytes:
        """Get all audio data"""
        return bytes(self.audio_data)

    def clear(self):
        """Clear audio buffer"""
        self.audio_data = bytearray()
        self.start_time = None


def load_model():
    """Load Whisper model"""
    print(f"🧠 Loading Whisper model '{WHISPER_MODEL}'...")
    model = WhisperModel(WHISPER_MODEL, device="cpu", compute_type="int8")
    print(f"✅ Model loaded successfully!\n")
    return model


def transcribe_audio(audio_data: bytes, model) -> str:
    """Transcribe audio data using Whisper"""
    if not audio_data or len(audio_data) == 0:
        return ""

    # Create temp file for transcription
    temp_file = tempfile.NamedTemporaryFile(suffix='.wav', delete=False)
    temp_path = temp_file.name
    temp_file.close()

    with wave.open(temp_path, 'wb') as wf:
        wf.setnchannels(CHANNELS)
        wf.setsampwidth(SAMPLE_WIDTH)
        wf.setframerate(SAMPLE_RATE)
        wf.writeframes(audio_data)

    try:
        segments, info = model.transcribe(temp_path, language=LANGUAGE)
        text = "".join([segment.text for segment in segments])
        return text
    finally:
        # Delete temp file after transcription
        if os.path.exists(temp_path):
            os.unlink(temp_path)


async def handle_client(websocket, model):
    """Handle client connection with push-to-talk protocol"""
    print(f"✅ Client connected: {websocket.remote_address}")

    buffer = AudioBuffer()
    is_recording = False

    try:
        while True:
            message = await websocket.recv()

            if isinstance(message, bytes):
                # Audio data - only record if we're in recording state
                if is_recording:
                    buffer.add_audio_data(message)

            elif isinstance(message, str):
                # Control command
                if message == "START_RECORDING":
                    is_recording = True
                    buffer.start()

                elif message == "STOP_RECORDING":
                    is_recording = False
                    buffer.stop()

                    # Transcribe the recorded audio
                    duration = buffer.get_duration()
                    if duration > 0:
                        text = transcribe_audio(buffer.get_audio(), model)
                        if text:
                            print(f"📝 {text}")
                        else:
                            print("⚠️  No speech detected")
                    else:
                        print("⚠️  No audio data received")

                    buffer.clear()

    except websockets.exceptions.ConnectionClosed:
        print(f"\n❌ Client disconnected")

        # Transcribe any remaining audio if recording was active
        if is_recording and len(buffer.audio_data) > 0:
            duration = buffer.get_duration()
            print(f"🎤 Finalizing ({duration:.1f}s)...")
            text = transcribe_audio(buffer.get_audio(), model)
            if text:
                print(f"📝 {text}")

    except Exception as e:
        print(f"❌ Error: {e}")


async def main():
    """Main function"""
    # Load model at startup
    model = load_model()

    print("=" * 60)
    print("🎙️  M5 StickS3 WiFi Audio Receiver - Push to Talk")
    print("=" * 60)
    print(f"📡 Listening on: ws://{HOST}:{PORT}")
    print("Press Ctrl+C to stop server")
    print("=" * 60)
    print()
    print("StickS3 configuration:")
    print(f"  WEBSOCKET_URL = \"{get_local_ip()}\"")
    print(f"  WEBSOCKET_PORT = {PORT}")
    print()
    print("Protocol:")
    print("  1. Send 'START_RECORDING' when button pressed")
    print("  2. Send audio data (binary) while holding button")
    print("  3. Send 'STOP_RECORDING' when button released")
    print("  4. Server transcribes and outputs text")
    print()

    try:
        async with websockets.serve(lambda ws: handle_client(ws, model), HOST, PORT):
            await asyncio.Future()  # Run forever
    except KeyboardInterrupt:
        print("\n👋 Server stopped")
    except Exception as e:
        print(f"❌ Server error: {e}")
        sys.exit(1)


def get_local_ip():
    """Get local IP address"""
    import socket
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        s.connect(("8.8.8.8", 80))
        ip = s.getsockname()[0]
        s.close()
        return ip
    except Exception:
        return "192.168.x.x"


if __name__ == "__main__":
    asyncio.run(main())
