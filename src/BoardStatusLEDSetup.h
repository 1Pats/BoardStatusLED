// 1Pats February 2026
// Code developed according to KISS principle
// Enabling/disabling defines means excluding unnecessary code
// Library generates an LED driver only for your board

// Uncomment line to match your board
 #define USE_WS2812_LED                                                        // built in WS2812 LED (typically ESP32 S3 DEV boards)  compact option
// #define USE_WS2812_LED_ADAFRUIT                                             // built in WS2812 LED (typically ESP32 S3 DEV boards) Adafruit variant (requires AdaFruit_NeoPixel libray)
// #define USE_MONO_LED                                                        // built in LED (some of ESP32 DEV boards)
// #define USE_EXTERNAL_RGB_LED                                                // external (not on board) RGB led
// #define USE_EXTERNAL_MONO_LED                                               // external (not on board)) mono led

// Use these defines and your PINs if USE_EXTERNAL_RGB_LED
// #define RGB_BLUE_PIN        25                                              // tested on these pins
// #define RGB_GREEN_PIN       33                                              // 
// #define RGB_RED_PIN         32                                              // 
// use this define and your PIN if USE_EXTERNAL_MONO_LED
// #define EXT_MONO_LED_PIN    26                                              // tested on this pin