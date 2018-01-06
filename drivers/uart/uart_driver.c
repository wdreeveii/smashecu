
#include <stdio.h>

#include "platform.h"
#include "pll/pll_driver.h"
#include "interrupt/interrupt.h"
#include "uart/uart_driver.h"
#include "buffer.h"
#include "atomic.h"

char rxstore[UART_BUFFER_SIZE];
char txstore[UART_BUFFER_SIZE];

buffer_t rxbuffer;
buffer_t txbuffer;


void uart_interrupt(void) {

	uint32_t ip = UART0_REG(UART_REG_IP);

	if ( ip & UART_IP_TXWM ) {
		uint8_t val;
		if ( !buffer_pop(&txbuffer, &val) ) {
			UART0_REG(UART_REG_IE) &= ~UART_IP_TXWM;
		} else {
			UART0_REG(UART_REG_TXFIFO) = val;
		}
	}

	if ( ip & UART_IP_RXWM ) {

		//uint8_t fifo[8];
		int ii = 0;
		for (; ii < 8; ii++ ) {
			uint32_t val = UART0_REG(UART_REG_RXFIFO);
			if ( val & 0x80000000 ) {
				// invalid character
				// fifo empty
				break;
			} else {
				if ( val == '\r' ) val = '\n';
				//fifo[ii] = val;

				// enqueue here or echo back for REPL ORRR?
				buffer_push(&rxbuffer, val);
			}
		}

		//if (ii > 0)
		//	uart_write(0, fifo, ii);
	}
}


void uart_init(plic_instance_t * this_plic, size_t baud_rate) {
	buffer_init(&rxbuffer, rxstore, UART_BUFFER_SIZE);
	buffer_init(&txbuffer, txstore, UART_BUFFER_SIZE);

	GPIO_REG(GPIO_IOF_SEL) &= ~IOF0_UART0_MASK;
	GPIO_REG(GPIO_IOF_EN) |= IOF0_UART0_MASK;
	UART0_REG(UART_REG_DIV) = get_cpu_freq() / baud_rate - 1;
	UART0_REG(UART_REG_TXCTRL) = UART_TXEN | UART_TXWM(1);
	UART0_REG(UART_REG_RXCTRL) = UART_RXEN | UART_RXWM(0);

	// drain rx fifo
	for (int ii = 0; ii < 8; ii++ ) {
		uint32_t val = UART0_REG(UART_REG_RXFIFO);
		if ( val & 0x80000000 ) break;
	}

	UART0_REG(UART_REG_IE) |= UART_IP_RXWM;

	PLIC_enable_interrupt(this_plic, INT_UART0_BASE, uart_interrupt);
	PLIC_set_priority(this_plic, INT_UART0_BASE, 2);
	PLIC_complete_interrupt(this_plic, INT_UART0_BASE);

}

ssize_t uart_write(int fd, const uint8_t* ptr, size_t len) {

	for (size_t ii = 0; ii < len; ii++) {
		char status;
		do {
			ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
				status = buffer_push(&txbuffer, ptr[ii]);
			}
		} while (!status && fd != 2);
		if (ptr[ii] == '\n')
			do {
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
					status = buffer_push(&txbuffer, '\r');
				}
			} while (!status && fd != 2);
	}

	UART0_REG(UART_REG_IE) |= UART_IP_TXWM;

	return len;
}

/*ssize_t uart_write(int fd, const uint8_t* ptr, size_t len) {
	for (size_t jj = 0; jj < len; jj++) {
		while (UART0_REG(UART_REG_TXFIFO) & 0x80000000) ;
		UART0_REG(UART_REG_TXFIFO) = ptr[jj];

		if (ptr[jj] == '\n') {
        	while (UART0_REG(UART_REG_TXFIFO) & 0x80000000) ;
        	UART0_REG(UART_REG_TXFIFO) = '\r';
    	}
    }

    return len;
}*/

ssize_t uart_read(int fd, uint8_t* ptr, size_t len) {
	int ii = 0;

	for (; ii < len; ii++) {
		uint8_t val;
		
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
			if ( !buffer_pop(&rxbuffer, &val) ) {
				return ii;
			}
		}
		
		*(ptr + ii) = val;
	}

	return ii;
}
