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



void app_main( void )
{
    btnInit( btn1PIN, btn1ISR );
    btnInit( btn3PIN, btn2ISR );
    btnInit( btn2PIN, btn3ISR );
    btnInit( btn4PIN, btn4ISR );

    adc_oneshot_unit_handle_t joyXHandle = adcInit( joyXChannel );
    adc_oneshot_unit_handle_t joyYHandle = adcInit( joyYChannel );

    for( ;; )
    {
        printf( "timer time: %lld\n", esp_timer_get_time() );

        printf( "btn1PressN: %d\n", btn1PressN );
        printf( "btn2PressN: %d\n", btn2PressN );
        printf( "btn3PressN: %d\n", btn3PressN );
        printf( "btn4PressN: %d\n", btn4PressN );

        adc_oneshot_read( joyXHandle, ADC_CHANNEL_6, &joyXValue );

        printf( "ADC Value X: %d\n", joyXValue );

        adc_oneshot_read( joyYHandle, ADC_CHANNEL_7, &joyYValue );
        printf( "ADC Value Y: %d\n", joyYValue );

        printf( "\n" );
        vTaskDelay( 1000U / portTICK_PERIOD_MS );
    }
}


