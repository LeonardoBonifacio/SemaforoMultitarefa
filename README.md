
# 🚦 Projeto Semáforo Inteligente com Modo Noturno (FreeRTOS + Raspberry Pi Pico)

Este projeto simula um semáforo com dois modos de operação: **normal** e **noturno**. Utiliza o microcontrolador RP2040 da placa BitDogLab (Raspberry Pi Pico W), com display OLED SSD1306, matriz de LEDs WS2812, buzzer, botões e multitarefas via FreeRTOS.

## 🛠 Funcionalidades

- Modo **normal**: funcionamento típico de um semáforo (verde → amarelo → vermelho).
- Modo **noturno**: pisca em amarelo com sincronização precisa entre display OLED, matriz de LEDs e buzzer.
- Alteração de modo via botão.
- Uso de **FreeRTOS** para multitarefas com sincronização entre estados.

## 📦 Estrutura do Projeto

```
.
├── CMakeLists.txt
├── main.c
├── lib/
│   ├── FreeRTOS/
│   ├── FreeRTOSConfig.h
│   ├── ssd1306.h/.c
│   ├── font.h
│   ├── matrizLeds.h
├── ws2812.pio.h/.pio
```

## ⚙️ Configuração do CMake

```cmake
cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)

project(semaforo_rtos C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(semaforo
    main.c
    lib/ssd1306.c
    lib/matrizLeds.c
    ${CMAKE_CURRENT_LIST_DIR}/lib/FreeRTOS/portable/MemMang/heap_4.c
    ${CMAKE_CURRENT_LIST_DIR}/lib/FreeRTOS/tasks.c
    ${CMAKE_CURRENT_LIST_DIR}/lib/FreeRTOS/queue.c
    ${CMAKE_CURRENT_LIST_DIR}/lib/FreeRTOS/list.c
    ${CMAKE_CURRENT_LIST_DIR}/lib/FreeRTOS/timers.c
    ${CMAKE_CURRENT_LIST_DIR}/lib/FreeRTOS/event_groups.c
)

target_include_directories(semaforo PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/lib
    ${CMAKE_CURRENT_LIST_DIR}/lib/FreeRTOS/include
    ${CMAKE_CURRENT_LIST_DIR}/lib/FreeRTOS/portable/GCC/RP2040
)

target_link_libraries(semaforo pico_stdlib hardware_pwm hardware_i2c hardware_pio pico_time pico_multicore)

pico_enable_stdio_usb(semaforo 1)
pico_enable_stdio_uart(semaforo 0)

pico_add_extra_outputs(semaforo)
```

## 📸 Demonstração

(Adicione aqui imagens ou vídeos do projeto em funcionamento.)

## 📚 Requisitos

- Placa BitDogLab (RP2040 com Pico W)
- Display OLED SSD1306
- LEDs WS2812 (matriz)
- Buzzer passivo
- Botão (para trocar modo)
- FreeRTOS (incluído no repositório)

## 🧠 Conceitos Aplicados

- **RTOS (FreeRTOS)**: criação de múltiplas tasks com prioridades diferentes.
- **Sincronização entre tarefas**: controle de modo e estado compartilhado.
- **PIO do RP2040**: controle preciso da matriz WS2812.
- **PWM**: controle do buzzer.
- **I2C**: comunicação com o display OLED.

## ✅ Compilação e Upload

Clone este repositório e use o `CMake` com `ninja` ou `make`:

```bash
mkdir build
cd build
cmake ..
make
```

Use o [Picotool](https://github.com/raspberrypi/picotool) ou arraste o `.uf2` para a unidade montada do Pico.
