#ifndef WIFI_CONTROL_H
#define WIFI_CONTROL_H

#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_mac.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_http_client.h"

#include "cJSON.h"

extern EventGroupHandle_t wifi_event_group;

extern const char* ntpServer1;
extern const char* ntpServer2;

extern struct tm timeinfo;

extern char* country;
extern char* city;


void wifi_event_handler( void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data );
void wifi_init_sta( void );

void setup_time( void );
void initialize_sntp( void );
bool obtain_time( struct tm *timeinfo );

esp_err_t http_event_handler( esp_http_client_event_t *evt );
void setup_location( void );

#endif