cmake_minimum_required(VERSION 3.13)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(PICO_BOARD pico_w CACHE STRING "Board type")
include(pico_sdk_import.cmake)


#Trecho para configuração do FreeRTOS

# Permitir que o usuário defina via variável de ambiente ou argumento -D
if(DEFINED ENV{FREERTOS_KERNEL_PATH})
    set(FREERTOS_KERNEL_PATH $ENV{FREERTOS_KERNEL_PATH})
endif()

# Ou definir manualmente, se não definido
#set(FREERTOS_KERNEL_PATH "${FREERTOS_KERNEL_PATH}" CACHE PATH "caminho para o kernel do freRtos")

# Verificar se o caminho é válido
if(NOT EXISTS "${FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2040/FreeRTOS_Kernel_import.cmake")
    message(FATAL_ERROR "FREERTOS_KERNEL_PATH inválido ou não definido.")
endif()

# Importar o FreeRTOS
include(${FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2040/FreeRTOS_Kernel_import.cmake)

project(SemaforoMultitarefa C CXX ASM)
pico_sdk_init()


include_directories(${CMAKE_SOURCE_DIR}/lib)


add_executable(${PROJECT_NAME}  
        SemaforoMultitarefa.c 
        lib/ssd1306.c # Biblioteca para o display OLED
        lib/matrizLeds.c # Biblioteca para leds em geral
        )

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR})



target_link_libraries(${PROJECT_NAME} 
    pico_stdlib 
    hardware_gpio
    hardware_i2c
    hardware_pio
    hardware_pwm
    FreeRTOS-Kernel 
    FreeRTOS-Kernel-Heap4
)

pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)

# Generate PIO header
pico_generate_pio_header(SemaforoMultitarefa ${CMAKE_CURRENT_LIST_DIR}/ws2812.pio)

pico_add_extra_outputs(${PROJECT_NAME})



