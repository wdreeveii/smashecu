#ifndef _ATOMIC_H
#define _ATOMIC_H

#include "encoding.h"

static __inline__ uint8_t __iDisable(void) {
	clear_csr(mstatus, MSTATUS_MIE);
	return 1;
}

static __inline__ void __iRestore(const  unsigned long *__s) {
	write_csr(mstatus, *__s);
}

#define ATOMIC_BLOCK(type) for ( type, __ToDo = __iDisable(); __ToDo ; __ToDo = 0 )

#define ATOMIC_RESTORESTATE unsigned long csr_save __attribute__((__cleanup__(__iRestore))) = read_csr(mstatus)

#endif