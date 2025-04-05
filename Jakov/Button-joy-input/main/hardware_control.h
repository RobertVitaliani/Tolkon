#ifndef HARDWARE_CONTROL_H
#define HARDWARE_CONTROL_H

#include "driver/adc.h"  // For adc_channel_t, etc.
#include "esp_err.h"     // For esp_err_t

typedef void (*button_pressed_isr_t )( void *p_arg );

// Constants (extern to avoid multiple definitions)
extern const uint8_t btn1PIN;
extern const uint8_t btn2PIN;
extern const uint8_t btn3PIN;
extern const uint8_t btn4PIN;
extern const adc_channel_t joyXChannel;
extern const adc_channel_t joyYChannel;
extern const uint64_t debounceTime;

// Global variables (extern)
extern uint8_t btn1PressN;
extern uint64_t btn1PressT;
extern uint8_t btn2PressN;
extern uint64_t btn2PressT;
extern uint8_t btn3PressN;
extern uint64_t btn3PressT;
extern uint8_t btn4PressN;
extern uint64_t btn4PressT;

extern int joyXValue;
extern int joyYValue;

// Function declarations (no static unless defined here)

esp_err_t btnInit( uint8_t btnPin, button_pressed_isr_t p_isr );
void IRAM_ATTR btn1ISR( void *p_arg );
void IRAM_ATTR btn2ISR( void *p_arg );
void IRAM_ATTR btn3ISR( void *p_arg );
void IRAM_ATTR btn4ISR( void *p_arg );
adc_oneshot_unit_handle_t adcInit( adc_channel_t adcChannel );

#endif