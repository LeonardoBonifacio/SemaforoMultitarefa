#include "hardware/pio.h"
#include "matrizLeds.h"


bool sinal_verde[LED_COUNT] = {
    0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 
    0, 0, 1, 0, 0
};

bool sinal_amarelo[LED_COUNT] = {
    0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 
    0, 0, 1, 0, 0, 
    0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0
};

bool sinal_vermelho[LED_COUNT] = {
    0, 0, 1, 0, 0, 
    0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0
};

bool sinal_modo_noturno[LED_COUNT] = {
    0, 0, 1, 0, 0, 
    0, 0, 0, 0, 0, 
    0, 0, 1, 0, 0, 
    0, 0, 0, 0, 0, 
    0, 0, 1, 0, 0
};

bool sinal_apagado[LED_COUNT] = {
    0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0
};



// Para mandar um valor grb de 32bits(mas so 24 sendo usados) para a maquina de estado 0 do bloco 0 do PIO
static inline void put_pixel(uint32_t pixel_grb)
{
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

// cria um valor grb de 32 bits
static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b)
{
    return ((uint32_t)(r) << 8) | ((uint32_t)(g) << 16) | (uint32_t)(b);
}

void set_one_led(uint8_t r, uint8_t g, uint8_t b, bool desenho[])
{
    // Define a cor com base nos parâmetros fornecidos
    uint32_t color = urgb_u32(r, g, b);

    // Define todos os LEDs com a cor especificada
    for (int i = 0; i < LED_COUNT; i++)
    {
        if (desenho[i])
        {
            put_pixel(color); // Liga o LED com um no buffer
        }
        else
        {
            put_pixel(0);  // Desliga os LEDs com zero no buffer
        }
    }
}
