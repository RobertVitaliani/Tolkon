set(COMPONENT_SRCS "led_control.c")
list(APPEND COMPONENT_ADD_INCLUDEDIRS ${CMAKE_SOURCE_DIR}/include)

# Dodaj sve komponente koje koristiš u led_control.c:
# - driver (radi GPIO)
# - freertos (radi xQueue funkcija)
# - app_events ako je zasebna komponenta (nije uvijek nužno)
set(COMPONENT_REQUIRES driver freertos)

register_component()
