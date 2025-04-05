#include "../components/Adafruit_VEML7700/Adafruit_VEML7700.h"
#include "Wire.h"  // This is the I2C library for ESP32

Adafruit_VEML7700 veml7700; // Create sensor object

void app_main() {
    Wire.begin(); // Initialize I2C bus (default pins for ESP32: SDA = 21, SCL = 22)

    // Initialize the VEML7700 sensor
    if (!veml7700.begin()) {
        printf("Failed to initialize VEML7700 sensor\n");
        return;
    }

    printf("VEML7700 sensor initialized!\n");

    // Continuously read and print lux values
    while (1) {
        // Get lux value
        float lux = veml7700.readLux(VEML_LUX_NORMAL);
        printf("Lux: %.2f\n", lux);

        // Add a delay to avoid flooding the output
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
