
/* 
 * Illustration of BoardStatusLED library usage
 * Sequentially changes the LED status
 * 1Pats February 2026
 * Update - May 2026
 * Please check #defines in ...\libraries\BoardStatusLed\src\BoardStatusLEDSetup.h
 * Code developed according to KISS principle
*/

#include "BoardStatusLED.h"

#define DP(...)  Serial.printf(__VA_ARGS__)                                    // Debug print macro
//#define DP(...)                                                              // Replace DP with empty definition to disable debug output      

void setup() {
    Serial.begin(115200);
    while (!Serial) delay(10);
    DP("\n************** BoardStatusLED Library Test ****************\n");
    LED(USER);        
    DP("USER\n");                                                             // initial status
    LEDUSER(10,25,35,100);                                                    // update user pattern for next time
}

void loop() {
    static unsigned long ulLastChange = 0;                                     // to check time
    if (millis() - ulLastChange < 10000) return;                               // change status every 10 seconds
    ulLastChange = millis();                                                   // time 
    static int iState = 0;                                                     // track current state
    switch (iState++%20){                                                      // cycle through patterns
        case 0: LED(CONNECTING);  DP("CONNECTING\n");   break;
        case 1: LED(OK);          DP("OK\n");           break;
        case 2: LED(INFO);        DP("INFO\n");         break;
        case 3: LED(WORKING);     DP("WORKING\n");      break; 
        case 4: LED(RAINBOW);     DP("RAINBOW\n");      break;
        case 5: LED(ARMED);       DP("ARMED\n");        break; 
        case 6: LED(RETRIEVING);  DP("RETRIEVNG\n");    break;
        case 7: LED(WAITING);     DP("WAITING\n");      break;
        case 8: LED(WARNING);     DP("WARNING\n");      break;
        case 9: LED(USER);        DP("USER\n");         break;
        case 10:LED(ERROR);       DP("ERROR\n");        break;
        case 11:LED(RED);         DP("RED\n");          break;
        case 12:LED(GREEN);       DP("GREEN\n");        break;                  
        case 13:LED(BLUE);        DP("BLUE\n");         break;
        case 14:LED(YELLOW);      DP("YELLOW\n");       break;
        case 15:LED(PINK);        DP("PINK\n");         break;
        case 16:LED(SKY);         DP("SKY\n");          break;  
        case 17:LED(WHITE);       DP("WHITE\n");        break;
        case 18:LED(OFF);         DP("OFF\n");          break;
        case 19:                  DP("PAUSE\n\n\n");    break;                  // Pasue, then do it again
       

    }  
}