cmake_minimum_required(VERSION 3.17)

project(mqtt_c C)
set(CMAKE_C_STANDARD 99)

find_package(eclipse-paho-mqtt-c 1.3.14 QUIET)

if(eclipse-paho-mqtt-c_FOUND)
    set(PAHO_MQTT_LIBRARIES paho-mqtt3c)
    set(PAHO_MQTT_TLS_LIBRARIES paho-mqtt3cs)
else()
    include_directories(/usr/local/include)
    link_directories(/usr/local/lib)
    set(PAHO_MQTT_LIBRARIES paho-mqtt3c)
    set(PAHO_MQTT_TLS_LIBRARIES paho-mqtt3cs)
endif()

add_executable(mqtt_c main.c)
target_link_libraries(mqtt_c ${PAHO_MQTT_LIBRARIES})

add_executable(mqtt_tls_c main_tls.c)
target_link_libraries(mqtt_tls_c ${PAHO_MQTT_TLS_LIBRARIES})

add_executable(simple_test simple_test.c)
target_link_libraries(simple_test ${PAHO_MQTT_LIBRARIES})

add_executable(emqx_file_transfer emqx_file_transfer.c)
target_link_libraries(emqx_file_transfer ${PAHO_MQTT_LIBRARIES})
