## ADDED Requirements

### Requirement: Real-time Audio Stream

The system shall stream audio data to PC in real-time over WiFi.

#### Scenario: Continuous Transmission
- **WHEN** user presses and holds Button A
- **THEN** system continuously captures and sends audio data without noticeable interruption

#### Scenario: Stop Transmission
- **WHEN** user releases Button A
- **THEN** system stops capturing and sending audio data

### Requirement: Audio Buffer

The system shall use appropriate buffer size to balance latency and stability.

#### Scenario: Buffer Size
- **WHEN** capturing audio
- **THEN** use small buffer of 128-256 samples for continuous reading

#### Scenario: Transmission Rate
- **WHEN** operating normally
- **THEN** average transmission rate is approximately 32KB/s (16kHz × 16bit × 1 channel)
