/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <string.h>
#include <time.h>

#include "wifi_controller.h"

char strftime_buf[ 64 ];

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

    for( ;; )
    {
        vTaskDelay( 1000U );
    }
}
