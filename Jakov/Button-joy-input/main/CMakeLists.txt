idf_component_register(SRCS "app_main.c" "hardware_control.c"
INCLUDE_DIRS "."
REQUIRES esp_timer driver esp_event esp_adc)