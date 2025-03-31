#ifndef APP_EVENTS_H
#define APP_EVENTS_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

typedef enum {
    SOURCE_GUI
} SourceType;

typedef enum {
    CMD_LED_RED,
    CMD_LED_GREEN,
    CMD_LED_BLUE,
    CMD_LED_RED_FLICKER,
    CMD_LED_GREEN_FLICKER,
    CMD_LED_BLUE_FLICKER
} CommandType;

typedef struct {
    SourceType source;
    CommandType command;
    uint32_t flicker_delay_ms;
} AppMessage;

// Globalne queue varijable
extern QueueHandle_t led_switch_in;
extern QueueHandle_t mediator_to_led_queue;

#endif
