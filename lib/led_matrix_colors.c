#include <string.h>
#include "led_matrix_colors.h"

// Definição da matriz de cores
const int colors_matrix[10][3] = {
    {0, 0, 0}, // Preto // Correto
    {30, 8, 0}, // Marrom // Correto
    {8, 0, 0}, // Vermelho // Correto
    {12, 8, 0}, // Laranja // Correto
    {4, 8, 0}, // Amarelo // Correto
    {0, 8, 0}, // Verde // Correto
    {0, 0, 8}, // Azul // Correto
    {8, 0, 8}, // Violeta // Correto
    {8, 8, 8}, // Cinza // Correto
    {12, 12, 12}  // Branco // Correto
};

// Implementação da função de conversão string para enum
enum colors string_to_color(const char *color_name)
{
    if (strcmp(color_name, "Preto") == 0)
        return PRETO;
    if (strcmp(color_name, "Marrom") == 0)
        return MARROM;
    if (strcmp(color_name, "Vermelho") == 0)
        return VERMELHO;
    if (strcmp(color_name, "Laranja") == 0)
        return LARANJA;
    if (strcmp(color_name, "Amarelo") == 0)
        return AMARELO;
    if (strcmp(color_name, "Verde") == 0)
        return VERDE;
    if (strcmp(color_name, "Azul") == 0)
        return AZUL;
    if (strcmp(color_name, "Violeta") == 0)
        return VIOLETA;
    if (strcmp(color_name, "Cinza") == 0)
        return CINZA;
    if (strcmp(color_name, "Branco") == 0)
        return BRANCO;
    return PRETO; // valor default
}
