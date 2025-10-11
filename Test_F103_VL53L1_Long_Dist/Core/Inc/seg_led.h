/*
 * seg_led.h
 *
 *  Created on: Sep 23, 2025
 *      Author: Nagaraju Lakkaraju
 */

#ifndef SEG_LED_H_
#define SEG_LED_H_

#include <stdint.h>

extern uint16_t dist;

void seg_led_gpio_init(void);
void seg_led_digit_off_all(void);
void seg_led_digit_on(uint8_t d);
void seg_led_write(uint8_t segbits);
void seg_led_display_number(uint16_t num);
void seg_led_digit_change(void);

#endif /* SEG_LED_H_ */

