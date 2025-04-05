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
#include "esp_sntp.h"

#include "esp_mac.h"

#include "nvs_flash.h"

#include <string.h>
#include "esp_log.h"
#include "time.h"
#include "lwip/apps/sntp.h"
#include "sntp.h"
#include <esp_mac.h>

#include <stdio.h>
#include "esp_http_client.h"
#include "cJSON.h"


#define WIFI_CONNECTED_BIT BIT0
#define WIFI_SSID "BIRD-Incubator-WIFI"
#define WIFI_PASS "BIRD01012021"


// Time zone offsets (in seconds)
#define GMT_OFFSET_SEC 7200  // 2 * 3600 for UTC+1
#define DAYLIGHT_OFFSET_SEC 0  // daylight saving offset ( 3600 for 1 hour)

EventGroupHandle_t wifi_event_group;

// NTP servers
const char* ntpServer1 = "pool.ntp.org";
const char* ntpServer2 = "time.nist.gov";

struct tm timeinfo = {0};

char* country = NULL;
char* city = NULL;


void wifi_event_handler( void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data )
{
    if ( event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START )
    {
        esp_wifi_connect();
    }
    else if ( event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED )
    {
        esp_wifi_connect();
        xEventGroupClearBits( wifi_event_group, WIFI_CONNECTED_BIT );
        printf( "Retrying WiFi connection...\n" );
    }
    else if ( event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP )
    {
        ip_event_got_ip_t* event = ( ip_event_got_ip_t* ) event_data;
        printf( "Connected to the WiFi network\n" );
        printf( "Local ESP32 IP: " IPSTR "\n", IP2STR(&event->ip_info.ip ) );
        xEventGroupSetBits( wifi_event_group, WIFI_CONNECTED_BIT );
    }
}

void wifi_init_sta( void )
{
    // Initialize NVS (non-volatile storage)
    esp_err_t ret = nvs_flash_init();

    if ( ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND )
    {
        ESP_ERROR_CHECK( nvs_flash_erase() );
        ret = nvs_flash_init();
    }

    ESP_ERROR_CHECK( ret );

    // Create event group
    wifi_event_group = xEventGroupCreate();

    // Initialize TCP/IP stack
    ESP_ERROR_CHECK( esp_netif_init() );
    ESP_ERROR_CHECK( esp_event_loop_create_default() );
    esp_netif_create_default_wifi_sta();

    // Initialize WiFi
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg ) );

    // Register event handlers
    ESP_ERROR_CHECK( esp_event_handler_instance_register( WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, NULL ) );
    ESP_ERROR_CHECK( esp_event_handler_instance_register( IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, NULL ) );

    // Configure WiFi as Station mode
    wifi_config_t wifi_config =
    {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
        },
    };
    ESP_ERROR_CHECK( esp_wifi_set_mode( WIFI_MODE_STA ) );
    ESP_ERROR_CHECK( esp_wifi_set_config( ESP_IF_WIFI_STA, &wifi_config ) );
    ESP_ERROR_CHECK( esp_wifi_start() );

    printf( "Connecting\n" );

    // Wait for connection
    while ( !( xEventGroupWaitBits( wifi_event_group, WIFI_CONNECTED_BIT, pdFALSE, pdTRUE, portTICK_PERIOD_MS * 100 ) & WIFI_CONNECTED_BIT ) )
    {
        printf( "." );
        vTaskDelay( 100 / portTICK_PERIOD_MS ); // 100ms delay
    }
}


bool obtain_time( struct tm *timeinfo )
{
    // Wait for time to be set
    time_t now = 0;
    int retry = 0;
    const int retry_count = 10;

    while ( sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && retry < retry_count )
    {
        printf( "Waiting for system time to be set... (%d/%d)\n", retry, retry_count );
        vTaskDelay( 2000 / portTICK_PERIOD_MS ); // Wait 2 seconds
        retry++;
    }

    time(&now );
    localtime_r(&now, timeinfo );

    // Check if time was actually set (non-zero year is a simple validation)
    return ( timeinfo->tm_year > 70 ); // Years are offset from 1900, so > 70 means post-1970
}


void initialize_sntp( void )
{
    // Initialize SNTP
    sntp_setoperatingmode( SNTP_OPMODE_POLL );
    sntp_setservername( 0, ( char* )ntpServer1 );
    sntp_setservername( 1, ( char* )ntpServer2 );
    sntp_init();

    // Set timezone
    setenv( "TZ", "UTC-2", 1 ); // Adjust this string based on GMT_OFFSET_SEC and DAYLIGHT_OFFSET_SEC
    tzset();
}


void setup_time( void )
{
    // Configure time with NTP servers
    initialize_sntp();

    // Wait for time to be set
    if ( !obtain_time(&timeinfo ) )
    {
        printf( "Failed to obtain time\n" );
        return;
    }

}


esp_err_t http_event_handler( esp_http_client_event_t *evt )
{
    static char *response_buffer = NULL;
    static size_t response_len = 0;

    switch( evt->event_id )
    {
        case HTTP_EVENT_ON_DATA:
            // Reallocate buffer to accommodate new data
            response_buffer = realloc( response_buffer, response_len + evt->data_len + 1 );
            memcpy( response_buffer + response_len, evt->data, evt->data_len );
            response_len += evt->data_len;
            response_buffer[ response_len ] = '\0'; // Null terminate
            break;

        case HTTP_EVENT_ON_FINISH:
            if ( response_buffer )
            {
                // Parse JSON
                cJSON *json = cJSON_Parse( response_buffer );

                if ( json != NULL )
                {
                    cJSON* countryJSON = cJSON_GetObjectItem( json, "country" );
                    cJSON* cityJSON = cJSON_GetObjectItem( json, "city" );

                    country = strdup( countryJSON->valuestring );
                    city = strdup( cityJSON->valuestring );

                    if ( !( countryJSON && cityJSON && cJSON_IsString( countryJSON ) && cJSON_IsString( cityJSON ) ) )
                    {
                        printf( "Failed to extract JSON fields\n" );
                    }

                    cJSON_Delete( json );
                }
                else
                {
                    printf( "Failed to parse JSON\n" );
                }

                // Clean up
                free( response_buffer );
                response_buffer = NULL;
                response_len = 0;
            }

            break;

        case HTTP_EVENT_ERROR:
            printf( "HTTP client error\n" );
            break;

        case HTTP_EVENT_DISCONNECTED:
            if ( response_buffer )
            {
                free( response_buffer );
                response_buffer = NULL;
                response_len = 0;
            }

            break;

        default:
            break;
    }

    return ESP_OK;
}

void setup_location( void )
{
    // HTTP client configuration
    esp_http_client_config_t config =
    {
        .url = "http://ip-api.com/json",
        .event_handler = http_event_handler,
        .method = HTTP_METHOD_GET,
    };

    // Initialize HTTP client
    esp_http_client_handle_t client = esp_http_client_init(&config );

    if ( client == NULL )
    {
        printf( "Failed to initialize HTTP client\n" );
        return;
    }

    // Perform HTTP GET request
    esp_err_t err = esp_http_client_perform( client );

    if ( err == ESP_OK )
    {
        int status_code = esp_http_client_get_status_code( client );

        if ( status_code != 200 )
        {
            printf( "HTTP GET failed. Code: %d\n", status_code );
        }
    }
    else
    {
        printf( "HTTP request failed: %s\n", esp_err_to_name( err ) );
    }

    // Cleanup
    esp_http_client_cleanup( client );
}