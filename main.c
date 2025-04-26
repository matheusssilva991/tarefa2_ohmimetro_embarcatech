#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "pico/bootrom.h"

#include "hardware/adc.h"
#include "hardware/i2c.h"

#include "lib/ssd1306.h"
#include "lib/font.h"
#include "lib/button.h"

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define DISPLAY_ADDRESS 0x3C
#define ADC_PIN 28     // GPIO para o voltímetro

const int ADC_RESOLUTION = 4095; // Resolução do ADC (12 bits)
const float KNOWN_R_VALUE = 9910;    // Resistor de 10k ohm
const float ADC_VREF = 3.31;     // Tensão de referência do ADC

float r_x = 0.0;           // Resistor desconhecido

void gpio_irq_handler(uint gpio, uint32_t events);
void init_display(ssd1306_t *ssd);
float get_adc_average(uint adc_input, int samples);

int main()
{
  stdio_init_all(); // Inicializa a comunicação serial

  float tensao;
  char str_x[5]; // Buffer para armazenar a string
  char str_y[5]; // Buffer para armazenar a string
  ssd1306_t ssd; // Envia os dados para o display
  bool cor = true;
  float average = 0.0f;

  init_btns();   // Inicializa os botões
  adc_init();
  init_display(&ssd); // Inicializa o display
  adc_gpio_init(ADC_PIN); // GPIO 28 como entrada analógica

  // Define a interrupt handler para o botão B
  gpio_set_irq_enabled_with_callback(BUTTON_B_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

  while (true)
  {
    average = get_adc_average(2, 500); // Lê o valor médio do ADC

    // Fórmula simplificada: R_x = R_conhecido * ADC_encontrado /(ADC_RESOLUTION - adc_encontrado)
    r_x = (KNOWN_R_VALUE * average) / (ADC_RESOLUTION - average);

    sprintf(str_x, "%1.0f", average); // Converte o inteiro em string
    sprintf(str_y, "%1.0f", r_x);   // Converte o float em string

    // cor = !cor;
    //  Atualiza o conteúdo do display com animações
    ssd1306_fill(&ssd, !cor);                     // Limpa o display
    ssd1306_rect(&ssd, 3, 3, 122, 60, cor, !cor); // Desenha um retângulo

    ssd1306_draw_string(&ssd, "Cor 1", 8, 6);
    ssd1306_draw_string(&ssd, "Vermelha", 54, 6);
    ssd1306_line(&ssd, 3, 15, 123, 15, cor); // Desenha uma linha

    ssd1306_draw_string(&ssd, "Cor 2", 8, 18); // Desenha uma string
    ssd1306_draw_string(&ssd, "Vermelha", 54, 18);
    ssd1306_line(&ssd, 3, 27, 123, 27, cor); // Desenha uma linha

    ssd1306_draw_string(&ssd, "Cor 3", 8, 29); // Desenha uma string
    ssd1306_draw_string(&ssd, "Vermelha", 54, 29);
    ssd1306_line(&ssd, 3, 37, 123, 37, cor); // Desenha uma linha

    ssd1306_draw_string(&ssd, "ADC", 13, 41);       // Desenha uma string
    ssd1306_draw_string(&ssd, "Resisten.", 50, 41); // Desenha uma string
    ssd1306_line(&ssd, 44, 37, 44, 60, cor);        // Desenha uma linha vertical

    ssd1306_draw_string(&ssd, str_x, 8, 52);  // Desenha uma string
    ssd1306_draw_string(&ssd, str_y, 59, 52); // Desenha uma string
    ssd1306_send_data(&ssd);                  // Atualiza o display
    sleep_ms(700);
  }
}

void gpio_irq_handler(uint gpio, uint32_t events) {
  reset_usb_boot(0, 0);
}

void init_display(ssd1306_t *ssd) {
  // I2C Initialisation. Using it at 400Khz.
  i2c_init(I2C_PORT, 400 * 1000);

  gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);                           // Set the GPIO pin function to I2C
  gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);                           // Set the GPIO pin function to I2C
  gpio_pull_up(I2C_SDA);                                               // Pull up the data line
  gpio_pull_up(I2C_SCL);                                               // Pull up the clock line
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

float get_adc_average(uint adc_input, int samples) {
  adc_select_input(adc_input); // Seleciona o ADC para o pino desejado

  float sum = 0.0f;
  for (int i = 0; i < samples; i++)
  {
    sum += adc_read();
    sleep_ms(1);
  }
  return sum / samples;
}
