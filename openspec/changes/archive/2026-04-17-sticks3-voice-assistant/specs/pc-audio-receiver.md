## ADDED Requirements

### Requirement: Audio Reception

The PC program shall receive audio data from M5 StickS3 via serial port.

#### Scenario: Serial Connection
- **WHEN** program starts
- **THEN** automatically detect and open available serial port at 115200 baud rate

#### Scenario: Data Reception
- **WHEN** data arrives at serial port
- **THEN** program reads raw PCM data and adds to buffer

### Requirement: WAV Save

The program shall support saving received audio data as standard WAV file.

#### Scenario: Start Recording
- **WHEN** audio data is detected
- **THEN** create new recording file with timestamp name

#### Scenario: Save File
- **WHEN** user stops program
- **THEN** save buffer data as 16kHz 16-bit mono WAV file

### Requirement: Real-time Statistics

The program shall display real-time reception statistics.

#### Scenario: Display Statistics
- **WHEN** receiving data
- **THEN** update and display recording duration, data amount, and bitrate every second
