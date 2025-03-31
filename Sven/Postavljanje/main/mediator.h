#ifndef MEDIATOR_H
#define MEDIATOR_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "app_events.h"

extern QueueHandle_t mediator_to_led_queue;

/**
 * @brief Task koji prosljeđuje poruke iz GUI-ja prema LED handleru.
 */
void mediator_task(void *pv);

#endif
