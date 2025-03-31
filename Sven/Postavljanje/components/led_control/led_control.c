#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "driver/gpio.h"
#include "led_control.h"
#include "app_events.h"

extern QueueHandle_t mediator_to_led_queue;

#define GPIO_LED_BLUE    (14U)
#define GPIO_LED_RED     (26U)
#define GPIO_LED_GREEN   (27U)
#define GPIO_BIT_MASK(X) ((1ULL << (X)))
#define MIN_FLICKER_INTERVAL_MS 100

// --- Interna stanja ---
static bool red_on = false;
static bool green_on = false;
static bool blue_on = false;

static TimerHandle_t red_timer = NULL;
static TimerHandle_t green_timer = NULL;
static TimerHandle_t blue_timer = NULL;

static int red_interval_ms = 100;
static int green_interval_ms = 100;
static int blue_interval_ms = 100;

// --- Forward declarations ---
static void _led_init_pin(uint8_t pin);
static void _led_on(uint8_t pin);
static void _led_off(uint8_t pin);

// --- Public ---
void led_control_init(void)
{
    _led_init_pin(GPIO_LED_BLUE);
    _led_init_pin(GPIO_LED_GREEN);
    _led_init_pin(GPIO_LED_RED);
}

void led_set(led_color_t color, bool state)
{
    uint8_t pin = 0;
    switch (color) {
        case LED_BLUE:  pin = GPIO_LED_BLUE;  break;
        case LED_GREEN: pin = GPIO_LED_GREEN; break;
        case LED_RED:   pin = GPIO_LED_RED;   break;
        default: return;
    }

    state ? _led_on(pin) : _led_off(pin);
}

// --- Static privates ---
static void _led_init_pin(uint8_t pin)
{
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = GPIO_BIT_MASK(pin);
    gpio_config(&io_conf);
}

static void _led_on(uint8_t pin)  
{ 
    gpio_set_level(pin, 1); 
}

static void _led_off(uint8_t pin)
{ 
    gpio_set_level(pin, 0); 
}

// --- Flicker Timers ---
static void flicker_cb_red(TimerHandle_t xTimer)
{
    red_on = !red_on;
    led_set(LED_RED, red_on);
}

static void flicker_cb_green(TimerHandle_t xTimer)
{
    green_on = !green_on;
    led_set(LED_GREEN, green_on);
}

static void flicker_cb_blue(TimerHandle_t xTimer)
{
    blue_on = !blue_on;
    led_set(LED_BLUE, blue_on);
}

static void start_flicker(led_color_t color, int interval_ms)
{
    TimerHandle_t *timer = NULL;
    TimerCallbackFunction_t cb;
    int *interval_storage = NULL;

    switch (color) {
        case LED_RED:
            timer = &red_timer;
            cb = flicker_cb_red;
            interval_storage = &red_interval_ms;
            break;
        case LED_GREEN:
            timer = &green_timer;
            cb = flicker_cb_green;
            interval_storage = &green_interval_ms;
            break;
        case LED_BLUE:
            timer = &blue_timer;
            cb = flicker_cb_blue;
            interval_storage = &blue_interval_ms;
            break;
        default:
            return;
    }

    // Ako već postoji timer, interpretiraj ponovno pritiskanje kao gašenje
    if (*timer != NULL && xTimerIsTimerActive(*timer)) {
        xTimerStop(*timer, 0);
        led_set(color, false);
        return;
    }

    // Osiguraj minimalan interval
    if (interval_ms <= 0) {
        interval_ms = MIN_FLICKER_INTERVAL_MS;
    }

    *interval_storage = interval_ms;

    if (*timer == NULL) {
        *timer = xTimerCreate("led_flicker", pdMS_TO_TICKS(interval_ms), pdTRUE, NULL, cb);
    } else {
        xTimerChangePeriod(*timer, pdMS_TO_TICKS(interval_ms), 0);
    }

    if (*timer) {
        xTimerStart(*timer, 0);
    }
}
/*moguci rucni stoper za flicker ako ce nam trebat kasnije
---------------------------------------------------------
static void stop_flicker(led_color_t color)
{
    TimerHandle_t *timer = NULL;

    switch (color) {
        case LED_RED:   timer = &red_timer;   break;
        case LED_GREEN: timer = &green_timer; break;
        case LED_BLUE:  timer = &blue_timer;  break;
        default: return;
    }

    if (*timer && xTimerIsTimerActive(*timer)) {
        xTimerStop(*timer, 0);
        led_set(color, false);
    }
}
----------------------------------------------------------
*/

// --- Unified LED task ---
void led_handler_task(void *pv)
{
    AppMessage msg;
    while (1) {
        if (xQueueReceive(mediator_to_led_queue, &msg, portMAX_DELAY)) {
            switch (msg.command) {
                // --- SWITCH LED ---
                case CMD_LED_RED:
                    red_on = !red_on;
                    led_set(LED_RED, red_on);
                    break;

                case CMD_LED_GREEN:
                    green_on = !green_on;
                    led_set(LED_GREEN, green_on);
                    break;

                case CMD_LED_BLUE:
                    blue_on = !blue_on;
                    led_set(LED_BLUE, blue_on);
                    break;

                // --- FLICKER LED ---
                case CMD_LED_RED_FLICKER:
                    start_flicker(LED_RED, 100);
                    break;

                case CMD_LED_GREEN_FLICKER:
                    start_flicker(LED_GREEN, 100);
                    break;

                case CMD_LED_BLUE_FLICKER:
                    start_flicker(LED_BLUE, 100);
                    break;

                // --- STOP FLICKER  ---
                default:
                    break;
            }
        }
    }
}