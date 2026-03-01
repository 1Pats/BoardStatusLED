
/* 
 * Illustration of BoarDtatusLed library usage
 * Sequentially changes the LED status
 * 1Pats February 2026
 * Code developed according to KISS principle
*/


// Uncomment line to match your board
// #define USE_WS2812_LED                                                      // built in WS2812 LED (typically ESP32 S3 DEV boards)  compact option
// #define USE_WS2812_LED_ADAFRUIT                                             // built in WS2812 LED (typically ESP32 S3 DEV boards) Adafruit variant (requires AdaFruit_NeoPixel libray)
// #define USE_MONO_LED                                                        // built in LED (some of ESP32 DEV boards)
// #define USE_EXTERNAL_RGB_LED                                                // external (not on board) RGB led
// #define USE_EXTERNAL_MONO_LED                                               // external (not on board)) mono led
#include "BoardStatusLED.h"

#define DP(...)  Serial.printf(__VA_ARGS__)                                    // Debug print macro
//#define DP(...)                                                              // Replace DP with empty definition to disable debug output      

void setup() {
    Serial.begin(115200);
    LED(CONNECTING);                                                           // initial status
    // LEDUSER(15,15,15,100);                                                  // to update user pattern
}

void loop() {
    static unsigned long ulLastChange = 0;                                     // to check time
    if (millis() - ulLastChange < 10000) return;                               // change status every 10 seconds
    ulLastChange = millis();                                                   // time 
    static int iState = 0;                                                     // track current state
    switch (iState++%8){                                                       // cycle through patterns
        case 0:LED(CONNECTING);  DP("CONNECTING\n");   break;
        case 1:LED(OK);          DP("OK\n");           break;
        case 2:LED(INFO);        DP("INFO\n");         break;
        case 3:LED(RAINBOW);     DP("RAINBOW\n");      break;
        case 4:LED(WARNING);     DP("WARNING\n");      break;
        case 5:LED(USER);        DP("USER\n");         break;
        case 6:LED(ERROR);       DP("ERROR\n");        break;
        case 7:LED(OFF);         DP("OFF\n");          break;
    }  
}