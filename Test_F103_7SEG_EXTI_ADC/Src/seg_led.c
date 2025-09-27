/*
 * seg_led.c
 *
 *  Created on: Sep 23, 2025
 *      Author: Nagaraju Lakkaraju
 */

#include <stdio.h>
#include <stdint.h>
#include "stm32f103x.h"

volatile uint32_t msTicks = 0;

/* ---- 7-Segment Lookup Table ------- *
 * Segments (7 pins):
 * PA3 → Segment A
 * PA4 → Segment B
 * PA5 → Segment C
 * PA6 → Segment D
 * PA7 → Segment E
 * PB12 → Segment F
 * PB13 → Segment G
 *
 * Digit selects (4 pins):
 * PB0 → Digit 1
 * PB1 → Digit 2
 * PB2 → Digit 3
 * PB10 → Digit 4
 */
const uint8_t seg_table[10] = {
/* Common Cathode 7 Segment Display */
#if 1
/*----*gfedcba--*/
    0b00111111, // 0
    0b00000110, // 1
    0b01011011, // 2
    0b01001111, // 3
    0b01100110, // 4
    0b01101101, // 5
    0b01111101, // 6
    0b00000111, // 7
    0b01111111, // 8
    0b01101111  // 9
#endif
	/* Common Anode 7 Segment Display */
#if 0
	/*----*gfedcba--*/
	    0b01000000, // 0
	    0b01111001, // 1
	    0b00100100, // 2
	    0b00110000, // 3
	    0b00011001, // 4
	    0b00010010, // 5
	    0b00000010, // 6
	    0b01111000, // 7
	    0b00000000, // 8
	    0b00010000  // 9
#endif
};


/* -------------------- Global Variables -------------------- */
static uint8_t digits[4] = {0, 0, 0, 0};  // digits to display
static uint8_t current_digit = 0;
uint16_t adc_val = 1234;

/* -------------------- GPIO Initialization ----------------- */
void seg_led_gpio_init(void)
{
//	uint8_t dpins[4] = {0, 1, 2, 10};
//	uint8_t idx, pin;

	/* // Enable GPIOA and GPIOB clocks */
	RCC_APB1ENR |= (IOPAEN | IOPBEN);

    /* Configure PA3..PA7 as output 2MHz push-pull */
    GPIOA_CRL &= ~((0xF << (3 * 4)) | (0xF << (4 * 4)) | \
    		       (0xF << (5 * 4)) | (0xF << (6 * 4)) | \
				   (0xF << (7 * 4)));
    GPIOA_CRL |=  ((0x2 << (3 * 4)) | (0x2 << (4 * 4)) | \
    		       (0x2 << (5 * 4)) | (0x2 << (6 * 4)) | \
				   (0x2 << (7 * 4)));
    /* Configure PB12, PB13 as output 2MHz push-pull */
    GPIOB_CRH &= ~((0xF << ((12-8) * 4)) | (0xF << ((13-8) * 4)));
    GPIOB_CRH |=  ((0x2 << ((12-8) * 4)) | (0x2 << ((13-8) * 4)));

    /* PB0, PB1, PB2, PB10 → digit selects as output 2MHz push-pull */
    GPIOB_CRL &= ~((0xF << (0 * 4)) | (0xF << (1 * 4)) | (0xF << (2 * 4)));
    GPIOB_CRL |=  ((0x2 << (0 * 4)) | (0x2 << (1 * 4)) | (0x2 << (2 * 4)));
    GPIOB_CRH &= ~(0xF << ((10-8) * 4));
    GPIOB_CRH |=  (0x2 << ((10-8) * 4));
}

/* --------- Digit & Segment Control -------- */
void seg_led_digit_off_all(void)
{
    GPIOB_ODR &= ~((1U << 0) | (1U << 1) | (1U << 2) | (1U << 10));
}

void seg_led_digit_on(uint8_t d)
{
	/* Turn off all digits first */
    seg_led_digit_off_all();

    /* Enable selected digit */
    switch(d) {
    case 0:
		GPIOB_ODR |= (1U << 0);
		break;
    case 1:
		GPIOB_ODR |= (1U << 1);
		break;
    case 2:
		GPIOB_ODR |= (1U << 2);
		break;
    case 3:
		GPIOB_ODR |= (1U << 10);
		break;
    }
}

void seg_led_write(uint8_t segbits)
{
	uint8_t idx;

//	printf("%s(): segbits: 0x%02X\n", __func__, segbits);
	/* PA3..PA7 → bits 0..4 */
    for(idx = 0; idx <= 4; idx++) {
        if(segbits & (1U << idx))
			GPIOA_ODR |= (1U << (idx + 3));
        else
			GPIOA_ODR &= ~(1U << (idx + 3));
    }
    /* PB12 → bit5, PB13 → bit6 */
    if(segbits & (1U << 5))
		GPIOB_ODR |= (1U << 12);
	else
		GPIOB_ODR &= ~(1U << 12);
    if(segbits & (1U << 6))
		GPIOB_ODR |= (1U << 13);
	else
		GPIOB_ODR &= ~(1U << 13);
}

/* ------ Display Number Function -------- */
void seg_led_display_number(uint16_t num)
{
    if(num > 9999)
		num = 9999; // limit to 4 digits

    digits[0] = (num / 1000) % 10;  // thousands
    digits[1] = (num / 100) % 10;   // hundreds
    digits[2] = (num / 10) %10;     // tens
    digits[3] = num % 10;           // units
//    printf("D[0]:%u, D[1]:%u, D[2]:%u, D[3]:%u\n", digits[0], digits[1], digits[2], digits[3]);
}


void delay_ms(uint32_t ms)
{
    uint32_t target = msTicks + ms;

    while (msTicks < target);   // Busy wait
}

/* SysTick Handler: called every 0.5 ms */
void SysTick_Handler(void)
{
	msTicks++;   // Increment every 1 ms

//	seg_led_display_number(adc_val);
	seg_led_write(seg_table[digits[current_digit]]);
	seg_led_digit_on(current_digit);

	current_digit++;
	if(current_digit >= 4)
		current_digit = 0;
}


