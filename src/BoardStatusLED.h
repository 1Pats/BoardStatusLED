
#pragma once
#include <Arduino.h>
#include "BoardStatusLedSetup.h"
 

#define BREATH_SPEED          (uint32_t)(0.002 * float(1 << 14))              // breath speed, but use unsigned long to save it
#define RAINBOW_STEP          (uint32_t)(PI/180* float(1 << 14))              // RAINBOW  change - one degree per call
#define BLINK_INTERVAL_FAST   250                                             // milliseconds, blink rate for ERROR
#define BLINK_INTERVAL_SLOW   1000                                            // ms WARNING blink rate
#define BLINK_INTERVAL_CUSTOM 100                                             // ms, default blink rate for USER

enum  eStatuses {SOLID, BLINK_FAST, BLINK_SLOW, BREATH, RAINBOW, HEARTBEAT, CUSTOM, OFF};   // enumerated LED patterns

struct sLedStatusConfig {                                                     // defines LED color and behavior configuration
    uint8_t   bR, bG, bB;                                                     // red, green, blue 
    uint32_t  ulParameter;                                                    // blink interval, breath speed
    eStatuses ePattern;                                                       // enumerted patterns
};


namespace LEDStatus {                                                         // namespace is used to isolate commonly used keywords
    extern const sLedStatusConfig ERROR;
    extern const sLedStatusConfig WARNING;
    extern const sLedStatusConfig INFO;
    extern const sLedStatusConfig OK;
    extern const sLedStatusConfig CONNECTING;
    extern const sLedStatusConfig RAINBOW;
    extern const sLedStatusConfig OFF;
    extern       sLedStatusConfig USER;                                       // no const because user can modify
}

class cLedDriver  {                                                            // base LED driver providing basic interface functions
 public:
    cLedDriver() {};                                                           // constructor
    ~cLedDriver() {};                                                          // destructor
    virtual void vBegin() {};                                                  // initialize led driver
    virtual void vSetColor(uint8_t, uint8_t, uint8_t) {};                      // set RGB color
    virtual void vOn()  {vSetColor(255,255,255);}                              // turn LED on (white) 
    virtual void vOff() {vSetColor(0,   0,  0);}                               // turn LED off
};

class cDummyLed : public cLedDriver  {                                         // minimal implementation when no LED is present
public:                                                                        // forces the compiler to override virtual methods
   void vSetColor(uint8_t, uint8_t, uint8_t) override {}                       // do nothing
};

// Mono LED support
#if defined(USE_MONO_LED) || defined(USE_EXTERNAL_MONO_LED)
class cMonoLed : public cLedDriver  {                                          // this is mono led instance
    int iPin;                                                                  // pin where the LED is connected (built-in or external)
public:
    cMonoLed(int gpio);                                                        // constructor: LED PIN as argument
public:
    void vBegin() override;                                                    // initilize driver
    void vSetColor(uint8_t bR, uint8_t bG, uint8_t bB) override;               // reduce RGB input to simple ON/OFF behavior
};
#endif

#ifdef USE_WS2812_LED_ADAFRUIT                                                 // variant using Adafruit NeoPixel library
#include <Adafruit_NeoPixel.h>
class cWS2812Led : public cLedDriver {
    Adafruit_NeoPixel oPixel;
public:
    cWS2812Led(int iPin);
    void vBegin() override; 
    void vSetColor(uint8_t bR, uint8_t bG, uint8_t bB) override;
};
#endif

#ifdef USE_WS2812_LED                                                          // variant with RMT driver (Bare-Metal RMT)
#include "driver/rmt_tx.h"
#include "esp_err.h"
#include "esp_rom_sys.h"
class cWS2812Led : public cLedDriver {
    gpio_num_t iPin;
    rmt_channel_handle_t tx_chan = nullptr;
    rmt_encoder_handle_t copy_encoder = nullptr;
    static constexpr uint32_t RESOLUTION_HZ = 20*1000*1000; // 20 MHz (0.05µs per tick)
    static constexpr uint16_t T0H = 8;   // 0.4µs WS2812 timing (at 20 MHz resolution)
    static constexpr uint16_t T0L = 17;  // 0.85µs
    static constexpr uint16_t T1H = 16;  // 0.8µs
    static constexpr uint16_t T1L = 9;   // 0.45µs
public:
    cWS2812Led(int iPin);
    void vBegin() override; 
    void vSetColor(uint8_t bR, uint8_t bG, uint8_t bB) override;
};
#endif

#ifdef USE_EXTERNAL_RGB_LED                                                    // external RGB LED (used when no onboard LED is available)
#include "driver/ledc.h"
class cRGBLed: public cLedDriver {
    int iRPin, iGPin, iBPin;
    bool bCommonAnode;
    static constexpr uint32_t         PWM_FREQ  = 5000;                        // 5 kHz
    static constexpr ledc_timer_bit_t PWM_RES   = LEDC_TIMER_8_BIT;
    static constexpr ledc_mode_t      PWM_MODE  = LEDC_LOW_SPEED_MODE;
    static constexpr ledc_timer_t     PWM_TIMER = LEDC_TIMER_0;
    static constexpr ledc_channel_t   R_CHANNEL = LEDC_CHANNEL_0;
    static constexpr ledc_channel_t   G_CHANNEL = LEDC_CHANNEL_1;
    static constexpr ledc_channel_t   B_CHANNEL = LEDC_CHANNEL_2;
public:
    cRGBLed(int, int, int, bool);
    void vBegin() override;
    void vSetColor(uint8_t r, uint8_t g, uint8_t b) override;
};
#endif

class cLedServiceBase {                                                       //  base service, create pure virtual class
public:
  virtual ~cLedServiceBase() {}                                               // virtual destructor 
  virtual void vBegin() = 0;                                                  // define pure virtual functions
  virtual void vSetStatus(const sLedStatusConfig&) = 0;                       // set led behaviour pattern
  virtual void vUpdate() = 0;                                                 // keep led (status) alive
  virtual void vSetUser(const uint8_t, const uint8_t, const uint8_t, const uint32_t) = 0;  // a special function which allows to change USER/CUSTOM parameters
};

class cLedServiceNull : public cLedServiceBase {                               // class, if no led at all
public:                                                                        // just frame, no code
  void vBegin() override {}                                                    // define functions which do nothing
  void vSetStatus(const sLedStatusConfig&) override {}
  void vUpdate() override {}
  void vSetUser(const uint8_t, const uint8_t, const uint8_t, const uint32_t) override {}
}; 

class cLedService : public cLedServiceBase{                                    // main LED service implementation
private:
     cLedDriver *_Led  = NULL;                                                 // pointer to LED driver
     uint32_t _lLastUpdate = 0;                                                // timestamp of last update 
     sLedStatusConfig _ActiveStatus = LEDStatus::OFF;                          // currently active status
public:                                                                        // 
   cLedService(cLedDriver* oDrv) : _Led(oDrv) {}                               // constructor - accepts pointer to LED driver
   void vBegin() override;                                                     // initialize service and LED driver
   void vSetUser(const uint8_t, const uint8_t, const uint8_t, const uint32_t) override;   // set USER/custom parameters
   void vSetStatus(const sLedStatusConfig&) override;                          // set LED status     
   void vUpdate() override;                                                    // update LED state
private:                                                                       // internal behaviour functions 
   void vBlink(unsigned long);                                                 // blink pattern
   void vHeartBeat(unsigned long);                                             // heartbeat pattern
   void vBreathe(unsigned long);                                               // breathing pattern
   void vRainbow();                                                            // rainbow pattern
};

extern cLedServiceBase* pLedService; 

// Macros for user code
#define LED(status) pLedService->vSetStatus(LEDStatus::status)                  // macro- set led state
#define LEDUSER(R,G,B,I) pLedService->vSetUser(R,G,B,I)                         // macro to change USER/CUSTOM parameters: RGB- color(0,255) and I-interval