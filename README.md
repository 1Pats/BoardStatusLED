ESP32 LED Status Indicator V2.0


A lightweight, non-blocking status LED library for dual-core ESP32 boards.

Provides a unified API for built-in or external LEDs without requiring changes to application code when switching LED types.



✨ Features

✅ Supports built-in and external LEDs

✅ Mono, RGB, and WS2812 support

✅ Fully non-blocking (Core 0 task)

✅ Compile-time LED type selection

✅ Dummy driver if no LED is present

✅ Lightweight (no mandatory dependencies)

✅ Optional Adafruit NeoPixel support



📦 Requirements

Dual-core ESP32 (not supported on ESP32-C3)

Arduino IDE (ESP32 core installed)



🔧 Supported LED Types

Built-in mono LED

Built-in WS2812

External mono LED

External RGB LED (common cathode or anode)

LED type is selected via #define at compile time.

No application code changes are required when switching LED hardware.



🚦 Supported Status Modes
| Status     | Description                                      |
|------------|--------------------------------------------------|
| OFF        | LED off                                          |
| OK         | Solid green                                      |
| INFO       | Green breathing effect                           |
| WORKING    | Green slow breathing effect                      |
| ARMED      | Light red, very slow breathing effect            |
| RETRIEVING | Sky blue, asymetric blinking                     |
| WAITING    | Yellow, very slow breathing effect               |
| CONNECTING | Blue heartbeat effect                            |
| WARNING    | Slow yellow blinking                             |
| ERROR      | Fast red blinking                                |
| RAINBOW    | RGB rainbow cycle (mono LED: solid ON)           |
| RED        | Solid red                                        |
| GREEN      | Solid green                                      |
| BLUE       | Solid blue                                       |
| YELLOW     | Solid red                                        |
| PINK       | Solid pink                                       |
| SKY        | Solid sky blue                                   |
| WHITE      | Solid white                                      |  
| USER       | Custom color and blink rate (Default: gray, fast blinking) |



🚀 Basic Usage

1. Enable the appropriate LED type by uncomenting a `#define` in file BoardStatusLEDSetup.h
Edit this fl in external editotor like notepad.
It is ebanled for ESP32 S3 by default- do nothing in this case

(Location of file is...libraries\BoardStatusLED\src\BoardStatusLedSetup.h)

```cpp
// #define USE_WS2812_LED                  // Built-in WS2812 LED (typically ESP32-S3 Dev boards) — compact option
// #define USE_WS2812_LED_ADAFRUIT         // Built-in WS2812 LED, Adafruit variant (requires Adafruit_NeoPixel library)
// #define USE_MONO_LED                    // Built-in mono LED (some ESP32 Dev boards)
// #define USE_EXTERNAL_RGB_LED            // External (not on-board) RGB LED
// #define USE_EXTERNAL_MONO_LED           // External (not on-board) mono LED

//In a case of external LED, you need to define GPIOs
if USE_EXTERNAL_MONO_LED 
//   #define EXT_MONO_LED_PIN

if USE_EXTERNAL_RGB_LED
//   #define RGB_BLUE_PIN                                                    
//   #define RGB_GREEN_PIN                                                     
//   #define RGB_RED_PIN         

2. Use the macros in your code:

LED(status)
```

where `status` is one of the supported status modes.

No object creation or initialization is required — just call `LED(status)` wherever you need to update the LED status.

To customize the USER status, use:

```cpp
LEDUSER(R, G, B, I)
```

where `R`, `G`, `B` are the RGB color components (0–255) and `I` is the blink interval in milliseconds.

See examples:

&nbsp; Example0 — bare minimum

&nbsp; Example1 — illustration of all states

&nbsp; Example2 — memory allocation test

&nbsp; Example3 — board and Wi-Fi diagnostics



🧠 Non-Blocking Operation

LED handling runs on a dedicated Core 0 task.

Your application code runs independently and is never blocked.

Example:

If `LED(INFO)` is set at startup, the LED continues breathing until a new status is selected.



🏗 Architecture
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

LedService handles behavior (blink, breathe, rainbow)

LedDriver abstracts hardware

Hardware can be swapped without modifying application code



🔌 External Wiring (if no on-board LED)

**Mono LED:**
```
GPIO → 220 Ω resistor → LED → GND
```
Recommended resistor: 150 Ω – 250 Ω

**RGB LED:**

One resistor (~220 Ω) per color channel

The longest pin is usually the common pin

**Common Cathode:**
- Common → GND
- LED ON → GPIO HIGH

**Common Anode:**
- Common → 3.3 V
- LED ON → GPIO LOW



🌈 WS2812 Support

The default implementation uses a lightweight RMT-based driver.

Optional support via the Adafruit NeoPixel library requires installing:

Adafruit NeoPixel library



📊 Memory Footprint (Approximate)
| Configuration        | Flash Usage | Notes                |
|----------------------|-------------|----------------------|
| No LED (dummy)       | Minimal     | Service layer only   |
| Mono LED             | Very small  | GPIO only            |
| External RGB         | Small       | Uses LEDC PWM        |
| WS2812 (RMT)         | Small       | Lightweight driver   |
| WS2812 (Adafruit)    | Larger      | External dependency  |

Exact values depend on the optimization level and ESP32 core version.



🧩 Example Project Structure
```
ESP32-LED-Status/
│
├── src/
│   ├── BoardStatusLED.h
│   ├── BoardStatusLED.cpp
│
├── examples/
│   └── Example1/
│       └── Example0.ino
│       └── Example1.ino
│       └── Example2.ino
│       └── Example3.ino
│
├── library.properties
├── LICENSE
└── README.md
```



🏷 Supported Popular Boards
| Board                  | Built-in LED                  |
|------------------------|-------------------------------|
| ESP32 WROOM/WROVER     | No RGB                        |
| ESP32 DevKitC          | Usually mono (GPIO 2)         |
| ESP32-S3 and successors| Often WS2812 (GPIO 48)        |
| ESP32-CX               | Not supported (single-core)   |

🎯 Design Principles

KISS (Keep It Simple and Small)

Minimal memory footprint

Compile-time configuration

Hardware abstraction

No runtime LED detection



📄 License

MIT License

