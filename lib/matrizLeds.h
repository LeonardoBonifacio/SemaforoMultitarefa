#include "hardware/pio.h"


// Definição do número de LEDs e pinos.
#define LED_COUNT 25
#define MATRIZ_LED_PIN 7



// Constantes dos pinos dos leds
#define LED_GREEN_PIN 11                                    
#define LED_RED_PIN 13       


extern  bool sinal_verde[LED_COUNT];
extern  bool sinal_amarelo[LED_COUNT];
extern  bool sinal_vermelho[LED_COUNT];
extern  bool sinal_modo_noturno[LED_COUNT];
extern  bool sinal_apagado[LED_COUNT];

static inline void put_pixel(uint32_t pixel_grb);
static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b);
void set_one_led(uint8_t r, uint8_t g, uint8_t b, bool desenho[]);
