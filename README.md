# ESP32 LED Status Indicator

A lightweight, non-blocking status LED library for dual-core ESP32 boards.
Provides a unified API for built-in or external LEDs without requiring changes to application code when switching LED types.


## ✨ Features

- ✅ Supports built-in and external LEDs
- ✅ Mono, RGB, and WS2812 support
- ✅ Fully non-blocking (Core 0 task)
- ✅ Compile-time LED type selection
- ✅ Dummy driver if no LED is present
- ✅ Lightweight (no mandatory dependencies)
- ✅ Optional Adafruit NeoPixel support


## 📦 Requirements

- Dual-core ESP32 (not supported on ESP32-C3)
- Arduino IDE (ESP32 core installed)


## 🔧 Supported LED Types

- Built-in mono LED
- Built-in WS2812
- External mono LED
- External RGB LED (common cathode or anode)

LED type is selected via `#define` at compile time.

No application code changes are required when switching LED hardware.

## 🚦 Supported Status Modes

| Status | Description |
|--------|-------------|
| OFF | LED off |
| OK | Solid green |
| INFO | Green breathing effect |
| CONNECTING | Blue heartbeat effect |
| WARNING | Slow yellow blinking |
| ERROR | Fast red blinking |
| RAINBOW | RGB rainbow cycle (mono LED: solid ON) |
| USER | Custom color and blink rate |


## 🚀 Basic Usage

1. To enable the appropriate type of LED, use `#define` before the line: `#include "BoardStatusLed.h"`

```cpp
// #define USE_WS2812_LED                              // built-in WS2812 LED (typically ESP32 S3 DEV boards) - compact option
// #define USE_WS2812_LED_ADAFRUIT                     // built-in WS2812 LED Adafruit variant (requires Adafruit_NeoPixel library)
// #define USE_MONO_LED                                // built-in mono LED (some ESP32 DEV boards)
// #define USE_EXTERNAL_RGB_LED                        // external (not on board) RGB LED
// #define USE_EXTERNAL_MONO_LED                       // external (not on board) mono LED
```

2. Put macros in your code:

```cpp
LED(status)
```

Where `status` is one of the supported status modes. Neither object creation nor any initialization is required; just put `LED(status)` in your code where you need to update the LED status.

To change USER behavior, use:

```cpp
LEDUSER(R,G,B,I)
```

Where `R`, `G`, `B` are RGB color components, and `I` is the blinking interval in milliseconds.

**See examples:**
- Example1 - Illustration of all states
- Example2 - Memory allocation test
- Example3 - Board and WiFi diagnostics


## 🧠 Non-Blocking Operation

- LED handling runs on a dedicated Core 0 task
- Your application code runs independently and is never blocked
- **Example:** If `LED(INFO)` is set at startup, the LED continues breathing until a new status is selected

## 🏗 Architecture

```
Application
    |
    v
LedService (logic & patterns)
LedDriver (hardware abstraction)
    |
    v
Mono | RGB | WS2812 | Dummy
```

- **LedService** handles behavior (blink, breathe, rainbow)
- **LedDriver** abstracts hardware
- Hardware can be swapped without modifying application code


## 🔌 External Wiring (if no onboard LED)

### Mono LED:
```
GPIO → 220Ω resistor → LED → GND
```
Recommended resistor: 150Ω–250Ω

### RGB LED:
- One resistor (~220Ω) per color channel
- The longest pin is usually the common pin

**Common Cathode:**
- Common → GND
- LED ON → GPIO HIGH

**Common Anode:**
- Common → 3.3V
- LED ON → GPIO LOW


## 🌈 WS2812 Support

- Default implementation uses a lightweight driver (RMT-based)
- Optional support via Adafruit NeoPixel requires installing:
  - Adafruit NeoPixel library


## 📊 Memory Footprint (Approximate)

| Configuration | Flash Usage | Notes |
|---------------|-------------|-------|
| No LED (dummy) | Minimal | Only service layer |
| Mono LED | Very small | GPIO only |
| External RGB | Small | Uses LEDC PWM |
| WS2812 (RMT) | Small | Lightweight driver |
| WS2812 (Adafruit) | Larger | External dependency |

Exact values depend on the optimization level and ESP32 core version.


## 🧩 Example Project Structure

```
BoardStatusLED/
│
├── src/
│   ├── BoardStatusLed.h
│   └── BoardStatusLed.cpp
│
├── examples/
│   ├── Example1/
│   │   └── Example1.ino
│   ├── Example2/
│   │   └── Example2.ino
│   └── Example3/
│       └── Example3.ino
│
├── library.properties
├── LICENSE
└── README.md
```


## 🏷 Popular Boards

| Board | Built-in LED |
|-------|-------------|
| ESP32 WROOM/WROVER | No RGB |
| ESP32 DevKitC | Usually mono (GPIO2) |
| ESP32-S3 DevKitC-1 | Often WS2812 (GPIO48) |
| ESP32-C3 | Not supported (single core) |


## 🎯 Design Principles

- **KISS** (Keep It Simple and Small)
- Minimal memory footprint
- Compile-time configuration
- Hardware abstraction
- No runtime LED detection