#ifndef _UART_CONTROL_H
#define _UART_CONTROL_H
#include "main.h"
void usart1_init();
void usart_control_mode();
void track_speed_set();
void turn_speed_set();
void uart_error(void);
#endif