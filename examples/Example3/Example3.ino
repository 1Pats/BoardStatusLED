/* 
 * ESP32 Diagnostics
 * 1Pats February 2026
 * Detects various ESP32 parameters and illustrates status LED usage
 * Code developed according to KISS principle
*/

#include "esp_now.h"
#include <esp_wifi.h>
#include "WiFi.h"
#include "esp_chip_info.h"
#include "esp_app_desc.h"

// LED: Uncomment line to match your board
 #define USE_WS2812_LED                                                      // built in WS2812 LED (typically ESP32 S3 DEV boards)  compact option
// #define USE_WS2812_LED_ADAFRUIT                                             // built in WS2812 LED (typically ESP32 S3 DEV boards) Adafruit variant (requires AdaFruit_Neopicel libray)
// #define USE_MONO_LED                                                        // built in LED (some of ESP32 DEV boards)
// #define USE_EXTERNAL_RGB_LED                                                // external (not on board) RGB led
// #define USE_EXTERNAL_MONO_LED                                               // external (not on board)) mono led

// Use these defines and your PINs if USE_EXTERNAL_RGB_LED
// #define RGB_BLUE_PIN        25                                              // testing on these pins
// #define RGB_GREEN_PIN       33                                              // testing on these pins
// #define RGB_RED_PIN         32                                              // testing on these pins
// use this define and your PIN, if USE_EXTERNAL_MONO_LED
// #define EXT_MONO_LED_PIN    26   


#include "BoardStatusLED.h"


uint8_t uiGetESPNOWVersion(){
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
     return 0xFF;
  }
  uint32_t lVersion = 0;
  esp_now_get_version(&lVersion);
  if (lVersion == 0x0100) lVersion = 1;
  else if (lVersion == 0x0200) lVersion = 2;
  return (uint8_t)lVersion;
}

void vScanNetworks(){
  
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  Serial.println("---- Available WiFi:");
  int n = WiFi.scanNetworks();
  for (int i = 0; i < n; ++i) {
    Serial.print(WiFi.SSID(i));
    Serial.print(" - Auth:");
    Serial.println(WiFi.encryptionType(i)); // 7=WPA3, 5=WPA2, etc.
  }
}

void printResetReason() {
  esp_reset_reason_t reason = esp_reset_reason();
  if (reason == 1) return;
  LED(ERROR);
  Serial.print("Reset reason: ");
  Serial.println(reason);
}
     
void setup() {
  Serial.begin(115200);
  LED(INFO);
  delay(2000);
  LED(RAINBOW); 
  Serial.printf("\n\n    ESP32 DIAGNOSTICS    \n");
  esp_chip_info_t chip_info;
  esp_chip_info(&chip_info);
  Serial.printf("Chip Model          :%s\n",         ESP.getChipModel());      // Chip model              
  Serial.printf("Chip Revision       :%d\n",         ESP.getChipRevision()/100); // Chip revision
  Serial.printf("CPU Cores           :%d\n",         chip_info.cores);         // Cores
  Serial.printf("CPU Frequency       :%d (MHz)\n",   getCpuFrequencyMhz());    // CPU fequency
  Serial.printf("ESP-IDF Version     :%s\n",         esp_get_idf_version());   // IDF version
  Serial.printf("Arduino Core Version:%s\n",         ESP.getSdkVersion());     // Core vesion
  Serial.printf("Flash Size          :%d (bytes)\n", ESP.getFlashChipSize());  // Flash memory size 
  Serial.printf("Flash Speed         :%d (Hz)\n",    ESP.getFlashChipSpeed()); // Flash speed 
  Serial.printf("Free Heap           :%d (bytes)\n", ESP.getFreeHeap());       // Free heap
  Serial.printf("Min Free Heap       :%d (bytes)\n", ESP.getMinFreeHeap());    // Minimums free heap 
  Serial.printf("Max Alloc Heap      :%d (bytes)\n", ESP.getMaxAllocHeap());   // Maximum free heap
  
  if (psramFound()) {
      Serial.printf("PSRAM Size          :%d (bytes)\n",ESP.getPsramSize());   // PSRAM size   
      Serial.printf("Free PSRAM          :%d (bytes)\n",ESP.getFreePsram());   // free size
  } else Serial.printf("PSRAM               :NOT FOUND\n");
  WiFi.mode(WIFI_STA);                                                         // required to read mac address
  uint8_t bMac[6];                                                             // 6 bytes to store MAC address
  esp_wifi_get_mac(WIFI_IF_STA, bMac);                                         // read mac address
  Serial.printf("WiFi MAC            :");
  Serial.printf("%02x:%02x:%02x:%02x:%02x:%02x\n", bMac[0],bMac[1],bMac[2],bMac[3],bMac[4],bMac[5]);
  uint8_t uiEspNowVersion= uiGetESPNOWVersion();                               // Suppoted ESP NOW version
  Serial.printf("ESP NOW Version     :V%d\n",uiEspNowVersion);

  const esp_app_desc_t *pAppDesc = esp_app_get_description();                  // Dont know what it means
  Serial.printf("Magic word          :%d\n", pAppDesc->magic_word);    
  Serial.printf("Secure vesion       :%d\n", pAppDesc->secure_version);    
  Serial.printf("Application version :%d\n", pAppDesc->version);    
  Serial.printf("Project Name        :%s\n", pAppDesc->project_name);    
  Serial.printf("Compile time        :%s\n", pAppDesc->time);    
  Serial.printf("Compile date        :%s\n", pAppDesc->date);    
  Serial.printf("Version IDF         :%s\n", pAppDesc->idf_ver);    
  Serial.printf("sha256 of elf file  :%d\n", pAppDesc->app_elf_sha256);    
  Serial.printf("min_efuse_blk_rev_full:%d\n", pAppDesc->min_efuse_blk_rev_full);   
  Serial.printf("max_efuse_blk_rev_full:%d\n", pAppDesc->max_efuse_blk_rev_full);    
  Serial.printf("mmu_page_size       :%d\n", pAppDesc->mmu_page_size);    

  LED(CONNECTING);
  vScanNetworks();                                                             // print out available WiFi networks
  LED(WARNING);
  delay(1000);
  printResetReason();                                                          
  LED(USER);
}

// just to illustrate USER/Custom LED usage
void loop() {
   static uint32_t ulCounter = 0;                      // tlops counter
   if (++ulCounter/1024 != 0) return;                  // otherwise too frequently
   byte bG = byte(32+ ulTime/1024);                    // change green component in Color
   LEDUSER(32,bG,32,ulCounter/1024+100);               // blinking is asynchronous, LED(USER) call is not required, it is set in setup()                                                               
}
