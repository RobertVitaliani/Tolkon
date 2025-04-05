#include <stdio.h>
#include "../components/veml7700/veml7700-esp-idf/include/veml7700.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "i2c_control.h"

#define I2C_MASTER_NUM I2C_NUM_0
#define I2C_MASTER_SDA_IO 19
#define I2C_MASTER_SCL_IO 18
#define I2C_MASTER_FREQ_HZ 100000

void app_main()
{
    i2c_master_init();

    veml7700_handle_t sensor;

    if ( veml7700_initialize(&sensor, I2C_MASTER_NUM ) != ESP_OK )
    {
        printf( "Failed to initialize VEML7700 sensor" );
        return;
    }

    while ( 1 )
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

        vTaskDelay( 1000 / portTICK_PERIOD_MS );
    }
}
