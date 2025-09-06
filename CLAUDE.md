# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is an ESP32-S3 webcam server project built with PlatformIO and the Arduino framework. The application creates a WiFi-enabled camera server that can capture and serve JPEG images via HTTP endpoints.

## Development Commands

### Build and Upload
```bash
# Build the project
pio run

# Upload to device
pio run --target upload

# Build and upload in one command
pio run --target upload

# Monitor serial output
pio device monitor

# Build, upload, and monitor
pio run --target upload && pio device monitor
```

### Testing
```bash
# Run unit tests
pio test
```

## Hardware Configuration

- **Target Board**: ESP32-S3-DevKitC-1
- **Camera Module**: Uses Freenove standard pinout configuration
- **Pin Definitions**: All camera pins are defined in src/main.cpp:12-27

## Code Architecture

### Core Components

1. **Camera Initialization** (src/main.cpp:37-70)
   - Configures camera with VGA resolution (640x480)
   - JPEG format with quality setting of 12
   - Uses ESP32 camera library (`esp_camera.h`)

2. **WiFi Connectivity** (src/main.cpp:74-83)
   - Hardcoded SSID/password in source (consider moving to config)
   - Connects on startup and displays local IP

3. **Web Server** (src/main.cpp:85-90)
   - Runs on port 80
   - Two main routes: `/` (info page) and `/capture` (image capture)

4. **HTTP Endpoints**:
   - `GET /` - Simple HTML page with navigation link
   - `GET /capture` - Captures and returns JPEG image directly

### Key Functions

- `handleRoot()` - Serves basic HTML landing page
- `handleCapture()` - Captures image from camera and streams JPEG data
- `setup()` - Initializes camera, WiFi, and web server
- `loop()` - Handles incoming HTTP requests

## Network Configuration

Update WiFi credentials in src/main.cpp:6-7:
```cpp
const char* ssid = "Your_SSID";
const char* password = "Your_Password";
```

## Camera Settings

Current configuration (src/main.cpp:61-63):
- Frame size: VGA (640x480)
- JPEG quality: 12 (0-63 range, lower = better quality)
- Frame buffer count: 1

## Development Notes

- Serial baud rate: 115200
- Camera uses SCCB protocol for configuration (I2C-like)
- Single-threaded web server handles one request at a time
- Images are captured on-demand, no streaming capability
- Memory management: Frame buffers are returned after use