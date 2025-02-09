#include <stdlib.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "inc/ssd1306.h"
#include "inc/font.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "hardware/irq.h"
#include "ws2812.pio.h"

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C
#define led_pin_g 11
#define led_pin_b 12
#define led_pin_r 13
#define BUTTON_A_PIN 5
#define BUTTON_B_PIN 6
#define WS2812_PIN 7
#define MAX_LEDS 25

ssd1306_t ssd;
volatile int number = 0;  // Número exibido na matriz de LEDs

// Variáveis para controle dos LEDs WS2812
static int sm = 0;  // Máquina de estado do PIO
static PIO pio = pio0;  // Bloco PIO
static uint32_t grb[MAX_LEDS];  // Array para armazenar as cores dos LEDs

// Flags de controle de estado dos LEDs
volatile bool led_green_on = false;
volatile bool led_blue_on = false;
volatile uint32_t last_button_press_time = 0;  // Tempo do último pressionamento

// Função para inicializar o LED RGB
void init_rgb_led() {
    gpio_init(led_pin_r);
    gpio_init(led_pin_g);
    gpio_init(led_pin_b);
    gpio_set_dir(led_pin_r, GPIO_OUT);
    gpio_set_dir(led_pin_g, GPIO_OUT);
    gpio_set_dir(led_pin_b, GPIO_OUT);
}

// Função de debouncing para verificar tempo entre pressionamentos
bool debounce_button(uint32_t button_press_time) {
    uint32_t current_time = time_us_32();
    
    // Verifica se o tempo desde o último pressionamento é maior que o tempo de debounce
    if (current_time - button_press_time < 300000) {  // Aumentei o debounce para 300ms
        return false;  // Ignora a interrupção se for muito rápido (menos que 300ms)
    }
    return true;
}

void button_isr(uint gpio, uint32_t events) {
    static uint32_t last_time = 0;
    uint32_t current_time = to_ms_since_boot(get_absolute_time());

    // Debouncing de 200ms
    if (current_time - last_time < 200) return;  
    last_time = current_time;

    if (gpio == BUTTON_A_PIN) {
        led_green_on = !led_green_on;
        gpio_put(led_pin_g, led_green_on); // Aciona o LED verde

        // Atualiza o display SSD1306 com o estado do LED verde
        ssd1306_fill(&ssd, false);
        ssd1306_draw_string(&ssd, led_green_on ? "LED Verde Ligado" : "LED Verde Desligado", 4, 4);
        ssd1306_send_data(&ssd);

        // Exibe o estado do LED verde na saída USB
        printf(led_green_on ? "LED Verde Ligado\n" : "LED Verde Desligado\n");
    } else if (gpio == BUTTON_B_PIN) {
        led_blue_on = !led_blue_on;
        gpio_put(led_pin_b, led_blue_on); // Aciona o LED azul

        // Atualiza o display SSD1306 com o estado do LED azul
        ssd1306_fill(&ssd, false);
        ssd1306_draw_string(&ssd, led_blue_on ? "LED Azul Ligado" : "LED Azul Desligado", 4, 4);
        ssd1306_send_data(&ssd);

        // Exibe o estado do LED azul na saída USB
        printf(led_blue_on ? "LED Azul Ligado\n" : "LED Azul Desligado\n");
    }
    
    // Debug: Verifica qual botão acionou a interrupção
    printf("Interrupção no Pino %d\n", gpio);
}



// Função para inicializar os botões e suas interrupções
void init_buttons() {
    gpio_init(BUTTON_A_PIN);
    gpio_init(BUTTON_B_PIN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_set_dir(BUTTON_B_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_A_PIN);  // Configura o botão A com pull-up
    gpio_pull_up(BUTTON_B_PIN);  // Configura o botão B com pull-up

    // Configura interrupção para o botão A (borda de subida)
    gpio_set_irq_enabled_with_callback(BUTTON_A_PIN, GPIO_IRQ_EDGE_RISE, true, button_isr);
    
    // Configura interrupção para o botão B (borda de subida)
    gpio_set_irq_enabled_with_callback(BUTTON_B_PIN, GPIO_IRQ_EDGE_RISE, true, button_isr);
}


// Função para inicializar a matriz de LEDs WS2812
void init_ws2812() {
    uint offset = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, false);
    for (int i = 0; i < MAX_LEDS; i++) {
        pio_sm_put_blocking(pio, sm, 0);
    }
}

// Função para converter porcentagens de cores RGB para o formato GRB
static void rgb_to_grb(uint32_t porcentColors[][3]) {
    for (int i = 0; i < MAX_LEDS; i++) {
        uint8_t r = porcentColors[i][0] ? 255 * (porcentColors[i][0] / 100.0) : 0;
        uint8_t g = porcentColors[i][1] ? 255 * (porcentColors[i][1] / 100.0) : 0;
        uint8_t b = porcentColors[i][2] ? 255 * (porcentColors[i][2] / 100.0) : 0;
        grb[i] = (g << 16) | (r << 8) | b;
    }
}

// Função para exibir um número na matriz de LEDs WS2812
void display_number(int number) {
    const uint32_t numbers[10][25] = {
        {0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0}, // 0
        {0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0}, // 1
        {0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0}, // 2
        {0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0}, // 3
        {0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0}, // 4
        {0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 0}, // 5
        {0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 0}, // 6
        {0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0}, // 7
        {0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0}, // 8
        {0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0}  // 9
    };

    // Cores para cada número (R, G, B)
    const uint32_t colors[10][3] = {
        {100, 0, 0},   // Vermelho para 0
        {0, 100, 0},   // Verde para 1
        {0, 0, 100},   // Azul para 2
        {100, 100, 0}, // Amarelo para 3
        {100, 0, 100}, // Magenta para 4
        {0, 100, 100}, // Ciano para 5
        {100, 20, 0},  // Laranja para 6
        {20, 0, 100},  // Roxo para 7
        {100, 100, 100}, // Branco para 8
        {40, 40, 40}   // Cinza para 9
    };

    uint32_t porcentColors[MAX_LEDS][3] = {0};
    for (int i = 0; i < MAX_LEDS; i++) {
        if (numbers[number][i]) {
            porcentColors[i][0] = colors[number][0]; // Vermelho
            porcentColors[i][1] = colors[number][1]; // Verde
            porcentColors[i][2] = colors[number][2]; // Azul
        }
    }

    rgb_to_grb(porcentColors);
    for (int i = 0; i < MAX_LEDS; i++) {
        pio_sm_put_blocking(pio, sm, grb[i] << 8u);
    }
    sleep_us(10);
}

void process_usb_command() {
    if (stdio_usb_connected()) {
        char c = getchar();
        if (c >= '0' && c <= '9') {
            display_number((c - '0'));
        }

        // Exibe o caractere no display SSD1306
        ssd1306_fill(&ssd, false);
        ssd1306_draw_string(&ssd, &c, 4, 4);
        ssd1306_send_data(&ssd);

        printf("%c <- Eco do RP2\n", c);
    }
}

int main() {
    stdio_init_all();
    stdio_usb_init();  // Habilita a comunicação USB
    init_rgb_led();
    init_buttons();
    init_ws2812();
    
    // Inicialização do display SSD1306
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_send_data(&ssd);

    // Mensagem inicial via USB
    printf("Digite para ver o eco do RP2 e no Display OLED - E um número (0-9) para visualizar na matriz de LED:\n");

    ssd1306_fill(&ssd, false);
    ssd1306_draw_string(&ssd, "abcdefghijklmno", 4, 4);
    ssd1306_draw_string(&ssd, "pqrstuvwxyz", 4, 14);
    ssd1306_draw_string(&ssd, "ABCDEFGHIJKLMNO", 4, 24);
    ssd1306_draw_string(&ssd, "PQRSTUVWXYZ", 4, 34);
    ssd1306_send_data(&ssd);

    // Loop principal
    while (true) {
        process_usb_command();
        sleep_ms(1000);
    }
}