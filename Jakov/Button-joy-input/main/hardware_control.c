#include "driver/adc.h"
#include "esp_adc/adc_oneshot.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>
#include <esp_event.h>
#include <esp_timer.h>
#include "esp_mac.h"
#include "driver/adc.h"
#include "esp_adc/adc_oneshot.h"

#include "hardware_control.h"

// Constant definitions
const uint8_t btn1PIN = 36;
const uint8_t btn2PIN = 32;
const uint8_t btn3PIN = 33;
const uint8_t btn4PIN = 25;

const uint64_t debounceTime = 200000;

const adc_channel_t joyXChannel = ADC_CHANNEL_6;
const adc_channel_t joyYChannel = ADC_CHANNEL_7;

// Global variable definitions
uint8_t btn1PressN = 0;
uint64_t btn1PressT = 0;
uint8_t btn2PressN = 0;
uint64_t btn2PressT = 0;
uint8_t btn3PressN = 0;
uint64_t btn3PressT = 0;
uint8_t btn4PressN = 0;
uint64_t btn4PressT = 0;

int joyXValue = 0;
int joyYValue = 0;

esp_err_t btnInit( uint8_t btnPin, button_pressed_isr_t p_isr )
{
    static bool isr_service_installed = false; // Ensures only one installation

    // Configure the GPIO.
    gpio_config_t io_conf =
    {
        .pin_bit_mask = ( 1ULL << btnPin ),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLDOWN_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_POSEDGE,
    };

    esp_err_t esp_err = gpio_config(&io_conf );

    if ( ESP_OK == esp_err )
    {
        // Change gpio interrupt type for a pin.
        esp_err = gpio_set_intr_type( btnPin, io_conf.intr_type );
    }

    if ( ESP_OK == esp_err && !isr_service_installed )
    {
        // Install gpio isr service only once.
        esp_err = gpio_install_isr_service( ESP_INTR_FLAG_LEVEL3 );

        if ( ESP_OK == esp_err )
        {
            isr_service_installed = true; // Mark ISR service as installed
        }
    }

    if ( ESP_OK == esp_err )
    {
        // Hook isr handler for specific gpio pin.
        esp_err = gpio_isr_handler_add( btnPin, p_isr, ( void * )&btnPin );
    }

    return esp_err;
}


void IRAM_ATTR btn1ISR( void *p_arg )
{
    ( void )p_arg; /* Suppress the unused variable warning*/

    if( btn1PressT + debounceTime < esp_timer_get_time() )
    {
        btn1PressT = esp_timer_get_time();
        btn1PressN++;
    }
}

void IRAM_ATTR btn2ISR( void *p_arg )
{
    ( void )p_arg; /* Suppress the unused variable warning*/

    if( btn2PressT + debounceTime < esp_timer_get_time() )
    {
        btn2PressT = esp_timer_get_time();
        btn2PressN++;
    }
}

void IRAM_ATTR btn3ISR( void *p_arg )
{
    ( void )p_arg; /* Suppress the unused variable warning*/

    if( btn3PressT + debounceTime < esp_timer_get_time() )
    {
        btn3PressT = esp_timer_get_time();
        btn3PressN++;
    }
}

void IRAM_ATTR btn4ISR( void *p_arg )
{
    ( void )p_arg; /* Suppress the unused variable warning*/

    if( btn4PressT + debounceTime < esp_timer_get_time() )
    {
        btn4PressT = esp_timer_get_time();
        btn4PressN++;
    }
}

adc_oneshot_unit_handle_t adcInit( adc_channel_t adcChannel )
{
    adc_oneshot_unit_handle_t adc_handle;
    adc_oneshot_unit_init_cfg_t init_config =
    {
        .unit_id = ADC_UNIT_1
    };
    adc_oneshot_new_unit(&init_config, &adc_handle );

    adc_oneshot_chan_cfg_t channel_config =
    {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_11
    };
    adc_oneshot_config_channel( adc_handle, adcChannel, &channel_config );

    return adc_handle;
}
