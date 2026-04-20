#!/usr/bin/env python3
"""
Transcribe local WAV audio file to text using Whisper
"""

import sys
from faster_whisper import WhisperModel

# Model options: "tiny", "base", "small", "medium", "large-v2", "large-v3"
MODEL = "base"
# Language: "zh" for Chinese, "en" for English, etc.
LANGUAGE = "zh"

def transcribe(audio_path: str):
    print(f"🧠 Loading Whisper model '{MODEL}'...")
    model = WhisperModel(MODEL, device="cpu", compute_type="int8")
    print(f"✅ Model loaded")

    print(f"🎤 Transcribing: {audio_path}")
    segments, info = model.transcribe(audio_path, language=LANGUAGE)

    text = "".join([segment.text for segment in segments])

    print(f"\n{'='*60}")
    print(f"📝 TRANSCRIPTION RESULT:")
    print(f"{'='*60}")
    print(f"{text}")
    print(f"{'='*60}")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python transcribe.py <audio_file.wav>")
        print("Example: python transcribe.py recording_20260420_214334.wav")
        sys.exit(1)

    transcribe(sys.argv[1])
