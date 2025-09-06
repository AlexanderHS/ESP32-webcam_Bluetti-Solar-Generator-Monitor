# Bluetti Solar Generator Monitor

**Author:** Alex Hamilton Smith  
**Project Type:** Personal Home Automation  

## Overview

This is a weird little toy project for automating a non-web-enabled "dumb" solar generator (Bluetti device). The ESP32-S3 webcam captures images of the device's LCD screen to monitor battery percentage and automatically control charging/output based on battery levels.

## The Problem

The Bluetti solar generator needs careful power management:
- **Can't hit 0%** - All outputs turn off, including the monitoring camera itself
- **Shouldn't stay at 100%** with outputs disabled - wastes stored energy
- **No web interface** - Only manual LCD screen monitoring

## Solution Architecture

This ESP32 webcam is one component of a larger automation system:

1. **SwitchBot** - Physically taps screen to wake up the display
2. **ESP32 Webcam** (this project) - Captures LCD screen images
3. **Image Analysis** (TBD) - OCR/computer vision to read battery percentage
4. **Smart Switch Control** (TBD) - Controls power input and outputs based on battery level

### Planned Logic
- **< 10% battery** → Turn on grid power charging
- **> 80% battery** → Enable first power output
- **> [higher threshold]** → Enable second power output

## Hardware

- **ESP32-S3-DevKitC-1** with PSRAM enabled
- **Camera module** using Freenove standard pinout
- **Target:** Bluetti solar generator LCD screen

## Camera Settings

Optimized for reading bright LCD displays:
- **Resolution:** SVGA (800x600) with PSRAM
- **Exposure:** Reduced to prevent blown-out text
- **Contrast:** Enhanced for better text readability

## Usage

1. Build and upload: `pio run --target upload`
2. Monitor serial: `pio device monitor`
3. Connect to WiFi network and visit device IP
4. Use `/capture` endpoint to grab LCD screen images

## API Endpoints

- `GET /` - Simple status page
- `GET /capture` - Returns JPEG image of current LCD screen

## Development

See `CLAUDE.md` for detailed development instructions and project architecture.

---

*Part of a larger home automation project to prevent the Bluetti from hitting 0% battery while maximizing useful power output.*