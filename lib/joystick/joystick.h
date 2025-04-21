#ifndef JOYSTICK_H
#define JOYSTICK_H
#include <stdint.h>

#define WIDTH 128
#define HEIGHT 64

#define CENTER_VALUE 2048 // Valor central do joystick
#define DEAD_ZONE 200  // Zona morta ao redor do centro

#define JOYSTICK_Y_PIN 26
#define JOYSTICK_X_PIN 27
#define SWITCH_PIN 22

#define ADC_CHANNEL_0 0   // Canal ADC para o eixo X
#define ADC_CHANNEL_1 1   // Canal ADC para o eixo Y

void move_square(uint8_t *x, uint8_t *y, uint16_t vrx_value, uint16_t vry_value);
int map_adc_to_pos(uint16_t adc_value);
void adc_config();
void joystick_read_axis(uint16_t *vrx_value, uint16_t *vry_value);

#endif // JOYSTICK_H