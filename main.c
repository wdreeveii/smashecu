
#include <stdio.h>
#include <stdlib.h>
#include "platform.h"
#include <string.h>
#include "interrupt/interrupt.h"
#include "uart/uart_driver.h"
#include "pll/pll_driver.h"
#include "display/display_driver.h"

#include "encoding.h"
#include <unistd.h>
#include "stdatomic.h"




// Instance data for the PLIC.
plic_instance_t g_plic;


static const char greet_msg[] = "\
SMASH ECU \n\
";

void print_greet() {

  write (STDOUT_FILENO, greet_msg, sizeof(greet_msg) - 1);

}

void button_0_handler(void) {

  // Red LED on
  //GPIO_REG(GPIO_OUTPUT_VAL) |= (0x1 << RED_LED_OFFSET);

  // Clear the GPIO Pending interrupt by writing 1.
  //GPIO_REG(GPIO_RISE_IP) = (0x1 << BUTTON_0_OFFSET);

};

int main(int argc, char **argv)
{
  use_default_clocks();
  use_pll(0, 0, 1, 31, 1);

  // Disable the machine & timer interrupts until setup is done.
  clear_csr(mie, MIP_MEIP);
  clear_csr(mie, MIP_MTIP);

  // Set up the PLIC
  PLIC_init(&g_plic,
      PLIC_CTRL_ADDR,
      PLIC_NUM_INTERRUPTS,
      PLIC_NUM_PRIORITIES);

  uart_init(&g_plic, 115200);
  display_init(&g_plic);

  // Enable the Machine-External bit & Machine-Timer bit in MIE
  set_csr(mie, MIP_MEIP);

  // Set the machine timer to go off in 3 seconds.
  // The
  volatile uint64_t * mtime       = (uint64_t*) (CLINT_CTRL_ADDR + CLINT_MTIME);
  volatile uint64_t * mtimecmp    = (uint64_t*) (CLINT_CTRL_ADDR + CLINT_MTIMECMP);
  uint64_t now = *mtime;
  uint64_t then = now + 2*RTC_FREQ;
  *mtimecmp = then;

  //set_csr(mie, MIP_MTIP);
    
  // Enable interrupts in general.
  set_csr(mstatus, MSTATUS_MIE);

  printf("core freq at %d Hz\n", get_cpu_freq());

  print_greet();

  // Set up the GPIOs such that the LED GPIO
  // can be used as both Inputs and Outputs.
  while(1) {
    //printf("d: %d\n", ii++);

    char test[101] = {0};
    int num = read(1, test, 100);
    if (num) {
      printf("Got %d bytes: %s\n", num, test);
    }
  }

  //GPIO_REG(GPIO_OUTPUT_EN)  &= ~((0x1 << BUTTON_0_OFFSET));
  //GPIO_REG(GPIO_PULLUP_EN)  &= ~((0x1 << BUTTON_0_OFFSET));
  //GPIO_REG(GPIO_INPUT_EN)   |=  ((0x1 << BUTTON_0_OFFSET));

  //GPIO_REG(GPIO_INPUT_EN)    &= ~((0x1<< RED_LED_OFFSET) | (0x1<< GREEN_LED_OFFSET) | (0x1 << BLUE_LED_OFFSET)) ;
  //GPIO_REG(GPIO_OUTPUT_EN)   |=  ((0x1<< RED_LED_OFFSET)| (0x1<< GREEN_LED_OFFSET) | (0x1 << BLUE_LED_OFFSET)) ;
  //GPIO_REG(GPIO_OUTPUT_VAL)  |=   (0x1 << BLUE_LED_OFFSET) ;
  //GPIO_REG(GPIO_OUTPUT_VAL)  &=  ~((0x1<< RED_LED_OFFSET) | (0x1<< GREEN_LED_OFFSET)) ;

  
  // For Bit-banging with Atomics demo.
  
  uint32_t bitbang_mask = 0;
#ifdef _SIFIVE_HIFIVE1_H
  bitbang_mask = (1 << PIN_19_OFFSET);
#endif

  GPIO_REG(GPIO_OUTPUT_EN) |= bitbang_mask;
  
  // Have to enable the interrupt both at the GPIO level,
  // and at the PLIC level.
  //PLIC_enable_interrupt (&g_plic, INT_DEVICE_BUTTON_0, button_0_handler);
  
  // Priority must be set > 0 to trigger the interrupt.
  //PLIC_set_priority(&g_plic, INT_DEVICE_BUTTON_0, 1);
  
  //GPIO_REG(GPIO_RISE_IE) |= (1 << BUTTON_0_OFFSET);
  

    


   // Demonstrate fast GPIO bit-banging.
   // One can bang it faster than this if you know
   // the entire OUTPUT_VAL that you want to write, but 
   // Atomics give a quick way to control a single bit.
  // For Bit-banging with Atomics demo.
  
  while (1){
    atomic_fetch_xor_explicit(&GPIO_REG(GPIO_OUTPUT_VAL), bitbang_mask, memory_order_relaxed);
  }

  return 0;
}
