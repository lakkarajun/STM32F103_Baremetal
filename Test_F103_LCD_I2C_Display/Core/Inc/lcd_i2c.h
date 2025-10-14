/*
 * lcd_i2c.h
 *
 *  Created on: Oct 12, 2025
 *      Author: Nagaraju Lakkaraju
 */

#ifndef INC_LCD_I2C_H_
#define INC_LCD_I2C_H_

#include "stm32f1xx_hal.h"
#include "string.h"

void lcd_init(void);
void lcd_send_cmd(uint8_t cmd);
void lcd_send_data(uint8_t data);
void lcd_clear(void);
void lcd_put_cur(int row, int col);
void lcd_send_string(char *str);
void lcd_backlight_test(void);

#endif /* INC_LCD_I2C_H_ */
