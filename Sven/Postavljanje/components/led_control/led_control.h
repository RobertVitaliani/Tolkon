#ifndef LED_CONTROL_H
#define LED_CONTROL_H

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Enum koji predstavlja boje LED-ica.
 */
typedef enum {
    LED_BLUE,
    LED_GREEN,
    LED_RED
} led_color_t;

/**
 * @brief Inicijalizira sve LED-ice.
 */
void led_control_init(void);

/**
 * @brief Pali ili gasi određenu LED-icu.
 *
 * @param color Boja LED-ice
 * @param state true = upali, false = ugasi
 */
void led_set(led_color_t color, bool state);

/**
 * @brief Glavni task koji upravlja LED-icama (switch/flicker logika).
 */
void led_handler_task(void *pv);

#endif

