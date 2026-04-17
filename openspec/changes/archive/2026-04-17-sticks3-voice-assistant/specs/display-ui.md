## ADDED Requirements

### Requirement: Screen Display

The system shall display status information, Logo, and battery level on the 130x240 pixel display.

#### Scenario: Boot Display
- **WHEN** device starts
- **THEN** screen displays Claude Logo, status text, and battery icon

#### Scenario: Logo Display
- **WHEN** device is running
- **THEN** 110x110 pixel Claude Logo is displayed at top center of screen

#### Scenario: Status Text
- **WHEN** status changes
- **THEN** screen displays current status text at center (Idle in green, Listening in blue)

#### Scenario: Battery Icon
- **WHEN** device is running
- **THEN** battery icon and percentage is displayed in top right corner

### Requirement: Button Interaction

The system shall respond to Button A press and release operations.

#### Scenario: Press Button A
- **WHEN** user presses Button A
- **THEN** system enters Listening state and starts capture

#### Scenario: Release Button A
- **WHEN** user releases Button A
- **THEN** system returns to Idle state and stops capture
