TARGET           = smashecu

RISCV_ARCH      := rv32imac
RISCV_ABI       := ilp32

TOOL_DIR        := ~/freedom-e-sdk/work/build/riscv-gnu-toolchain/riscv64-unknown-elf/prefix/bin
RISCV_GCC       := $(TOOL_DIR)/riscv64-unknown-elf-gcc
RISCV_GXX       := $(TOOL_DIR)/riscv64-unknown-elf-g++
RISCV_OBJDUMP   := $(TOOL_DIR)/riscv64-unknown-elf-objdump
RISCV_GDB       := $(TOOL_DIR)/riscv64-unknown-elf-gdb
RISCV_AR        := $(TOOL_DIR)/riscv64-unknown-elf-ar
RISCV_OPENOCD   := ~/freedom-e-sdk/work/build/openocd/prefix/bin/openocd

ASM_SRCS         = start.S
ASM_SRCS        += drivers/interrupt/entry.S
ASM_OBJS        := $(ASM_SRCS:.S=.o)

C_SRCS           = init.c
C_SRCS          += main.c
C_SRCS          += drivers/buffer.c
C_SRCS		+= drivers/interrupt/interrupt.c
C_SRCS          += drivers/interrupt/plic_driver.c
C_SRCS          += drivers/pll/pll_driver.c
C_SRCS          += drivers/uart/uart_driver.c
C_SRCS          += drivers/display/display_driver.c
C_SRCS          += drivers/syscall.c
C_OBJS          := $(C_SRCS:.c=.o)

LD_SCRIPT        = flash.lds

CFLAGS          += -O2 -fno-builtin-printf -g -march=$(RISCV_ARCH) -mabi=$(RISCV_ABI) -mcmodel=medany -Werror

INCLUDES        += -Iinclude/
INCLUDES        += -Idrivers/

SYS_SYMS        := malloc free \
	open lseek read write fstat stat close link unlink \
	execve fork getpid kill wait \
	isatty times sbrk exit

LDFLAGS         += $(foreach s,$(SYS_SYMS),-Wl,--wrap=$(s))
LDFLAGS         += -T $(LD_SCRIPT) -nostartfiles

.PHONY: all
all: $(TARGET)

OPENOCD_FLAGS   += -f openocd.cfg

GDB_PORT         = 3333

GDB_UPLOAD_FLAGS = --batch

GDB_UPLOAD_CMDS += -ex "set remotetimeout 240"
GDB_UPLOAD_CMDS += -ex "target extended-remote localhost:$(GDB_PORT)"
GDB_UPLOAD_CMDS += -ex "monitor reset halt"
GDB_UPLOAD_CMDS += -ex "monitor flash protect 0 64 last off"
GDB_UPLOAD_CMDS += -ex "load"
GDB_UPLOAD_CMDS += -ex "monitor resume"
GDB_UPLOAD_CMDS += -ex "monitor shutdown"
GDB_UPLOAD_CMDS += -ex "quit"

GDBCMDS         += -ex "set remotetimeout 240"
GDBCMDS         += -ex "target extended-remote localhost:$(GDB_PORT)"

dasm: $(TARGET)
	$(RISCV_OBJDUMP) -D $(TARGET)

program: $(TARGET)
	$(RISCV_OPENOCD) $(OPENOCD_FLAGS) & \
        $(RISCV_GDB) $(TARGET) $(GDB_UPLOAD_FLAGS) $(GDB_UPLOAD_CMDS) && \
        echo "Upload Successful"

openocd:
	$(RISCV_OPENOCD) $(OPENOCD_FLAGS)

debug: $(TARGET)
	$(RISCV_GDB) $(TARGET) $(GDB_FLAGS) $(GDBCMDS)

$(TARGET): $(ASM_OBJS) $(C_OBJS) $(LD_SCRIPT)
	$(RISCV_GCC) $(CFLAGS) $(INCLUDES) $(ASM_OBJS) $(C_OBJS) -o $@ $(LDFLAGS)

$(ASM_OBJS): %.o: %.S $(HEADERS)
	$(RISCV_GCC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

$(C_OBJS): %.o: %.c $(HEADERS)
	$(RISCV_GCC) $(CFLAGS) $(INCLUDES) -include sys/cdefs.h -c -o $@ $<

.PHONY: clean
clean:
	rm -f $(TARGET) $(ASM_OBJS) $(C_OBJS)
