#ifndef LED_MATRIX_COLORS_H
#define LED_MATRIX_COLORS_H

#define LED_MATRIX_COUNT 25

// Enumeração das cores
enum colors
{
    PRETO,
    MARROM,
    VERMELHO,
    LARANJA,
    AMARELO,
    VERDE,
    AZUL,
    VIOLETA,
    CINZA,
    BRANCO
};

// Declarações das funções e variáveis externas
extern const int colors_matrix[10][3];
enum colors string_to_color(const char *color_name);

#endif // LED_MATRIX_COLORS_H
