# LoafOf

**LoafOf** — because the prototype lived on a breadboard. Get it? *Loaf of bread.*

A lightweight Teensy-based dual-CAN gateway and vehicle data parser. It listens to high-speed and low-speed CAN buses, extracts key vehicle signals (speed, RPM, throttle, blinkers, climate, steering, time, etc.), and optionally streams raw frames as JSON over USB serial for logging or further processing.

## Features

- Dual independent CAN interfaces (CAN1 @ 500 kbps, CAN2 @ 33.333 kbps)
- Real-time parsing of selected vehicle signals into a clean `Variables` struct
- Optional JSON logging of raw CAN frames (per-bus enable)
- Built-in LED indicator for logging activity
- Minimal dependency footprint (FlexCAN_T4 + a tiny semaphore helper)
- Serial command interface for runtime control

## Hardware

- Teensy 3.x / 4.x (tested with FlexCAN_T4)
- Two CAN transceivers (e.g. TJA1050 / SN65HVD230) on the appropriate pins
- Breadboard prototype preferred (for maximum loaf authenticity)

## Dependencies

- [FlexCAN_T4](https://github.com/tonton81/FlexCAN_T4) by tonton81

## Quick Start

1. Install the FlexCAN_T4 library via the Arduino Library Manager or by cloning into your libraries folder.
2. Open `LoafOf.ino` in the Arduino IDE (or PlatformIO).
3. Select your Teensy board and upload.
4. Open the serial monitor at **115200** baud.

### Serial Commands

Send a line starting with `l` followed by two characters to control logging:

| Command | Effect                          |
|---------|---------------------------------|
| `l00`   | Disable logging                 |
| `l10`   | Log CAN1 only                   |
| `l01`   | Log CAN2 only                   |
| `l11`   | Log both buses                  |

When logging is active the built-in LED turns on. Logged frames are emitted as one JSON object per line:

```json
{"bus": 1, "id": 271646784, "len": 8, "r": 0, "e": 1, "o": 0, "data": [0, 42, 0, 0, 0, 0, 0, 0]}
```

## Project Structure

```
LoafOf.ino     — Main sketch: CAN setup, receive callbacks, serial control
Datum.h        — Signal parser and vehicle data structure
Semaphore.h    — Tiny cooperative semaphore (unused in current loop but ready for expansion)
```

## Parsed Signals (Datum)

The parser currently extracts the following from known message IDs:

- Vehicle speed (mph / km/h)
- Engine RPM
- Throttle position
- Steering wheel angle
- Turn signals and brake light
- Fog lights
- Driver/passenger climate set points
- Rear defroster
- Date & time

Unmatched frames simply increment the receive counters.

## License

MIT (or whatever Jacob prefers — original author is Jacob A. Psimos, 2020).

---

*Built on a breadboard. Named accordingly. No actual bread was harmed.*
