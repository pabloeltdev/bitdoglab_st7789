cmake_minimum_required(VERSION 3.10)

project(bitdoglab_st7789 VERSION 0.0.1 DESCRIPTION "Biblioteca para o display ST7789 na placa BitDogLab")

add_library(${PROJECT_NAME} src/st7789.c)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/fonts
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        hardware_spi
    PUBLIC
        pico_stdlib
)