#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "platform.h"
#include "encoding.h"
#include "interrupt/interrupt.h"
#include "uart/uart_driver.h"

// Structures for registering different interrupt handlers
// for different parts of the application.


void no_interrupt_handler (void) {
  printf("TRAP: Global Interrupt\n");
};

extern plic_instance_t g_plic;

/*Entry Point for PLIC Interrupt Handler*/
void handle_m_ext_interrupt() {
  plic_source int_num  = PLIC_claim_interrupt(&g_plic);
  //printf("Int Num: %d\n", int_num);
  if ((int_num >=1 ) && (int_num < PLIC_NUM_INTERRUPTS)) {
    g_plic.interrupt_handlers[int_num]();
  }
  else {
    exit(1 + (uintptr_t) int_num);
  }
  PLIC_complete_interrupt(&g_plic, int_num);
}


/*Entry Point for Machine Timer Interrupt Handler*/
void handle_m_time_interrupt() {

  clear_csr(mie, MIP_MTIP);

  volatile uint64_t * mtime       = (uint64_t*) (CLINT_CTRL_ADDR + CLINT_MTIME);
  volatile uint64_t * mtimecmp    = (uint64_t*) (CLINT_CTRL_ADDR + CLINT_MTIMECMP);

  uint64_t start = *mtime;
  printf("mtime_int\n", start);
  uint64_t printfdone = *mtime;
  uart_write(0, "test\n", 5);
  uint64_t writedone = *mtime;

  printf("printf: %llu write: %llu\n", printfdone - start, writedone - start);
  // Reset the timer for 3s in the future.
  // This also clears the existing timer interrupt.


  uint64_t now = *mtime;
  uint64_t then = now + 2 * RTC_FREQ;
  *mtimecmp = then;
  
  // read the current value of the LEDS and invert them.
  //uint32_t leds = GPIO_REG(GPIO_OUTPUT_VAL);

  //GPIO_REG(GPIO_OUTPUT_VAL) ^= ((0x1 << RED_LED_OFFSET)   |
  //      (0x1 << GREEN_LED_OFFSET) |
  //      (0x1 << BLUE_LED_OFFSET));
  
  // Re-enable the timer interrupt.
  set_csr(mie, MIP_MTIP);

}

uintptr_t handle_trap(uintptr_t mcause, uintptr_t epc)
{

  if ((mcause & MCAUSE_INT) && ((mcause & MCAUSE_CAUSE) == IRQ_M_EXT)) {
    handle_m_ext_interrupt();
    // External Machine-Level interrupt from PLIC
  } else if ((mcause & MCAUSE_INT) && ((mcause & MCAUSE_CAUSE) == IRQ_M_TIMER)){
    handle_m_time_interrupt();
  }
  else {
    write(1, "trap\n", 5);
    exit(1 + mcause);
  }

  return epc;
}

