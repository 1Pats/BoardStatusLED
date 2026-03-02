#include "BoardStatusLED.h"

namespace LEDStatus {                                                                               // namespace here is used to isolate popular keywords
// define patterns                  R    G   B     Parameter              Pattern
const sLedStatusConfig ERROR     = {255,  0,  0, BLINK_INTERVAL_FAST,    eStatuses::BLINK_FAST};    // red, blinking fast
const sLedStatusConfig WARNING   = {127, 127, 0, BLINK_INTERVAL_SLOW,    eStatuses::BLINK_SLOW};    // yellow, blinking slow
const sLedStatusConfig INFO      = {  0,255,  0, BREATH_SPEED,           eStatuses::BREATH};        // green, breathing
const sLedStatusConfig OK        = {  0,255,  0, 0,                      eStatuses::SOLID};         // green, solid
const sLedStatusConfig CONNECTING= {  0,  0,255, 0,                      eStatuses::HEARTBEAT};     // blue, heartbeat
const sLedStatusConfig RAINBOW   = {  0,  0,  0, RAINBOW_STEP,           eStatuses::RAINBOW};       // rainbow, color wheel
const sLedStatusConfig OFF       = {  0,  0,  0, 0,                      eStatuses::OFF};           // just to switch led off
      sLedStatusConfig USER      = {  15,15, 15, BLINK_INTERVAL_CUSTOM,  eStatuses::CUSTOM};        // default: gray, blinking very fast 
}

// base LED driver with basic functions
#ifdef USE_MONO_LED                                                            // some ES32 Dev Boards
cMonoLed::cMonoLed(int gpio): iPin(gpio) {}                                    // constructor
void cMonoLed::vBegin() override {                                             // initial commands 
     pinMode(iPin, OUTPUT);                                                    // set pin to output
     digitalWrite(iPin, LOW);                                                  // switch led off                                         ;
}
void cMonoLed::vSetColor(uint8_t bR, uint8_t bG, uint8_t bB){                  // reduce to on/off
     digitalWrite(iPin, (bR||bG||bB) ? HIGH : LOW);                            // set on, if any color, off, if all 0
}
#endif

#ifdef USE_WS2812_LED_ADAFRUIT                                                // typically ESP32 S3
cWS2812Led::cWS2812Led(int iPin): oPixel(1, iPin, NEO_GRB+NEO_KHZ800) {}      // implementation based on Adafruit_NeoPixel library
void cWS2812Led::vBegin(){
    oPixel.begin();
    oPixel.show();
}
void cWS2812Led::vSetColor(uint8_t bR, uint8_t bG, uint8_t bB){
    oPixel.setPixelColor(0, bR, bG, bB);
    oPixel.show();
}
#endif
#include "BoardStatusLed.h"
#ifdef USE_WS2812_LED 
cWS2812Led::cWS2812Led(int gpio) : iPin((gpio_num_t)gpio) {}
void cWS2812Led::vBegin() {
     rmt_tx_channel_config_t tx_config = {};
     tx_config.gpio_num = iPin;
     tx_config.clk_src = RMT_CLK_SRC_DEFAULT;
     tx_config.resolution_hz = RESOLUTION_HZ;
     tx_config.mem_block_symbols = 64;   // enough for 24 bits
     tx_config.trans_queue_depth = 4;
     ESP_ERROR_CHECK(rmt_new_tx_channel(&tx_config, &tx_chan));
     ESP_ERROR_CHECK(rmt_enable(tx_chan));
     rmt_copy_encoder_config_t encoder_config = {};                            // required encoder
     ESP_ERROR_CHECK(rmt_new_copy_encoder(&encoder_config, &copy_encoder));
}
void cWS2812Led::vSetColor(uint8_t bR, uint8_t bG, uint8_t bB){
     uint32_t color = ((uint32_t)bG << 16) | ((uint32_t)bR << 8)  | bB;
     rmt_symbol_word_t symbols[24];
     for (int i = 0; i < 24; i++) {
         bool bit = color & (1 << (23 - i));
         symbols[i].level0 = 1;
         symbols[i].duration0 = bit ? T1H : T0H;
         symbols[i].level1 = 0;
         symbols[i].duration1 = bit ? T1L : T0L;
      }
      rmt_transmit_config_t transmit_config = {};
      transmit_config.loop_count = 0;
      ESP_ERROR_CHECK(rmt_transmit(tx_chan,copy_encoder,symbols,sizeof(symbols),&transmit_config));
      ESP_ERROR_CHECK(rmt_tx_wait_all_done(tx_chan, portMAX_DELAY));
      esp_rom_delay_us(60);                                                    // reset pulse (>50µs)
}
#endif

#ifdef USE_EXTERNAL_RGB_LED   
cRGBLed::cRGBLed(int bR, int bG, int bB, bool bAnode) : iRPin(bR), iGPin(bG), iBPin(bB), bCommonAnode(bAnode) {}
void cRGBLed::vBegin() {
        ledc_timer_config_t timer_conf = {};                // Ccnfigure timer          
        timer_conf.speed_mode       = PWM_MODE;
        timer_conf.timer_num        = PWM_TIMER;
        timer_conf.duty_resolution  = PWM_RES;
        timer_conf.freq_hz          = PWM_FREQ;
        timer_conf.clk_cfg          = LEDC_AUTO_CLK;
        ledc_timer_config(&timer_conf);
        ledc_channel_config_t r_conf = {};                 // configure R channel
        r_conf.gpio_num   = iRPin;
        r_conf.speed_mode = PWM_MODE;
        r_conf.channel    = R_CHANNEL;
        r_conf.intr_type  = LEDC_INTR_DISABLE;
        r_conf.timer_sel  = PWM_TIMER;
        r_conf.duty       = 0;
        r_conf.hpoint     = 0;
        ledc_channel_config(&r_conf);
        ledc_channel_config_t g_conf = r_conf;             // configure G channel
        g_conf.gpio_num = iGPin;
        g_conf.channel  = G_CHANNEL;
        ledc_channel_config(&g_conf);       
        ledc_channel_config_t b_conf = r_conf;             // configure B channel
        b_conf.gpio_num = iBPin;
        b_conf.channel  = B_CHANNEL;
        ledc_channel_config(&b_conf);
        vSetColor(0, 0, 0);
}

void cRGBLed::vSetColor(uint8_t r, uint8_t g, uint8_t b) {
        if (bCommonAnode){
            r = 255 - r; g = 255 - g; b = 255 - b;
        }
        ledc_set_duty(PWM_MODE, R_CHANNEL, r);
        ledc_update_duty(PWM_MODE, R_CHANNEL);
        ledc_set_duty(PWM_MODE, G_CHANNEL, g);
        ledc_update_duty(PWM_MODE, G_CHANNEL);
        ledc_set_duty(PWM_MODE, B_CHANNEL, b);
        ledc_update_duty(PWM_MODE, B_CHANNEL);
    }
}
#endif

SemaphoreHandle_t  _batton = NULL;                                             // batton (mutex) to synchronize LED updates with core 0
void _vUpdateLED(void *);                                                      // task function prototype


void cLedService::vBegin() {
     _Led->vBegin();                                                           // initialize driver   
     _batton = xSemaphoreCreateMutex();                                        // crete mutex for synchronization
     xSemaphoreTake(_batton,portMAX_DELAY);                                    // prevent core 0 for early activation
     xTaskCreatePinnedToCore(                                                  // create task pined to core 0 to handle LED updaye
        _vUpdateLED,                                                           // task on core 0
        "StatusLed",                                                           // name -to identify it in diagnostic messages  
        4096,                                                                  // stack size
        this,                                                                  // parameter passed  - pointer to this class
        1,                                                                     // priority
        NULL,                                                                  // task handle
        0);                                                                    // core 0                                                                               
     xSemaphoreGive(_batton);
}

void cLedService::vSetUser(uint8_t bR, uint8_t bG, uint8_t bB, uint32_t ulInterval = 0){  // set user-defined colors and blinking interval
     LEDStatus::USER.bR = bR;  LEDStatus::USER.bG = bG;  LEDStatus::USER.bB = bB;         // set new color
     if (ulInterval > 0)  LEDStatus::USER.ulParameter = (uint32_t)ulInterval;             // set new blinking interval
}
void cLedService::vSetStatus(const sLedStatusConfig& oStatus){
     if (_lLastUpdate == 0) vBegin();                                          // check if initialized 
     _ActiveStatus = oStatus;                                                  // set new status
     _lLastUpdate = millis();                                                  // reset timestamp
}

void cLedService::vUpdate() {
      unsigned long ulNow = millis();                                          // current time
      xSemaphoreTake(_batton,portMAX_DELAY);                                   // prevent core 0 task from running while updating pattern
      switch(_ActiveStatus.ePattern) {                                         // resume updates                    
            case OFF:        _Led->vOff();           break;
            case BREATH:     vBreathe(ulNow);        break;
            case SOLID:      _Led->vSetColor(_ActiveStatus.bR,_ActiveStatus.bG,_ActiveStatus.bB); break;
            case CUSTOM: 
            case BLINK_SLOW:                
            case BLINK_FAST: vBlink(ulNow);          break;
            case HEARTBEAT:  vHeartBeat(ulNow);      break;
            case RAINBOW:    vRainbow();             break;
      }
       xSemaphoreGive(_batton);                                                // start to update
}

#include <math.h>                                                              // required for sin(), cos()...
void cLedService::vBlink(unsigned long ulNow) {                                // toggle led on / off
     int iInterval = _ActiveStatus.ulParameter;                                // blinking interval in miliseconds 
     static bool bState = false;                                               // track led state             
     if (ulNow - _lLastUpdate >= iInterval){                                   // if time to change state
         _lLastUpdate = ulNow;                                                 // reset time
         bState = !bState;                                                     // toggle state
         if (bState)  _Led->vSetColor(_ActiveStatus.bR,_ActiveStatus.bG,_ActiveStatus.bB);   // turn LED on with defied color
         else _Led->vOff();                                                    // turn LED off 
      }
} 

void cLedService::vHeartBeat(unsigned long ulNow) {                            // simulate heartbeat pattern
     const int iTimings[]={120,180,120,900};                                   // define heartbeat timing sequence
     static uint8_t iStep = 0;                                                 // track current step
     if (ulNow - _lLastUpdate >= iTimings[iStep]){                             // check timing  
        _lLastUpdate = ulNow;                                                  // rememeber timestamp
        if ((iStep == 0) || (iStep == 2)) _Led->vSetColor(_ActiveStatus.bR,_ActiveStatus.bG,_ActiveStatus.bB);   // turn LED on for pulse
        else _Led->vOff();                                                     // turm LED off between pulses
        iStep = (iStep+1)%4;                                                   // 4 step cycle
     }
}

void cLedService::vBreathe(unsigned long ulNow) {                              // simulate breathing effect
     unsigned long ulDelta = ulNow-_lLastUpdate;                               // time since last call
     _lLastUpdate = ulNow;                                                     // reset timestamp
     float fSpeed =  float(_ActiveStatus.ulParameter)/float(1 << 14);          // convert stored speed value                
     static float fBreathPhase = 0;                                            // track breathing phase
     fBreathPhase += ulDelta*fSpeed;                                           // value depends on time. not on count
     while (fBreathPhase > TWO_PI) fBreathPhase -= TWO_PI;                     // keep phase within range
     float fIntensity = (1.0f - cos(fBreathPhase))*0.5;                        // simulate smooth breathing curve
     fIntensity = pow(fIntensity, 2.2);                                        // apply gamma correctionargument
     _Led->vSetColor(_ActiveStatus.bR*fIntensity, _ActiveStatus.bG*fIntensity, _ActiveStatus.bB*fIntensity);   // display intensity
}

void cLedService::vRainbow(){                                                  // pseido rainbow effect
      static float fAngle = 0;                                                 // track angle
      fAngle += float(_ActiveStatus.ulParameter)/float(1 << 14);               // increase angle each call
      uint8_t bR = (uint8_t)(127.5 + 127.5 * sin(fAngle));                     // red component 
      uint8_t bG = (uint8_t)(127.5 + 127.5 * sin(fAngle + 2.094395));          // +120° green
      uint8_t bB = (uint8_t)(127.5 + 127.5 * sin(fAngle + 4.188790));          // +240° blue 
      _Led->vSetColor(bR, bG, bB);                                             // display color
}

void _vUpdateLED(void *voidPointer){                                           // core 0 task, (not a class function) 
   while(true){                                                                // infinite loop
       xSemaphoreTake(_batton,portMAX_DELAY);                                  // wait until core 1 finishes update
       xSemaphoreGive(_batton);                                                // allow core 1 to continue 
       cLedService *pLedService = (cLedService *)voidPointer;                  // cast parameter to correct type
       pLedService->vUpdate();                                                 // keep LED updated
       vTaskDelay(pdMS_TO_TICKS(25));                                          // Yield to scheduler (minimum 25 ms)
    }                                                        
}

// Instantiate LED depending on configuration
#if defined(USE_MONO_LED)                                                      // on-board mono LED
#ifndef MONO_LED_PIN
  #define MONO_LED_PIN         2                                               // typically internal pin 2 used
#endif
  cMonoLed oBoardLed(MONO_LED_PIN);                                            // mono led driver
  cLedDriver* pLedPtr = &oBoardLed;                                            // pointer to driver
#elif defined(USE_EXTERNAL_MONO_LED)                                           // external mono led
#ifndef EXT_MONO_LED_PIN
  #define EXT_MONO_LED_PIN    26                                               // testing on this pin
#endif
  cMonoLed oBoardLed(EXT_MONO_LED_PIN);                                        // external mono led
  cLedDriver* pLedPtr = &oBoardLed;                                            // pointer to driver 
#elif defined(USE_EXTERNAL_RGB_LED)                                            // External RGB led
#ifndef RGB_BLUE_PIN
  #define RGB_BLUE_PIN        25                                               // testing on these pins
#endif
#ifndef RGB_GREEN_PIN
  #define RGB_GREEN_PIN       33                                               // testing on these pins
#endif
#ifndef RGB_RED_PIN
  #define RGB_RED_PIN         32                                               // testing on these pins
#endif 
  cRGBLed oBoardLed(RGB_RED_PIN,RGB_GREEN_PIN,RGB_BLUE_PIN, false);            // external rgb led false - common 
  cLedDriver* pLedPtr = &oBoardLed;                                            // pointer tp led driver 
#elif defined(USE_WS2812_LED) || defined(USE_WS2812_LED_ADAFRUIT)              // two variants supported for WS2812
  cWS2812Led oBoardLed(WS2812_LED_PIN);                                        // WS2812 led (typically on ESP32 S3)
  cLedDriver* pLedPtr = &oBoardLed;                                            // pointer to led driver 
#else
  cDummyLed oBoardLed;                                                         // dummy driver (no LED)
  cLedDriver* pLedPtr = &oBoardLed;                                            // pointer to led
#endif

#if defined(USE_WS2812_LED) || defined(USE_WS2812_LED_ADAFRUIT) || defined(USE_MONO_LED) || defined(USE_EXTERNAL_RGB_LED) || defined(USE_EXTERNAL_MONO_LED)
    cLedService oLedService(pLedPtr);                                          // real LED service
    cLedServiceBase* pLedService = &oLedService;   
#else                                         
    cLedServiceNull oLedServiceNull;                                           // no LED service fallback
    cLedServiceBase* pLedService = &oLedServiceNull;

#endif
