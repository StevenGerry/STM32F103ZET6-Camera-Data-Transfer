#ifndef LED_H
#define LED_H
#include "stm32f10x.h"

#define LEDS_NUMBER    4


#define LED_1          (GPIO_Pin_6 | (1UL<<16))
#define LED_2          (GPIO_Pin_8 | (2UL<<16))
#define LED_3          (GPIO_Pin_8 | (3UL<<16))
#define LED_4          (GPIO_Pin_9 | (4UL<<16))

void leds_init(void);
void leds_off(void);
void leds_on(void);

void led_off(uint32_t led_idx);
void led_on(uint32_t led_idx);
void led_toggle(uint32_t led_idx);


#endif
