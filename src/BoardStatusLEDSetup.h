// 1Pats February 2026
// Code developed according to the KISS principle
// Library generates an LED driver only for your board
// Modify this file to enable the right backend LED
// ESP32 S3 with onboard WS2812 LED is enabled by default, you do not need to change anything in this case

// Uncomment the line to match your board. 
#define USE_WS2812_LED                                                         // built in WS2812 LED (typically ESP32 S3 DEV boards) compact option
// #define USE_WS2812_LED_ADAFRUIT                                             // built in WS2812 LED (typically ESP32 S3 DEV boards) Adafruit variant (requires AdaFruit_NeoPixel libray
// #define USE_MONO_LED                                                        // built in LED (some of ESP32 DEV boards)
// #define USE_EXTERNAL_RGB_LED                                                // external (not on board) RGB led
// #define USE_EXTERNAL_MONO_LED                                               // external (not on board)) mono led

// Use these defines and your PINs if USE_EXTERNAL_RGB_LED
// #define RGB_BLUE_PIN        10                                              // tested on these pins
// #define RGB_GREEN_PIN       9                                               // put your pin numbers here
// #define RGB_RED_PIN         8                                               // 

// use this define and your PIN if USE_EXTERNAL_MONO_LED
// #define EXT_MONO_LED_PIN    12                                              // tested on this pin. Put your PIN

