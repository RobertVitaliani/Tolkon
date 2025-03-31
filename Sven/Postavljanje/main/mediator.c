#include "mediator.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "app_events.h"

// Queue-evi se definiraju ovdje
QueueHandle_t mediator_to_led_queue;
QueueHandle_t led_switch_in;

void mediator_task(void *pv)
{
    AppMessage msg;
    while (1) {
        //  Čeka poruke iz GUI-ja
        if (xQueueReceive(led_switch_in, &msg, portMAX_DELAY)) {
            //  Prosljeđuje LED handleru
            xQueueSend(mediator_to_led_queue, &msg, portMAX_DELAY);
        }
    }
}

