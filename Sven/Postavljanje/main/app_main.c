#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "gui/gui.h"
#include "led_control/led_control.h"
#include "app_events.h"
#include "mediator.h"
#include <stdio.h>

void app_main(void)
{
    // Inicijalizacije
    led_control_init();
    gui_init();

    led_switch_in = xQueueCreate(10, sizeof(AppMessage));
    // Kreiranje queue-a koji ide od GUI preko medijatora do LED taska
    mediator_to_led_queue = xQueueCreate(10, sizeof(AppMessage));

    // Pokretanje taskova
    xTaskCreate(mediator_task, "Mediator Task", 4096, NULL, 3, NULL);
    xTaskCreate(led_handler_task, "LED Handler Task", 2048, NULL, 2, NULL);
}
