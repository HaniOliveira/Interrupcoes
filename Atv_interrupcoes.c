#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/adc.h"
#include "pico/bootrom.h"

// Inclusão do arquivo gerado pelo PIO
#include "ATV_interrupcoes.pio.h"

// Definições para a matriz de LEDs WS2812
#define NUM_PIXELS 25 // Matriz 5x5
#define OUT_PIN 7     // Pino de saída para os dados da matriz

// Definição dos pinos dos botões de interrupção
const uint button_0 = 5; // Botão para incrementar
const uint button_1 = 6; // Botão para decrementar

// Definições para o LED RGB
#define LED_RED 13   // Pino do LED vermelho
#define LED_GREEN 11 // Pino do LED verde
#define LED_BLUE 12  // Pino do LED azul

// Variáveis globais para controle de interrupção e debouncing
volatile int numero_atual = 0;                  // Número atual sendo exibido (0-9)
volatile uint32_t ultimo_tempo_interrupcao = 0; // Timestamp da última interrupção
#define DEBOUNCE_DELAY 200                      // Tempo de debounce em milissegundos

// Arrays para representar números de 0-9 na matriz de LEDs
// Cada array representa um número em formato 5x5
// 1 = LED aceso, 0 = LED apagado
double numero0[25] = {
    0, 1, 1, 1, 0,
    1, 0, 0, 0, 1,
    1, 0, 0, 0, 1,
    1, 0, 0, 0, 1,
    0, 1, 1, 1, 0};

double numero1[25] = {
    0, 0, 1, 0, 0,
    0, 1, 1, 0, 0,
    0, 0, 1, 0, 0,
    0, 0, 1, 0, 0,
    0, 1, 1, 1, 0};

double numero2[25] = {
    0, 1, 1, 1, 0,
    1, 0, 0, 0, 1,
    0, 0, 1, 1, 0,
    0, 0, 0, 1, 0,
    1, 1, 1, 1, 1};

double numero3[25] = {
    0, 1, 1, 1, 0,
    1, 0, 0, 0, 1,
    0, 0, 1, 1, 0,
    1, 0, 0, 0, 1,
    0, 1, 1, 1, 0};

double numero4[25] = {
    0, 0, 0, 1, 0,
    0, 0, 1, 1, 0,
    0, 1, 0, 1, 0,
    1, 1, 1, 1, 1,
    0, 0, 0, 1, 0};

double numero5[25] = {
    1, 1, 1, 1, 1,
    1, 0, 0, 0, 0,
    1, 1, 1, 1, 0,
    0, 0, 0, 0, 1,
    1, 1, 1, 1, 0};

double numero6[25] = {
    0, 1, 1, 1, 0,
    1, 0, 0, 0, 0,
    1, 1, 1, 1, 0,
    1, 0, 0, 0, 1,
    0, 1, 1, 1, 0};

double numero7[25] = {
    1, 1, 1, 1, 1,
    0, 0, 0, 1, 0,
    0, 0, 1, 0, 0,
    0, 1, 0, 0, 0,
    1, 0, 0, 0, 0};

double numero8[25] = {
    0, 1, 1, 1, 0,
    1, 0, 0, 0, 1,
    0, 1, 1, 1, 0,
    1, 0, 0, 0, 1,
    0, 1, 1, 1, 0};

double numero9[25] = {
    0, 1, 1, 1, 0,
    1, 0, 0, 0, 1,
    0, 1, 1, 1, 1,
    0, 0, 0, 0, 1,
    0, 1, 1, 1, 0};

// Função para imprimir valor em formato binário (debug)
void imprimir_binario(int num)
{
    int i;
    for (i = 31; i >= 0; i--)
    {
        (num & (1 << i)) ? printf("1") : printf("0");
    }
}

// Função para piscar o LED vermelho a 5Hz
void piscar_led_vermelho()
{
    static uint32_t ultimo_toggle = 0;
    uint32_t tempo_atual = to_ms_since_boot(get_absolute_time());

    if (tempo_atual - ultimo_toggle >= 100)
    {
        gpio_xor_mask(1u << LED_RED);
        ultimo_toggle = tempo_atual;
    }
}

// Rotina de tratamento de interrupção para os botões
static void gpio_irq_handler(uint gpio, uint32_t events)
{
    uint32_t tempo_atual = to_ms_since_boot(get_absolute_time());

    if (tempo_atual - ultimo_tempo_interrupcao < DEBOUNCE_DELAY)
    {
        return;
    }

    if (gpio == button_0)
    {
        numero_atual = (numero_atual + 1) % 10;
    }
    else if (gpio == button_1)
    {
        numero_atual = (numero_atual - 1 + 10) % 10;
    }

    ultimo_tempo_interrupcao = tempo_atual;
}

// Função para definir a cor dos LEDs RGB
uint32_t matrix_rgb(double b, double r, double g)
{
    unsigned char R, G, B;
    R = r * 255;
    G = g * 255;
    B = b * 255;
    return (G << 24) | (R << 16) | (B << 8);
}

// Função para desenhar na matriz de LEDs
void desenho_pio(double *desenho, uint32_t valor_led, PIO pio, uint sm, double r, double g, double b)
{
    for (int16_t i = 0; i < NUM_PIXELS; i++)
    {
        valor_led = matrix_rgb(b = 0.0, desenho[24 - i], g = 0.0);
        pio_sm_put_blocking(pio, sm, valor_led);
    }

    imprimir_binario(valor_led);
}

// Função principal
int main()
{
    PIO pio = pio0;
    bool ok;
    uint16_t i;
    uint32_t valor_led;
    double r = 0.0, b = 0.0, g = 0.0;

    ok = set_sys_clock_khz(128000, false);

    stdio_init_all();

    printf("iniciando a transmissão PIO");
    if (ok)
        printf("clock set to %ld\n", clock_get_hz(clk_sys));

    uint offset = pio_add_program(pio, &Atv_interrupcoes_program);
    uint sm = pio_claim_unused_sm(pio, true);
    Atv_interrupcoes_program_init(pio, sm, offset, OUT_PIN);

    gpio_init(button_0);
    gpio_set_dir(button_0, GPIO_IN);
    gpio_pull_up(button_0);

    gpio_init(button_1);
    gpio_set_dir(button_1, GPIO_IN);
    gpio_pull_up(button_1);

    gpio_init(LED_RED);
    gpio_init(LED_GREEN);
    gpio_init(LED_BLUE);
    gpio_set_dir(LED_RED, GPIO_OUT);
    gpio_set_dir(LED_GREEN, GPIO_OUT);
    gpio_set_dir(LED_BLUE, GPIO_OUT);

    gpio_set_irq_enabled_with_callback(button_0, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled(button_1, GPIO_IRQ_EDGE_FALL, true);

    while (true)
    {
        piscar_led_vermelho();

        double *numero_atual_array;
        switch (numero_atual)
        {
        case 0:
            numero_atual_array = numero0;
            break;
        case 1:
            numero_atual_array = numero1;
            break;
        case 2:
            numero_atual_array = numero2;
            break;
        case 3:
            numero_atual_array = numero3;
            break;
        case 4:
            numero_atual_array = numero4;
            break;
        case 5:
            numero_atual_array = numero5;
            break;
        case 6:
            numero_atual_array = numero6;
            break;
        case 7:
            numero_atual_array = numero7;
            break;
        case 8:
            numero_atual_array = numero8;
            break;
        case 9:
            numero_atual_array = numero9;
            break;
        default:
            numero_atual_array = numero0;
            break;
        }

        desenho_pio(numero_atual_array, valor_led, pio, sm, r, g, b);
        sleep_ms(10);
    }
}
