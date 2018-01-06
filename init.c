#include "platform.h"
#include "encoding.h"
#include "interrupt/interrupt.h"

void _init()
{
  write_csr(mtvec, &trap_entry);
  
  if (read_csr(misa) & (1 << ('F' - 'A'))) { // if F extension is present
    write_csr(mstatus, MSTATUS_FS); // allow FPU instructions without trapping
    write_csr(fcsr, 0); // initialize rounding mode, undefined at reset
  }
  
}

void _fini()
{
}
