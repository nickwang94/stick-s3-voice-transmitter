## ADDED Requirements

### Requirement: USB Serial Transmission

The system shall transmit captured audio data to PC in real-time via USB serial port at 115200 baud rate.

#### Scenario: Data Transmission
- **WHEN** buffer has audio data
- **THEN** system sends raw PCM data using Serial.write()

#### Scenario: Transmission Rate
- **WHEN** operating normally
- **THEN** average transmission rate is approximately 10KB/s (16kHz × 16bit × 1 channel)

### Requirement: Data Format

Transmitted audio data shall be raw PCM data in 16-bit signed integer (int16_t) format.

#### Scenario: Data Format
- **WHEN** PC receives data
- **THEN** data is in PCM format with 16kHz sample rate, 16-bit depth, mono
