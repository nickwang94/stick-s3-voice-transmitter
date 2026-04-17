## ADDED Requirements

### Requirement: Button A Control

The system shall use Button A as the trigger switch for voice capture, using Press-to-Talk mode.

#### Scenario: Start Capture
- **WHEN** user presses and holds Button A
- **THEN** system enters Listening state and starts continuous audio capture

#### Scenario: Stop Capture
- **WHEN** user releases Button A
- **THEN** system returns to Idle state and stops capture

### Requirement: Continuous Capture

The system shall continuously capture audio while Button A is held, without noticeable interruption.

#### Scenario: Continuous Buffer
- **WHEN** Button A is held down
- **THEN** system uses small buffer of 128 samples for continuous reading and sending
