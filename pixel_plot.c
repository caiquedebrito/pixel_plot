#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "lib/display/ssd1306.h"
#include "lib/joystick/joystick.h"
#include "pico/bootrom.h"
#include "hardware/pio.h"
#include "ws2812.pio.h"
#include "hardware/timer.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define address 0x3C

#define BUTTON_A 5

#define BUTTON_B 6

#define IS_RGBW false
#define WS2812_PIN 7

#define BUZZER_PIN 21

#define BUZZER_FREQUENCY 200

const int LED_B = 13;                    
const int LED_R = 11;                    
const float DIVIDER_PWM = 16.0;          
const uint16_t PERIOD = 4096;            
uint16_t led_b_level, led_r_level = 100; 
uint slice_led_b, slice_led_r;          

ssd1306_t ssd;

bool show_menu = true;

typedef enum {
    DISPLAY_OLED,
    MATRIZ_5x5
} MenuOption;

int color_index = 0;

const uint8_t BRIGHTNESS = 10;

const uint8_t COLORS[][3] = {
    {BRIGHTNESS, 0, 0}, // Vermelho
    {0, BRIGHTNESS, 0}, // Verde
    {0, 0, BRIGHTNESS}, // Azul
    {BRIGHTNESS, BRIGHTNESS, 0}, // Amarelo
    {BRIGHTNESS, 0, BRIGHTNESS}, // Magenta
    {0, BRIGHTNESS, BRIGHTNESS}, // Ciano
    {BRIGHTNESS, BRIGHTNESS, BRIGHTNESS} // Branco
};

void pwm_init_buzzer(uint pin) {
    gpio_set_function(pin, GPIO_FUNC_PWM);

    uint slice_num = pwm_gpio_to_slice_num(pin);

    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, clock_get_hz(clk_sys) / (BUZZER_FREQUENCY * 4096));
    pwm_init(slice_num, &config, true);

    pwm_set_gpio_level(pin, 0);
}

void setup_pwm_led(uint led, uint *slice, uint16_t level)
{
  gpio_set_function(led, GPIO_FUNC_PWM); // Configura o pino do LED como saída PWM
  *slice = pwm_gpio_to_slice_num(led);   // Obtém o slice do PWM associado ao pino do LED
  pwm_set_clkdiv(*slice, DIVIDER_PWM);   // Define o divisor de clock do PWM
  pwm_set_wrap(*slice, PERIOD);          // Configura o valor máximo do contador (período do PWM)
  pwm_set_gpio_level(led, level);        // Define o nível inicial do PWM para o LED
  pwm_set_enabled(*slice, true);         // Habilita o PWM no slice correspondente ao LED
}

void play_note(int frequency, int duration) {
    uint slice_num = pwm_gpio_to_slice_num(BUZZER_PIN);
    uint32_t wrap = 4095;
    float divider = (float) clock_get_hz(clk_sys) / (frequency * (wrap + 1));
    pwm_set_clkdiv(slice_num, divider);
    uint16_t level = (uint16_t)(((wrap + 1) * 50) / 100); // 50% duty cycle
    pwm_set_gpio_level(BUZZER_PIN, level);
    pwm_set_enabled(slice_num, true);

    sleep_ms(duration);

    pwm_set_enabled(slice_num, false);
}

void go_to_selected_option() {
    show_menu = !show_menu;
}

void gpio_irq_handler(uint gpio, uint32_t events)
{
    volatile static uint32_t last_time = 0;
    volatile uint32_t current_time = to_ms_since_boot(get_absolute_time()); 

    if (current_time - last_time < 400) { // Debounce de 400ms
        return;
    }

    last_time = current_time;

    if (gpio == BUTTON_B) {
        reset_usb_boot(0, 0);
    } else if (gpio == SWITCH_PIN) {
        go_to_selected_option();
    } else if (gpio == BUTTON_A) {
        // Muda a cor do LED da matriz 5x5
        color_index = (color_index + 1) % (sizeof(COLORS) / sizeof(COLORS[0]));
    }
}


void set_one_led(uint8_t r, uint8_t g, uint8_t b) {
    uint32_t color = ((uint32_t)(r) << 8) | ((uint32_t)(g) << 16) | (uint32_t)(b);
    pio_sm_put_blocking(pio0, 0, color << 8u);
}

void clear_leds() {
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            set_one_led(0, 0, 0);
        }
    }
    pwm_set_gpio_level(LED_B, 0);
    pwm_set_gpio_level(LED_R, 0);
}

void move_led(int row, int column) {
    int r = COLORS[color_index][0];
    int g = COLORS[color_index][1];
    int b = COLORS[color_index][2];

    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            if (i == row && j == column) {
                set_one_led(r, g, b);
            } else {
                set_one_led(0, 0, 0);
            }
        }
    }
}

void show_led_position(int row, int column) {
    ssd1306_fill(&ssd, false);
    ssd1306_rect(&ssd, 2, 2, 122, 60, true, false);
    ssd1306_draw_string(&ssd, "Matriz 5x5", 8, 8);
    ssd1306_draw_string(&ssd, "Coluna: ", 8, 18);
    ssd1306_draw_string(&ssd, "Linha: ", 8, 28);
    ssd1306_draw_string(&ssd, "Cor: ", 8, 38);
    ssd1306_draw_char(&ssd, '0' + column, 70, 18);
    ssd1306_draw_char(&ssd, '0' + row, 70, 28);
    ssd1306_draw_char(&ssd, '0' + color_index, 70, 38);

    printf("Coluna: %d, Linha: %d\n", column, row);
}

void gpio_config() {
    gpio_init(SWITCH_PIN);
    gpio_set_dir(SWITCH_PIN, GPIO_IN);
    gpio_pull_up(SWITCH_PIN); 
    gpio_set_irq_enabled_with_callback(SWITCH_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    gpio_init(BUTTON_A);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_pull_up(BUTTON_A);
    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    gpio_init(BUTTON_B);
    gpio_set_dir(BUTTON_B, GPIO_IN);
    gpio_pull_up(BUTTON_B);
    gpio_set_irq_enabled_with_callback(BUTTON_B, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
}

int main()
{
    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);

    stdio_init_all();
    adc_config();

    setup_pwm_led(LED_B, &slice_led_b, led_b_level); // Configura o PWM para o LED azul
  setup_pwm_led(LED_R, &slice_led_r, led_r_level); // Configura o PWM para o LED vermelho
    pwm_init_buzzer(BUZZER_PIN);

    gpio_config();

    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);

    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    ssd1306_init(&ssd, WIDTH, HEIGHT, false, address, I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_send_data(&ssd);

    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);
    
    MenuOption current_option = DISPLAY_OLED;

    uint8_t square_x = 64, square_y = 32;
    uint16_t vrx_value, vry_value;

    while (true) {
        joystick_read_axis(&vrx_value, &vry_value);

        if (show_menu) {
            clear_leds();

            MenuOption new_option = current_option;
            if (vry_value + 100 > 2100) {
                new_option = DISPLAY_OLED;
            } else if (vry_value - 100 < 1800) {
                new_option = MATRIZ_5x5;
            }

            if (new_option != current_option) {
                current_option = new_option;
                play_note(1000, 50);
            }

            ssd1306_fill(&ssd, false);
            ssd1306_rect(&ssd, 2, 2, 122, 60, true, false);
            if (current_option == MATRIZ_5x5) {
                ssd1306_draw_string(&ssd, "  Display OLED", 8, 20);
                ssd1306_draw_string(&ssd, "> Matriz 5x5", 8, 32);
            } else {
                ssd1306_draw_string(&ssd, "> Display OLED", 8, 20);
                ssd1306_draw_string(&ssd, "  Matriz 5x5", 8, 32);
            }
        } else {
            pwm_set_gpio_level(LED_B, vrx_value); // Ajusta o brilho do LED azul com o valor do eixo X
            pwm_set_gpio_level(LED_R, vry_value); // Ajusta o brilho do LED vermelho com o valor do eixo Y
            if (current_option == MATRIZ_5x5) {
                int column = map_adc_to_pos(vrx_value);
                int row = map_adc_to_pos(vry_value);

                if (row % 2 == 0) { // inverte o eixo X para rows pares
                    column = 4 - column;
                }

                move_led(row, column);
                show_led_position(row, column);
                
            } else {
                move_square(&square_x, &square_y, vrx_value, vry_value);
                
                ssd1306_fill(&ssd, false);
                ssd1306_rect(&ssd, square_y, square_x, 8, 8, true, true);
            }
        }
            
        ssd1306_send_data(&ssd);
    
        sleep_ms(100);
    }
}
