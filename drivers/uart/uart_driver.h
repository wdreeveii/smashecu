#ifndef _UART_DRIVER_H
#define _UART_DRIVER_H

#include "interrupt/interrupt.h"

#define UART_BUFFER_SIZE 256

void uart_init(plic_instance_t * this_plic, size_t baud_rate);
//ssize_t uart_int_write(int fd, const uint8_t* ptr, size_t len);
ssize_t uart_write(int fd, const uint8_t* ptr, size_t len);
ssize_t uart_read(int fd, uint8_t* ptr, size_t len);
#endif
