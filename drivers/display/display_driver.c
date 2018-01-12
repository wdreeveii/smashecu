
#include <stdio.h>

#include "platform.h"
#include "pll/pll_driver.h"
#include "interrupt/interrupt.h"

#define DISPLAY_PWM_CFG PWM_CFG_ZEROCMP | PWM_CFG_ENALWAYS | PWM_CFG_STICKY | 0x4; //PWM_CFG_SCALE

typedef struct{
  unsigned int column;
  unsigned int row;
  uint8_t      vsync;
  uint8_t      hsync;
  uint8_t      vck;
}display_t;

display_t display;

void display_interrupt(void) {

	PWM1_REG(PWM_CFG) &= ~PWM_CFG_CMP0IP;

	/*if (GPIO_REG(GPIO_OUTPUT_VAL) & (0x1 << 23) ) {
		GPIO_REG(GPIO_OUTPUT_VAL) &= ~(0xF << 20);
	} else {
		GPIO_REG(GPIO_OUTPUT_VAL) |= (0xF << 20);
	}*/

	// sync buffer pointer to top of screen
	if ( display.row == 0 ) {
		// enable vsync
		if ( display.hsync == 0 ) {
			display.vsync = 1;
			// enable vsync
			//fprintf(stderr, "v+ ");
			GPIO_REG(GPIO_OUTPUT_VAL) |= (1 << 19);
		}

		if ( display.hsync == 2  && display.vsync ) {
			display.vsync = 0;
			// disable vsync
			//fprintf(stderr, "v- ");
			GPIO_REG(GPIO_OUTPUT_VAL) &= ~(1 << 19);
		}
	}
	// sync buffer pointer to left screen
	if ( display.column == 0 ) {
		// enable hsync
		if ( display.hsync == 0 ) {
			// enable hsync
			//fprintf(stderr, "h+ vck- vid-\n");
			// disable vck
			// disable vid0-3
			GPIO_REG(GPIO_OUTPUT_VAL) |= (1 << 18 );
			GPIO_REG(GPIO_OUTPUT_VAL) &= ~(( 1 << 13) | (0xF << 20));
			display.hsync += 1;
			return;
		}

		if ( display.hsync == 1 ) {
			// disable hsync
			//fprintf(stderr, "h-\n");
			GPIO_REG(GPIO_OUTPUT_VAL) &= ~( 1 << 18 );
			display.hsync += 1;
			return;
		}

	}

	if ( display.hsync == 2 ) {
		if ( display.vck == 0 ) {
			// enable vck
			//fprintf(stderr, "vck+ vid+\n");
			// enable vid0-3
			GPIO_REG(GPIO_OUTPUT_VAL) |= ( 0xA << 20 ) | ( 1 << 13 );
		}

		if ( display.vck == 1 ) {
			// disable vck
			//fprintf(stderr, "vck-\n");
			GPIO_REG(GPIO_OUTPUT_VAL) &= ~( 1 << 13 );

			display.column += 1;
			if ( display.column >= 160 ) {
				display.column = 0;
				display.hsync = 0;

				display.row += 1;
			}

			if ( display.row >= 200 ) {
				display.row = 0;
				fprintf(stderr, "r\n");
			}
		}
	}

	display.vck = !display.vck;

}

void display_init(plic_instance_t * this_plic, display_t * this_display) {

	display.column = 0;
	display.row    = 0;
	display.hsync  = 0;
	display.vck    = 0;

	GPIO_REG(GPIO_OUTPUT_EN) |= (0xF << 20) | (1 << 19) | (1 << 18) | (1 << 13);

	PWM1_REG(PWM_CFG) = PWM_CFG_ZEROCMP /*PWM_CFG_DEGLITCH |  | PWM_CFG_ENALWAYS /*| PWM_CFG_SCALE*/;
	PWM1_REG(PWM_COUNT) = 0;
	PWM1_REG(PWM_CMP0) = 0x0F << 2;
	PWM1_REG(PWM_COUNT) = 0x0F;

	PLIC_enable_interrupt(this_plic, INT_PWM1_BASE, display_interrupt);
	PLIC_set_priority(this_plic, INT_PWM1_BASE, 1);
	PLIC_complete_interrupt(this_plic, INT_PWM1_BASE);

	PWM1_REG(PWM_COUNT) = 0;
	PWM1_REG(PWM_CFG) = DISPLAY_PWM_CFG;

	printf("pwm_cfg: %X\n", PWM1_REG(PWM_CFG));

}