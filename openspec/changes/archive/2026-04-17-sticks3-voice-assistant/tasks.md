## 1. Project Initialization

- [ ] 1.1 Rename project directory to sticks3-voice-assistant
- [ ] 1.2 Rename .ino file to sticks3_voice_assistant.ino
- [ ] 1.3 Run openspec init --tools claude to initialize OpenSpec
- [ ] 1.4 Create change: sticks3-voice-assistant

## 2. Documentation Creation

- [ ] 2.1 Create proposal.md defining project scope
- [ ] 2.2 Create design.md describing technical architecture
- [ ] 2.3 Create specs/voice-capture.md audio capture spec
- [ ] 2.4 Create specs/usb-serial-transmission.md serial transmission spec
- [ ] 2.5 Create specs/display-ui.md display UI spec
- [ ] 2.6 Create specs/button-control.md button control spec
- [ ] 2.7 Create specs/pc-audio-receiver.md PC receiver spec
- [ ] 2.8 Create tasks.md implementation task list

## 3. Arduino Firmware Development

- [ ] 3.1 Configure M5Unified and M5.Mic initialization
- [ ] 3.2 Implement audio capture loop (128 sample buffer)
- [ ] 3.3 Implement serial data transmission
- [ ] 3.4 Implement UI display (Logo, status text, battery icon)
- [ ] 3.5 Implement Button A detection logic
- [ ] 3.6 Add blinking effect (500ms interval)

## 4. PC Receiver Program

- [ ] 4.1 Create Python virtual environment
- [ ] 4.2 Install websockets and pyserial dependencies
- [ ] 4.3 Implement serial data reception
- [ ] 4.4 Implement WAV file saving
- [ ] 4.5 Add real-time statistics display

## 5. Testing and Validation

- [ ] 5.1 Test audio capture functionality
- [ ] 5.2 Test serial transmission rate
- [ ] 5.3 Verify UI display correctness
- [ ] 5.4 Verify button response
- [ ] 5.5 End-to-end test (StickS3 -> PC save)

## 6. Documentation Completion

- [ ] 6.1 Update README.md usage instructions
- [ ] 6.2 Add serial receiver documentation
- [ ] 6.3 Document troubleshooting guide
