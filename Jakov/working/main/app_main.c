/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <string.h>
#include <time.h>

#include "../components/veml7700-esp-idf/include/veml7700.h"
#include "i2c_control.h"

#include "wifi_controller.h"
#include "i2c_control.h"

char strftime_buf[ 64 ];

veml7700_handle_t sensor;

void app_main( void )
{
    wifi_init_sta();

    setup_time();

    // get the time
    strftime( strftime_buf, sizeof( strftime_buf ), "%a %d %B %H:%M", &timeinfo );

    // print the time
    printf( "Current time: %s\n", strftime_buf );

    setup_location();

    printf( "Country: %s\n", country );
    printf( "City: %s\n", city );


    i2c_master_init();


    if ( veml7700_initialize(&sensor, i2cMasterNum ) != ESP_OK )
    {
        printf( "Failed to initialize VEML7700 sensor" );
        return;
    }

    for( ;; )
    {
        double lux = 0;

        if ( veml7700_read_als_lux_auto( sensor, &lux ) == ESP_OK )
        {
            printf( "Ambient Light: %.2f lux", lux );
        }
        else
        {
            printf( "Failed to read light level" );
        }

        vTaskDelay( 1000U / portTICK_PERIOD_MS );
    }
}
