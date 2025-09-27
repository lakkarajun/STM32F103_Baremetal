/*
 * console.h
 *
 *  Created on: Sep 23, 2025
 *      Author: Nagaraju Lakkaraju
 */

#ifndef CONSOLE_H_
#define CONSOLE_H_

#include <stdio.h>

void uart1_init(void);
void uart1_tx(char c);
char uart1_rx(void);

#endif /* CONSOLE_H_ */
