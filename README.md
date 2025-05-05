
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

## ⚙️ Configuração do CMake importantes para FreeRtos

```cmake
#Trecho para configuração do FreeRTOS

# Permitir que o usuário defina via variável de ambiente ou argumento -D
if(DEFINED ENV{FREERTOS_KERNEL_PATH})
    set(FREERTOS_KERNEL_PATH $ENV{FREERTOS_KERNEL_PATH})
endif()

# Ou definir manualmente, se não definido
#set(FREERTOS_KERNEL_PATH "${FREERTOS_KERNEL_PATH}" CACHE PATH "caminho para o kernel do freeRtos")

# Verificar se o caminho é válido
if(NOT EXISTS "${FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2040/FreeRTOS_Kernel_import.cmake")
    message(FATAL_ERROR "FREERTOS_KERNEL_PATH inválido ou não definido.")
endif()

# Importar o FreeRTOS
include(${FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2040/FreeRTOS_Kernel_import.cmake)

target_link_libraries(${PROJECT_NAME} 
    pico_stdlib 
    hardware_gpio
    hardware_i2c
    hardware_pio
    hardware_pwm
    FreeRTOS-Kernel 
    FreeRTOS-Kernel-Heap4
)
```

## 📦 Configurando o caminho para o FreeRTOS

O projeto depende da variável `FREERTOS_KERNEL_PATH` apontando para o diretório onde o **FreeRTOS-Kernel** está salvo.

Você pode configurar isso de duas formas:

### Configurar variável de ambiente (recomendado)

#### **No Windows:**

1. Pressione `Win + S` e procure por “variáveis de ambiente”.
2. Clique em **“Variáveis de ambiente...”**.
3. Em **"Variáveis de usuário"**, clique em **"Nova..."**.
4. Adicione:
   - Nome: `FREERTOS_KERNEL_PATH`
   - Valor: `C:\FreeRTOS-Kernel` *(ajuste conforme o local do seu kernel)*


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

