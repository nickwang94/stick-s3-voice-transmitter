#!/usr/bin/env python3
"""
M5 StickS3 WiFi Audio Stream Receiver Server
Receives audio data from StickS3 via WebSocket and saves as WAV files

Dependencies:
    pip install websockets asyncio wave

Usage:
    python wifi_audio_receiver.py
"""

import asyncio
import websockets
import wave
import struct
import sys
from datetime import datetime

# Audio parameters (must match StickS3 configuration)
SAMPLE_RATE = 15000  # Sample rate 15kHz
CHANNELS = 1         # Mono
SAMPLE_WIDTH = 2     # 16-bit = 2 bytes

# Server configuration
HOST = "0.0.0.0"  # Listen on all network interfaces
PORT = 8080       # WebSocket port


class AudioRecorder:
    def __init__(self):
        self.audio_data = bytearray()
        self.start_time = None
        self.file_path = None
        self.bytes_received = 0

    def start_recording(self):
        """Start recording"""
        self.audio_data = bytearray()
        self.start_time = datetime.now()
        timestamp = self.start_time.strftime("%Y%m%d_%H%M%S")
        self.file_path = f"recording_{timestamp}.wav"
        self.bytes_received = 0
        print(f"🎤 Start recording -> {self.file_path}")

    def add_audio_data(self, data: bytes):
        """Add audio data"""
        self.audio_data.extend(data)
        self.bytes_received += len(data)

    def save_recording(self):
        """Save recording as WAV file"""
        if len(self.audio_data) == 0:
            print("⚠️  No audio data to save")
            return

        with wave.open(self.file_path, 'wb') as wf:
            wf.setnchannels(CHANNELS)
            wf.setsampwidth(SAMPLE_WIDTH)
            wf.setframerate(SAMPLE_RATE)
            wf.writeframes(self.audio_data)

        duration = len(self.audio_data) / (SAMPLE_RATE * CHANNELS * SAMPLE_WIDTH)
        print(f"💾 Saved: {self.file_path} ({duration:.2f}s, {self.bytes_received} bytes)")

    def get_duration(self) -> float:
        """Get current recording duration (seconds)"""
        return len(self.audio_data) / (SAMPLE_RATE * CHANNELS * SAMPLE_WIDTH)

    def get_bitrate(self) -> float:
        """Get current bitrate (KB/s)"""
        duration = self.get_duration()
        if duration > 0:
            return self.bytes_received / 1024 / duration
        return 0


async def handle_client(websocket):
    """Handle client connection"""
    print(f"✅ Client connected: {websocket.remote_address}")

    recorder = AudioRecorder()
    recorder.start_recording()

    # Statistics
    total_bytes = 0
    last_stats_time = datetime.now()
    last_data_time = datetime.now()  # Time of last received data
    timeout_seconds = 1.0  # Timeout in seconds

    try:
        while True:
            try:
                # Use wait_for with timeout
                message = await asyncio.wait_for(websocket.recv(), timeout=timeout_seconds)

                if isinstance(message, bytes):
                    # Receive binary audio data
                    recorder.add_audio_data(message)
                    total_bytes += len(message)
                    last_data_time = datetime.now()  # Update last received data time

                    # Print statistics every second
                    now = datetime.now()
                    if (now - last_stats_time).total_seconds() >= 1:
                        duration = recorder.get_duration()
                        kbps = recorder.get_bitrate()
                        print(f"📊 Recording: {duration:.1f}s | {len(recorder.audio_data)} bytes | Bitrate: {kbps:.1f}KB/s")
                        total_bytes = 0
                        last_stats_time = now

            except asyncio.TimeoutError:
                # Timeout, check if we have received any data
                if len(recorder.audio_data) > 0:
                    # No new data for timeout_seconds, save and restart
                    recorder.save_recording()
                    # Start new recording
                    recorder = AudioRecorder()
                    recorder.start_recording()
                    last_data_time = datetime.now()
                    last_stats_time = datetime.now()

    except websockets.exceptions.ConnectionClosed:
        print(f"❌ Client disconnected")
    except Exception as e:
        print(f"❌ Error: {e}")
    finally:
        # Save recorded audio (if any)
        if len(recorder.audio_data) > 0:
            recorder.save_recording()


async def main():
    """Main function"""
    print("=" * 60)
    print("🎙️  M5 StickS3 WiFi Audio Receiver Server")
    print("=" * 60)
    print(f"📡 Listening on: ws://{HOST}:{PORT}")
    print("Press Ctrl+C to stop server")
    print("=" * 60)
    print()
    print("StickS3 configuration:")
    print(f"  WEBSOCKET_URL = \"{get_local_ip()}\"")
    print(f"  WEBSOCKET_PORT = {PORT}")
    print()

    try:
        async with websockets.serve(handle_client, HOST, PORT):
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
