
/*
 * 1Pats February 2026  
 * Example2: Memory test. Illustration of BoardStatusLED library usage 
 * Allocate memory block by block till it is available. Print the amount of allocated memory
 * if memory is over, set error status  and stop working
 * Code developed according to KISS principle
*/


#include "BoardStatusLED.h"

#define DP(...)  Serial.printf(__VA_ARGS__)                                    // debug print:  DP has to be replaced with Serial.printf
//#define DP(...)                                                              //               DP have to be replaced with empty line          

void setup() {
    Serial.begin(115200);
    LED(CONNECTING);
    delay(2000);                                                               // wait 2 seconds
    LED(INFO);                                                                 // start green "breathing"
}
// Allocate memory block by block. Print the amount of allocated memory
// if memory is over, set error status on LED and stop working
#define MEM_SIZE_BLOCK 1024                                                    // block size
void loop() {
  static uint32_t ulMemTotal = 0;                                              // count of available memory - bytes
  delay(100);
  void *pVoid = malloc(MEM_SIZE_BLOCK);                                        // allocate next block
  if (pVoid != NULL) {                                                         // if success 
    ulMemTotal += MEM_SIZE_BLOCK;                                              // count it
    DP("Memory allocated:%d blocks (%d bytes) total\n", ulMemTotal/MEM_SIZE_BLOCK, MEM_SIZE_BLOCK); 
  }
  else {
    DP("Memory over\n");
    LED(ERROR);                                                                // LED blinks red 
    while(true);                                                               // stop here 
  }   
}