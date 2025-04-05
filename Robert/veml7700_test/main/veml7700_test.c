#include <stdio.h>
#include "../components/veml7700/veml7700-esp-idf/include/veml7700.h"
#include "driver/i2c.h"
#include "esp_log.h"

#define I2C_MASTER_NUM I2C_NUM_0
#define I2C_MASTER_SDA_IO 19
#define I2C_MASTER_SCL_IO 18
#define I2C_MASTER_FREQ_HZ 100000

static const char* TAG = "APP";

void i2c_master_init() {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };
    i2c_param_config(I2C_MASTER_NUM, &conf);
    i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);
}

void app_main() {
    i2c_master_init();

    veml7700_handle_t sensor;
    if (veml7700_initialize(&sensor, I2C_MASTER_NUM) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize VEML7700 sensor");
        return;
    }

    while (1) {
        double lux = 0;
        if (veml7700_read_als_lux_auto(sensor, &lux) == ESP_OK) {
            ESP_LOGI(TAG, "Ambient Light: %.2f lux", lux);
        } else {
            ESP_LOGW(TAG, "Failed to read light level");
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
