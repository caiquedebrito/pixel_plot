#include "joystick.h"
#include "hardware/adc.h"
#include "pico/stdlib.h"
#include <stdio.h>

void move_square(uint8_t *x, uint8_t *y, uint16_t vrx_value, uint16_t vry_value) {
  uint8_t speed = 8;
  
  if (vrx_value < CENTER_VALUE - DEAD_ZONE || vrx_value > CENTER_VALUE + DEAD_ZONE) {
      if (vrx_value < CENTER_VALUE) {
          if (*x > 1) *x -= speed;  // Garante que não ultrapasse a borda esquerda
      } else {
          if (*x < WIDTH - 9) *x += speed; // Garante que não ultrapasse a borda direita
      }
  }

  if (vry_value < CENTER_VALUE - DEAD_ZONE || vry_value > CENTER_VALUE + DEAD_ZONE) {
      if (vry_value < CENTER_VALUE) {
          if (*y < HEIGHT - 9) *y += speed; // Garante que não ultrapasse a borda inferior
      } else {
          if (*y > 1) *y -= speed; // Garante que não ultrapasse a borda superior
      }
  }
}

int map_adc_to_pos(uint16_t adc_value) {
  return (adc_value * 5) / 4096; 
}

void adc_config() {
    adc_init();
    adc_gpio_init(JOYSTICK_X_PIN); 
    adc_gpio_init(JOYSTICK_Y_PIN);
}

void joystick_read_axis(uint16_t *vrx_value, uint16_t *vry_value)
{
    adc_select_input(ADC_CHANNEL_1);
    sleep_us(2);                     
    *vrx_value = adc_read();         

    adc_select_input(ADC_CHANNEL_0); 
    sleep_us(2);                     
    *vry_value = adc_read();         

    printf("X: %d, Y: %d\n", *vrx_value, *vry_value);
}