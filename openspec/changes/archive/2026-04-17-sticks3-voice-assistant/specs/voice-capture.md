## ADDED Requirements

### Requirement: Audio Capture

The system shall capture audio data via M5 StickS3's built-in microphone at 16kHz sample rate with 16-bit depth.

#### Scenario: Normal Capture
- **WHEN** user presses Button A
- **THEN** system starts capturing audio and sends via serial port

#### Scenario: Stop Capture
- **WHEN** user releases Button A
- **THEN** system stops capture and enters idle state

### Requirement: Status Display

The system shall display current working status on screen in real-time.

#### Scenario: Idle Status Display
- **WHEN** system is in idle state
- **THEN** screen displays green "Idle" text

#### Scenario: Capture Status Display
- **WHEN** system is capturing audio
- **THEN** screen displays blue "Listening..." text with blinking effect

### Requirement: Battery Display

The system shall display current battery percentage in the top right corner of the screen.

#### Scenario: Battery Update
- **WHEN** system is running
- **THEN** screen displays current battery percentage and charging status

### Requirement: Serial Transmission

The system shall send captured audio data to PC via USB serial port.

#### Scenario: Data Transmission
- **WHEN** audio data is captured
- **THEN** system sends raw PCM data at 115200 baud rate via serial port

#### Scenario: Charging Status Display
- **WHEN** device is connected to USB power
- **THEN** battery icon displays green indicating charging
