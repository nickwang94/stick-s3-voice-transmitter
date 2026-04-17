## MODIFIED Requirements

### Requirement: USB Serial Transmission

**FROM:** Primary audio transmission method
**TO:** Debug backup method

#### Scenario: Debug Output
- **WHEN** debugging is needed
- **THEN** serial port can be used for debug logs instead of audio data

**Reason:** WiFi transmission becomes the primary method, serial is demoted to debug use

**Migration:** No migration needed, keep serial port for log output
