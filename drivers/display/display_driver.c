
#include <stdio.h>

#include "platform.h"
#include "pll/pll_driver.h"
#include "interrupt/interrupt.h"

#define DISPLAY_PWM_CFG PWM_CFG_ZEROCMP | PWM_CFG_ENALWAYS | PWM_CFG_STICKY | PWM_CFG_SCALE

void display_interrupt(void) {
	fprintf(stderr, "pwm_cfg: %X\n", PWM1_REG(PWM_CFG));
	PWM1_REG(PWM_CFG) = DISPLAY_PWM_CFG;
	fprintf(stderr, "pwm_cfg: %X\n", PWM1_REG(PWM_CFG));
}

void display_init(plic_instance_t * this_plic) {

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