# Ohmímetro Digital com Display OLED para BitDogLab

## Visão Geral
Implementação de um ohmímetro digital utilizando a placa BitDogLab e display OLED SSD1306. O projeto mede valores de resistores desconhecidos e exibe tanto o valor medido quanto seu equivalente comercial mais próximo da série E24.

## Descrição Funcional
O ohmímetro utiliza um circuito divisor de tensão com um resistor de referência conhecido para medir valores de resistência desconhecidos. Características:
- Medição de resistência em tempo real
- Conversão automática para o valor mais próximo da série E24
- Exibição das cores das faixas do resistor
- Interface interativa via display OLED
- Matriz de LEDs para feedback visual

## Requisitos de Hardware
- Placa BitDogLab
- Display OLED SSD1306 (128x64)
- Matriz de LEDs WS2812B
- Resistor desconhecido para medição
- Resistor de referência (10kΩ)

## Uso dos Periféricos da BitDogLab
1. **ADC (GPIO 28)**
   - Utilizado para medição de tensão analógica
   - Resolução de 12 bits
   - Tensão de referência: 3,3V

2. **Comunicação I2C (i2c1)**
   - SDA: GPIO 14
   - SCL: GPIO 15
   - Endereço do Display: 0x3C

3. **Botões**
   - Botão B (GPIO 6): Ativação do modo BOOTSEL
   - Utilizado para controle do sistema e navegação

4. **Matriz de LEDs WS2812B (GPIO 7)**
   - Representação visual das cores das faixas do resistor
   - Feedback em tempo real
   - Configuração de matriz 5x5

## Compilação e Execução
1. Clone o repositório
2. Compile usando CMake:
```bash
mkdir build
cd build
cmake ..
make
```
3. Conecte a placa BitDogLab
4. Faça o upload do arquivo binário para a placa

## Conexão do Circuito
- Conecte o resistor conhecido (10kΩ) entre 3,3V e a entrada do ADC
- Conecte o resistor desconhecido entre a entrada do ADC e GND
- Conecte o display OLED aos pinos I2C

## Observações
- A funcionalidade do botão BOOTSEL deve ser removida na versão final de produção
- A precisão da medição depende da precisão do resistor de referência
- Suporta medições de resistência na faixa de 100Ω a 1MΩ
