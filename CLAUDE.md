# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is an Arduino-based label maker project that draws vector text and shapes on label tape. It's based on the original CrunchLabs Hackpack but enhanced with an Arduino Nano ESP32 for Bluetooth Low Energy (BLE) connectivity. The device can be controlled via physical joystick/buttons or remotely through a web interface using BLE.

## Architecture

### Core Components

**Hardware Control**
- `label-maker.ino` - Main Arduino sketch containing the complete firmware
- `GCodeParser.h` - Lightweight G-code parser for interpreting drawing commands
- `ext_nimble_config.h` - NimBLE configuration for BLE functionality

**Web Interface**
- `web-client.html` - Standalone web client using Web Bluetooth API to control the device

### Key Systems

1. **Motion Control**: Uses Bresenham's line algorithm for smooth pen movement between two stepper motors (X/Y axes)
2. **Text Rendering**: Vector-based character definitions stored in lookup tables for precise plotting
3. **G-code Support**: Custom parser supporting basic G0/G1 moves, G90/G91 modes, M3/M5 pen control
4. **BLE Protocol**: Command-based interface with chunked transfer support for larger G-code programs
5. **State Machine**: UI states (MainMenu, Editing, PrintConfirmation, Printing) manage device behavior

### Coordinate System

- **Physical Units**: Internal coordinates in steps, with scaling factors to convert to/from millimeters
- **Print Area**: ~260mm (X) × 30mm (Y) usable print area
- **G-code Integration**: 1mm = 44.4 units (X), 114.3 units (Y) with 1mm Y-offset for clearance

## Build and Development Commands

### Arduino IDE Setup
```bash
# Install board package: "Arduino ESP32" by Arduino
# Select: Tools → Board → Arduino ESP32 → Arduino Nano ESP32
# Install libraries: LiquidCrystal I2C, ezButton, ESP32Servo (version 3.0.7)
```

### VS Code Development (Optional)
```bash
# Compile for Arduino Nano ESP32
arduino-cli compile --fqbn arduino:esp32:nano_nora --warnings default --export-binaries .

# Upload to device
arduino-cli upload --fqbn arduino:esp32:nano_nora .
```

### Web Interface Deployment

```bash
# Deploy web interface to server
./deploy.sh

# Edit server configuration in deploy.sh:
# SERVER_USER="your_username"
# SERVER_HOST="your_server_ip_or_hostname"
# DEPLOY_PATH="/path/to/web/directory"
```

## BLE Protocol

### Service Configuration

- **Service UUID**: `12345678-1234-5678-1234-56789abcdef0`
- **Command Characteristic**: `87654321-4321-8765-4321-87654321fedc` (Write/Read/Notify)
- **Status Characteristic**: `5f4e3d2c-1b0a-0908-0706-050403020100` (Notify only)

### Command Format

All commands use format: `command,parameters` (comma required)

**Basic Commands**:

- `print-text,<text>` - Print vector text
- `pen-up,` - Lift pen
- `pen-down,` - Lower pen
- `print-raw,<gcode>` - Execute G-code program

**Chunked Transfer** (for large G-code):

- `print-raw-begin,`
- `print-raw-data,<chunk>` (repeat as needed)
- `print-raw-end,`

## Development Notes

### Important Constraints

- ESP32 uses 3.3V logic vs original 5V Arduino Nano - joystick may need voltage dividers for safety
- ESP32Servo library version 3.0.7 recommended (3.0.8 has known issues)
- Character vectors are hardcoded in lookup tables - modifications require understanding the encoding system
- G-code parser is minimal - only supports basic movement and pen commands

### File Structure

- Main firmware: `label-maker.ino` (single file contains all functionality)
- Character definitions: Embedded in vector arrays within main file
- Font files: TTF files included but used by web interface, not firmware
- Build artifacts: Generated in `build/` directory

### Testing

- Use `web-client.html` for quick BLE testing
- Serial monitor (9600 baud) for debugging
- Test G-code: `G90;M5;G0 X0 Y0;M3;G1 X20 Y0;G1 X20 Y10;G1 X0 Y10;G1 X0 Y0;M5`

### Common Issues

- BLE connection failures: Power-cycle device and retry
- Missing commands: Ensure comma in command format
- Servo reliability: Increase `SERVO_DWELL_DOWN_MS` if pen doesn't touch tape consistently