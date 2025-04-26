#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "pico/stdlib.h"
#include "pico/bootrom.h"

#include "hardware/adc.h"
#include "hardware/i2c.h"

#include "lib/ssd1306.h"
#include "lib/font.h"
#include "lib/button.h"
#include "lib/ws2812b.h"

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define DISPLAY_ADDRESS 0x3C
#define ADC_PIN 28 // GPIO para o voltímetro
#define LED_MATRIX_PIN 7
#define LED_MATRIX_SIZE 5

void gpio_irq_handler(uint gpio, uint32_t events);
void init_display(ssd1306_t *ssd);
float get_adc_average(uint adc_input, int samples);
float get_normalized_resistance(float value, char *color);
void get_band_color(int band_number, char *color);
float find_nearest_e24(float value);

const int ADC_RESOLUTION = 4095;  // Resolução do ADC (12 bits)
const float KNOWN_R_VALUE = 9910; // Resistor de 10k ohm - 9910
const float ADC_VREF = 3.33;      // Tensão de referência do ADC
const float E24_VALUES[] = {1.0, 1.1, 1.2, 1.3, 1.5, 1.6, 1.8, 2.0, 2.2, 2.4, 2.7, 3.0,
                            3.3, 3.6, 3.9, 4.3, 4.7, 5.1, 5.6, 6.2, 6.8, 7.5, 8.2, 9.1};
const int E24_SIZE = 24;

float r_x = 0.0;     // Resistor desconhecido - 984
float r_x_e24 = 0.0; // Resistor desconhecido - 984

int main()
{
    stdio_init_all(); // Inicializa a comunicação serial

    char adc_reading_str[5];       // Buffer para armazenar a string
    char resistance_value_str[5];  // Buffer para armazenar a string
    char ohm_multiplier_color[10]; // Buffer para armazenar a string
    char band_color[10];           // Buffer para armazenar a string
    char band_color_2[10];         // Buffer para armazenar a string
    ssd1306_t ssd;                 // Envia os dados para o display
    bool display_color = true;
    float adc_average = 0.0f;
    float normalized_resistance = 0;
    int resistance_rounded = 0;
    int band_number = 0;
    int band_number_2 = 0;

    init_btns(); // Inicializa os botões
    adc_init();
    init_display(&ssd);           // Inicializa o display
    adc_gpio_init(ADC_PIN);       // GPIO 28 como entrada analógica
    ws2812b_init(LED_MATRIX_PIN); // Inicializa a matriz de LEDs

    ws2812b_clear();
    ws2812b_write(); // Atualiza a matriz de LEDs

    // Define a interrupt handler para o botão B
    gpio_set_irq_enabled_with_callback(BUTTON_B_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    while (true)
    {
        adc_average = get_adc_average(2, 500); // Lê o valor médio do ADC

        // Fórmula simplificada: R_x = R_conhecido * ADC_encontrado /(ADC_RESOLUTION - adc_encontrado)
        r_x = (KNOWN_R_VALUE * adc_average) / (ADC_RESOLUTION - adc_average);
        printf("Valor do resistor: %.2f\n", r_x); // Debug

        // Converte o valor do resistor para a série E24
        r_x_e24 = find_nearest_e24(r_x);                    // Encontra o valor mais próximo na série E24
        printf("Valor do resistor E24: %.2f\n\n", r_x_e24); // Debug

        // Converte o valor do resistor em string
        sprintf(adc_reading_str, "%1.0f", adc_average);                                   // Converte o inteiro em string
        sprintf(resistance_value_str, "%1.0f", r_x_e24);                                  // Converte o float em string
        normalized_resistance = get_normalized_resistance(r_x_e24, ohm_multiplier_color); // Converte o valor do resistor em string
        resistance_rounded = (int)round(normalized_resistance);                           // Arredonda para o inteiro mais próximo

        // Calcula o valor da banda 1 e 2
        band_number = resistance_rounded / 10;   // Valor da banda 1
        band_number_2 = resistance_rounded % 10; // Valor da banda 2

        // Converte o número da banda em string
        get_band_color(band_number, band_color);     // Converte o número da banda em string
        get_band_color(band_number_2, band_color_2); // Converte o número da banda em string

        //  Atualiza o conteúdo do display com animações
        ssd1306_fill(&ssd, !display_color);                               // Limpa o display
        ssd1306_rect(&ssd, 3, 3, 122, 60, display_color, !display_color); // Desenha um retângulo

        ssd1306_draw_string(&ssd, "Cor 1", 8, 6);
        ssd1306_draw_string(&ssd, band_color, 54, 6);
        ssd1306_line(&ssd, 3, 15, 123, 15, display_color); // Desenha uma linha

        ssd1306_draw_string(&ssd, "Cor 2", 8, 18); // Desenha uma string
        ssd1306_draw_string(&ssd, band_color_2, 54, 18);
        ssd1306_line(&ssd, 3, 27, 123, 27, display_color); // Desenha uma linha

        ssd1306_draw_string(&ssd, "Cor 3", 8, 29); // Desenha uma string
        ssd1306_draw_string(&ssd, ohm_multiplier_color, 54, 29);
        ssd1306_line(&ssd, 3, 37, 123, 37, display_color); // Desenha uma linha

        ssd1306_draw_string(&ssd, "ADC", 13, 41);          // Desenha uma string
        ssd1306_draw_string(&ssd, "Resisten.", 50, 41);    // Desenha uma string
        ssd1306_line(&ssd, 44, 37, 44, 60, display_color); // Desenha uma linha vertical

        ssd1306_draw_string(&ssd, adc_reading_str, 8, 52);       // Desenha uma string
        ssd1306_draw_string(&ssd, resistance_value_str, 59, 52); // Desenha uma string
        ssd1306_send_data(&ssd);                                 // Atualiza o display
        sleep_ms(700);
    }
}

void gpio_irq_handler(uint gpio, uint32_t events)
{
    reset_usb_boot(0, 0);
}

void init_display(ssd1306_t *ssd)
{
    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400 * 1000);

    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);                          // Set the GPIO pin function to I2C
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);                          // Set the GPIO pin function to I2C
    gpio_pull_up(I2C_SDA);                                              // Pull up the data line
    gpio_pull_up(I2C_SCL);                                              // Pull up the clock line
                                                                        // Inicializa a estrutura do display
    ssd1306_init(ssd, WIDTH, HEIGHT, false, DISPLAY_ADDRESS, I2C_PORT); // Inicializa o display
    ssd1306_config(ssd);                                                // Configura o display
    ssd1306_send_data(ssd);

    ssd1306_init(ssd, WIDTH, HEIGHT, false, DISPLAY_ADDRESS, I2C_PORT); // Inicializa o display
    ssd1306_config(ssd);                                                // Configura o display
    ssd1306_send_data(ssd);                                             // Envia os dados para o display
    ssd1306_fill(ssd, false);                                           // Limpa o display
    ssd1306_send_data(ssd);
}

float get_adc_average(uint adc_input, int samples)
{
    adc_select_input(adc_input); // Seleciona o ADC para o pino desejado

    float sum = 0.0f;
    for (int i = 0; i < samples; i++)
    {
        sum += adc_read();
        sleep_ms(1);
    }
    return sum / samples;
}

float get_normalized_resistance(float value, char *color)
{
    for (int i = -2; i < 8; i++)
    {
        float normalized_resistance = value / pow(10, i);

        if (normalized_resistance >= 10 && normalized_resistance < 100)
        {
            switch (i)
            {
            case -2:
                sprintf(color, "Prateado");
                return normalized_resistance;
            case -1:
                sprintf(color, "Dourado");
                return normalized_resistance;
            case 0:
                sprintf(color, "Preto");
                return normalized_resistance;
            case 1:
                sprintf(color, "Marrom");
                return normalized_resistance;
            case 2:
                sprintf(color, "Vermelho");
                return normalized_resistance;
            case 3:
                sprintf(color, "Laranja");
                return normalized_resistance;
            case 4:
                sprintf(color, "Amarelo");
                return normalized_resistance;
            case 5:
                sprintf(color, "Verde");
                return normalized_resistance;
            case 6:
                sprintf(color, "Azul");
                return normalized_resistance;
            case 7:
                sprintf(color, "Violeta");
                return normalized_resistance;
            default:
                sprintf(color, "Invalida");
                return 0.0f;
            }
        }
    }
}

void get_band_color(int band_number, char *color)
{
    switch (band_number)
    {
    case 0:
        sprintf(color, "Preto");
        break;
    case 1:
        sprintf(color, "Marrom");
        break;
    case 2:
        sprintf(color, "Vermelho");
        break;
    case 3:
        sprintf(color, "Laranja");
        break;
    case 4:
        sprintf(color, "Amarelo");
        break;
    case 5:
        sprintf(color, "Verde");
        break;
    case 6:
        sprintf(color, "Azul");
        break;
    case 7:
        sprintf(color, "Violeta");
        break;
    case 8:
        sprintf(color, "Cinza");
        break;
    case 9:
        sprintf(color, "Branco");
        break;
    default:
        sprintf(color, "Invalida");
        break;
    }
}

float find_nearest_e24(float value)
{
    // Normaliza o valor para estar entre 1.0 e 10.0
    int exponent = 0;
    float normalized_value = value;

    // Primeiro, ajusta para menor que 10
    while (normalized_value >= 10.0)
    {
        normalized_value /= 10.0;
        exponent++;
    }

    printf("Valor normalizado inicial: %.2f\n", normalized_value); // Debug

    // Se o valor normalizado for maior que o maior valor E24 (9.1),
    // ajusta para a próxima década
    if (normalized_value > E24_VALUES[E24_SIZE - 1])
    {
        normalized_value /= 10.0;
        exponent++;
    }

    printf("Valor normalizado final: %.2f\n", normalized_value); // Debug
    printf("Expoente: %d\n", exponent);                          // Debug

    // Encontra o valor mais próximo na série E24
    float nearest = E24_VALUES[0];
    float min_diff = fabs(normalized_value - E24_VALUES[0]);

    for (int i = 1; i < E24_SIZE; i++)
    {
        float diff = fabs(normalized_value - E24_VALUES[i]);
        if (diff < min_diff)
        {
            min_diff = diff;
            nearest = E24_VALUES[i];
        }
    }

    // Retorna o valor ajustado com o expoente
    return nearest * pow(10, exponent);
}
