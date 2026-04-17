## ADDED Requirements

### Requirement: WebSocket Client

The system shall implement WebSocket client functionality to connect to PC's WebSocket server.

#### Scenario: Connect to Server
- **WHEN** WiFi connection succeeds
- **THEN** system attempts to connect to configured WebSocket server address

#### Scenario: Connection Confirmation
- **WHEN** WebSocket connection succeeds
- **THEN** system enters audio transmission ready state

#### Scenario: Reconnection
- **WHEN** WebSocket connection is lost
- **THEN** system automatically attempts to reconnect to server

### Requirement: Binary Data Transmission

The system shall send audio data via WebSocket binary frames.

#### Scenario: Send Audio
- **WHEN** audio data is captured
- **THEN** system sends PCM data via WebSocket binary message

#### Scenario: Data Format
- **WHEN** sending data
- **THEN** data format is 16kHz, 16-bit, mono raw PCM data
