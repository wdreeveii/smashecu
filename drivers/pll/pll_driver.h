#ifndef _PLL_DRIVER_H
#define _PLL_DRIVER_H

void use_default_clocks();
void use_pll(int refsel, int bypass, int r, int f, int q);
unsigned long get_cpu_freq(void);
//unsigned long get_timer_freq(void);
//uint64_t get_timer_value(void);


#endif
