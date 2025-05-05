#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "lib/FreeRTOSConfig.h"
#include "task.h"
#include "hardware/pwm.h"
#include "hardware/i2c.h"
#include "lib/ssd1306.h"
#include "lib/font.h"
#include "lib/matrizLeds.h"
#include "ws2812.pio.h"  

 

// Variáveis para conexão i2c e do display oled
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C
ssd1306_t ssd; // Inicializa a estrutura do display

#define GPIO_BOTAO_ALTERNA_MODO 5
volatile bool modo = true; // False = modo normal      True = modo noturno
volatile bool modo_trocado = false; // Para verificar se ouve alteraçao nos modos de operação e para sincronização entre tasks 


// Para definir o wrap global para uso no buzzer
volatile uint32_t wrap_global = 0;

// Enumeration para sincronização dos estados do sinal
typedef enum {
    SINAL_VERDE,
    SINAL_AMARELO,
    SINAL_VERMELHO,
    SINAL_NENHUM
} EstadoSinal;

// Enumeration para sincronização dos modos de operação do Sinal
typedef enum{
    NORMAL,
    NOTURNO
} ModoSinal;

// Variáveis criadas das Enumerations
volatile EstadoSinal estado_sinal = SINAL_NENHUM;
volatile ModoSinal modo_sinal = NORMAL;



// Task para controle dos leds e do buzzer que imitam o semáforo de trânsito
void vControlaSinalTask(void *pvParameters) {

    while (true) {
        // Se o modo foi trocado pela task do botão, então defina qual vai ser o novo modo e reset a variável de troca de modo
        if (modo_trocado) {
            modo_sinal = modo ? NOTURNO : NORMAL;
            estado_sinal = SINAL_NENHUM; 
            modo_trocado = false;
        }
        // Alterna entre os modos de operação(NORMAL e NOTURNO)
        switch (modo_sinal){
            // No modo normal usei apenas o vtaskDelay e o valores da Enumeration para sincronização
            case NORMAL:
                gpio_put(LED_GREEN_PIN,0);
                gpio_put(LED_RED_PIN,0);
                estado_sinal = SINAL_VERDE;
                pwm_set_gpio_level(21, wrap_global / 2);
                set_one_led(0,255,0,sinal_verde); 
                gpio_put(LED_GREEN_PIN,1);
                vTaskDelay(pdMS_TO_TICKS(1500)); 
                pwm_set_gpio_level(21, 0); 
                gpio_put(LED_GREEN_PIN,0);
                
                estado_sinal = SINAL_AMARELO;
                set_one_led(255,255,0,sinal_amarelo);
                for (size_t i = 0; i < 5; i++) {
                    pwm_set_gpio_level(21, wrap_global / 2);
                    gpio_put(LED_RED_PIN,1);
                    gpio_put(LED_GREEN_PIN,1);
                    vTaskDelay(pdMS_TO_TICKS(100));
                    pwm_set_gpio_level(21, 0);
                    gpio_put(LED_GREEN_PIN,0);
                    gpio_put(LED_RED_PIN,0);
                    vTaskDelay(pdMS_TO_TICKS(100));
                }

                estado_sinal = SINAL_VERMELHO;
                set_one_led(255,0,0, sinal_vermelho);
                pwm_set_gpio_level(21, wrap_global / 2);
                gpio_put(LED_RED_PIN,1);
                vTaskDelay(pdMS_TO_TICKS(500));
                pwm_set_gpio_level(21, 0);
                gpio_put(LED_RED_PIN,0);
                estado_sinal =  SINAL_NENHUM;
                set_one_led(0,0,0, sinal_apagado);
                vTaskDelay(pdMS_TO_TICKS(1500));
                break;
            // No modo noturno foi necessário usar a contagem de ticks , pois ao trocar de modo e com o passar do tempo o vtask delay 
            // dessincronizava o piscar dos leds na matriz com o piscar dos "semaforos" no display oled
            case NOTURNO:
                TickType_t tick = xTaskGetTickCount();
                // Em segundos pares ele acende os leds e liga o buzzer
                if ((tick / 1000) % 2 == 0) {
                    pwm_set_gpio_level(21, wrap_global / 2);
                    set_one_led(255,255,0,sinal_modo_noturno); 
                    gpio_put(LED_RED_PIN,1);
                    gpio_put(LED_GREEN_PIN,1);
                } else { // Em segundos impares ele apaga os leds e desliga o buzzer
                    pwm_set_gpio_level(21, 0);
                    set_one_led(0,0,0,sinal_apagado);
                    gpio_put(LED_GREEN_PIN,0);
                    gpio_put(LED_RED_PIN,0);
                }
                vTaskDelay(pdMS_TO_TICKS(100)); // Pequeno delay para acompanhar as transições
                break;
        }
    }
}


// Task para configurar o pwm com prioridade mais alta para ser executada primeiro 
void vPWMConfigTask(void *pvParameters) {
    gpio_set_function(21, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(21);
    uint32_t clock_hz = clock_get_hz(clk_sys);
    uint32_t wrap = 65000;

    pwm_set_wrap(slice_num, wrap - 1);
    pwm_set_clkdiv(slice_num, 4.0);
    pwm_set_enabled(slice_num, true);

    wrap_global = wrap;       // Define o valor global
    vTaskDelete(NULL);        // Finaliza a task de configuração, pois so precisa ser executada uma vez
}


// Task que controla o display
void vDisplayTask(void *pvParameters){

    bool cor = true;
    ssd1306_rect(&ssd,0,30,20,64,cor,!cor); // Desenha um retângulo para representar um semáforo
    while (true){
        switch (modo_sinal){
            // Troca entre os modos normal e noturno
            // Aqui no modo normal usei a sincronização através dos valores da enumeration
            case NORMAL:
                // Desenha a cor do sinal ja que o ssd nao possui cores
                ssd1306_draw_string(&ssd, "VERDE  ",60,10);
                ssd1306_draw_string(&ssd, "AMARELO",60,30);
                ssd1306_draw_string(&ssd, "VERMELHO",60,50);
                // Apaga inicialmente os quadrados dentro do retângulo que representam cada cor do display
                ssd1306_rect(&ssd,21,31,18,21,!cor,cor);     // Amarelo desligado
                ssd1306_rect(&ssd,43,31,18,19,!cor,cor);     // Vermelho desligado
                ssd1306_rect(&ssd,2,31,18,18,!cor,cor);      // Verde desligado
                // Troca entre os estados do sinal
                switch (estado_sinal){
                    case SINAL_VERDE:
                        ssd1306_rect(&ssd,2,31,18,18,cor,cor);      // "Verde" ligado
                        ssd1306_send_data(&ssd);
                        break;
                    case SINAL_AMARELO:
                        ssd1306_rect(&ssd,21,31,18,21,cor,cor);     // "Amarelo" ligado
                        ssd1306_send_data(&ssd);
                        break;
                    
                    case SINAL_VERMELHO:
                        ssd1306_rect(&ssd,43,31,18,19,cor,cor);     // "Vermelho" ligado
                        ssd1306_send_data(&ssd);
                        break;

                    case SINAL_NENHUM:
                        ssd1306_rect(&ssd,2,31,18,18,!cor,cor);      // "Verde" desligado
                        ssd1306_rect(&ssd,21,31,18,21,!cor,cor);     // "Amarelo" desligado
                        ssd1306_rect(&ssd,43,31,18,19,!cor,cor);     // "Vermelho" desligado
                        ssd1306_send_data(&ssd);
                        break;
                }

                break;
                // Usei o contador de ticks no modo noturno das duas tasks
                case NOTURNO:
                    // Desenha o nome amarelo nas 3 cores do sinal pra representar o modo noturno
                    ssd1306_draw_string(&ssd, "AMARELO  ",60,10);
                    ssd1306_draw_string(&ssd, "AMARELO",60,30);
                    ssd1306_draw_string(&ssd, "AMARELO ",60,50);
                    // Em segundos pares liga e em impares desliga
                    TickType_t tick = xTaskGetTickCount();
                    if ((tick / 1000) % 2 == 0) {
                        ssd1306_rect(&ssd,21,31,18,21,cor,cor);   
                        ssd1306_rect(&ssd,2,31,18,18, cor,cor);     
                        ssd1306_rect(&ssd,43,31,18,19,cor,cor);     
                    } else {
                        ssd1306_rect(&ssd,21,31,18,21,!cor,cor);   
                        ssd1306_rect(&ssd,2,31,18,18, !cor,cor);     
                        ssd1306_rect(&ssd,43,31,18,19,!cor,cor); 
                    }
                    ssd1306_send_data(&ssd);
                    vTaskDelay(pdMS_TO_TICKS(100)); // Delay curto para acompanhar as transições
                    break;
            }     
    }         
}
        

// Task que controla o botão que alterna entre os modos e os sincroniza
void vBotaoTask(void *pvParameters) {
    bool botaoAnterior = false;
    while (1) {
        bool estadoBotao = gpio_get(GPIO_BOTAO_ALTERNA_MODO); 

        if (estadoBotao && !botaoAnterior) {
            modo = !modo; // variável de controle simples, pode manter
            modo_trocado = true; // apenas sinaliza a mudança
        }
        botaoAnterior = estadoBotao;
        vTaskDelay(pdMS_TO_TICKS(100)); 
    }
}

void vInicializaGpio(void * pvParameters){
    gpio_init(LED_GREEN_PIN);
    gpio_init(LED_RED_PIN);
    gpio_set_dir(LED_GREEN_PIN, GPIO_OUT);
    gpio_set_dir(LED_RED_PIN, GPIO_OUT);
    gpio_init(GPIO_BOTAO_ALTERNA_MODO);
    gpio_set_dir(GPIO_BOTAO_ALTERNA_MODO, GPIO_IN);
    gpio_pull_up(GPIO_BOTAO_ALTERNA_MODO);
    vTaskDelete(NULL);        // Finaliza a task de inicialização
}

void vInicializaProgramaPio(void * pvParameters){
    PIO pio = pio0;// Seleciona o bloco pio que será usado
    int sm = 0; // Define qual state machine será usada
    uint offset = pio_add_program(pio, &ws2812_program);// Carrega o programa PIO para controlar os WS2812 na memória do PIO.
    ws2812_program_init(pio, sm, offset, MATRIZ_LED_PIN, 800000, false); //Inicializa a State Machine para executar o programa PIO carregado.
    vTaskDelete(NULL);        // Finaliza a task de inicialização
}

void vInicializaI2C_Display(void * pvParameters){
     // I2C Initialisation. Using it at 400Khz.
     i2c_init(I2C_PORT, 400 * 1000);
     gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);                    // Set the GPIO pin function to I2C
     gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);                    // Set the GPIO pin function to I2C
     gpio_pull_up(I2C_SDA);                                        // Pull up the data line
     gpio_pull_up(I2C_SCL);                                        // Pull up the clock line
     ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); // Inicializa o display
     ssd1306_config(&ssd);                                         // Configura o display
     ssd1306_fill(&ssd, false); // Limpa o display. O display inicia com todos os pixels apagados.
     ssd1306_send_data(&ssd); // Envia os dados
     vTaskDelete(NULL);        // Finaliza a task de inicialização
}


int main(){
    xTaskCreate(vInicializaI2C_Display, "Inicializa conexão i2c e display", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, NULL);
    xTaskCreate(vInicializaProgramaPio, "Inicializa programa Pio", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, NULL);
    xTaskCreate(vInicializaGpio, "Inicializa gpios", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, NULL);
    xTaskCreate(vPWMConfigTask, "PWM Config", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, NULL);
    xTaskCreate(vDisplayTask, "Controla Display", 1024, NULL, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(vControlaSinalTask, "Controla sinal", 512, NULL, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(vBotaoTask, "Botao", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
   

    
   vTaskStartScheduler();
   panic_unsupported();
}
