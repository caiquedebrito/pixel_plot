# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(PICO_BOARD pico_w CACHE STRING "Board type")

include(pico_sdk_import.cmake)

project(pixel_plot C CXX ASM)

pico_sdk_init()

add_executable(pixel_plot pixel_plot.c lib/display/ssd1306.c lib/joystick/joystick.c)

pico_generate_pio_header(pixel_plot ${CMAKE_CURRENT_LIST_DIR}/ws2812.pio)

pico_set_program_name(pixel_plot "pixel_plot")
pico_set_program_version(pixel_plot "0.1")

pico_enable_stdio_uart(pixel_plot 0)
pico_enable_stdio_usb(pixel_plot 1)

target_link_libraries(pixel_plot
        pico_stdlib
        hardware_adc
        hardware_i2c
        hardware_pio
        hardware_pwm
        hardware_clocks
        )

target_include_directories(pixel_plot PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(pixel_plot 
        
        )

pico_add_extra_outputs(pixel_plot)