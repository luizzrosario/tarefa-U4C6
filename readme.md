# Tarefa - U4C6

Este projeto tem o objetivo de controlar LEDs RGB, uma matriz de LEDs WS2812, e exibir informações em um display OLED SSD1306. A interação com o sistema é feita através de botões físicos, comandos via USB e visualização no display OLED.

## Funcionalidades

1. **Controle de LEDs RGB**: Dois LEDs RGB (verde e azul) são controlados por botões físicos conectados aos pinos GPIO da Raspberry Pi Pico.
2. **Matriz de LEDs WS2812**: Um display de LEDs WS2812 exibe números de 0 a 9 em uma configuração de 5x5 LEDs, cada número sendo mostrado com cores específicas.
3. **Display SSD1306**: O display OLED SSD1306 exibe informações sobre o estado dos LEDs RGB e também reflete o número digitado via USB.
4. **Interrupções**: Os botões são configurados com interrupções para controle eficiente, utilizando um sistema de "debouncing" para evitar múltiplos acionamentos rápidos.
5. **Comunicação USB**: O microcontrolador comunica-se via USB para receber comandos de números (0-9), os quais são refletidos no display e nos LEDs.

## Componentes Utilizados

- **Raspberry Pi Pico** (RP2040)
- **Display OLED SSD1306**
- **LEDs RGB** (Pinos GPIO 11, 12, 13)
- **Matriz de LEDs WS2812** (Pino GPIO 7)
- **Botões** (Pinos GPIO 5 e 6)

## Funcionalidades de Software

### Controle dos LEDs RGB

Através dos botões físicos conectados aos pinos GPIO, é possível ligar e desligar os LEDs RGB (verde e azul). O estado dos LEDs é refletido no display OLED SSD1306 e na saída USB.

### Exibição de Números na Matriz de LEDs WS2812

A matriz de LEDs WS2812 exibe números de 0 a 9. Cada número é representado por uma configuração de LEDs acesos em uma grade de 5x5, com diferentes cores para cada número (vermelho, verde, azul, etc.).

### Comunicação USB

O programa permite que o usuário envie comandos através da porta USB, digitando um número de 0 a 9. Esse número é exibido tanto no display OLED SSD1306 quanto na matriz de LEDs WS2812.

### Debouncing de Botões

A função de debouncing é implementada para garantir que os botões não sejam acionados várias vezes rapidamente, estabilizando a entrada do usuário.

## Font.h - Implementações Personalizadas

Na biblioteca `font.h`, foi implementada a funcionalidade de exibir caracteres minúsculos. Além disso, a função `draw_char` foi modificada para suportar tanto caracteres maiúsculos quanto minúsculos, permitindo uma exibição mais completa no display OLED SSD1306.

## Funcionalidades de Hardware

- **Display SSD1306**: Conectado via I2C (pinos SDA e SCL).
- **Botões**: Conectados aos pinos GPIO 5 e 6 com pull-up configurado.
- **LEDs RGB**: Conectados aos pinos GPIO 11, 12 e 13 para controle de cores.
- **Matriz de LEDs WS2812**: Conectada ao pino GPIO 7.

## Instruções de Instalação

1. **Clonar o repositório**:
   ```bash
   git clone <URL_DO_REPOSITORIO>
   ```

2. **Compilar o código**:
   Utilize o CMake para compilar o código para o RP2040.

   ```bash
   mkdir build
   cd build
   cmake ..
   make
   ```

3. **Carregar o código no RP2040**:
   Copie o arquivo compilado para a memória flash do microcontrolador.

4. **Conectar via USB**:
   O projeto utiliza comunicação USB, então conecte a Raspberry Pi Pico ao computador via cabo USB e abra um terminal serial para visualizar as saídas.
