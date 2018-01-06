// See LICENSE for license details.

/* This is an incomplete version of a syscall library, 
 * which really only supports simple reads and writes over UART.
 */

#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <stdio.h>
#include <string.h>

#include "platform.h"
#include "uart/uart_driver.h"

static int _stub(int err) {
  return -1;
}

int __wrap_isatty(int fd) {
  if (fd == STDOUT_FILENO || fd == STDERR_FILENO)
    return 1;

  return 0;
}

ssize_t __wrap_write(int fd, const void* ptr, size_t len) {


  const uint8_t * current = (const char *)ptr;

  if (__wrap_isatty(fd)) {
    return uart_write(fd, ptr, len);
  } 

  return _stub(EBADF);
}

void write_hex(int fd, uint32_t hex) {
  uint8_t ii;
  uint8_t jj;
  char towrite;
  __wrap_write(fd , "0x", 2);
  for (ii = 8 ; ii > 0; ii--) {
    jj = ii - 1;
    uint8_t digit = ((hex & (0xF << (jj*4))) >> (jj*4));
    towrite = digit < 0xA ? ('0' + digit) : ('A' +  (digit - 0xA));
    __wrap_write(fd, &towrite, 1);
  }
}

void __wrap_exit(int code) {
  //volatile uint32_t* leds = (uint32_t*) (GPIO_BASE_ADDR + GPIO_OUT_OFFSET);
  const char * message = "\nProgam has exited with code:";
  
  //*leds = (~(code));

  __wrap_write(STDERR_FILENO, message, strlen(message));
  write_hex(STDERR_FILENO, code);
  __wrap_write(STDERR_FILENO, "\n", 1);

  while (1) ;
}

void *__wrap_sbrk(ptrdiff_t incr) {
  extern char _end[];
  extern char _heap_end[];
  static char *curbrk = _end;

  if ((curbrk + incr < _end) || (curbrk + incr > _heap_end))
    return NULL - 1;

  curbrk += incr;
  return curbrk - incr;
}

int __wrap_open(const char* name, int flags, int mode) {
  return _stub(ENOENT);
}

int __wrap_openat(int dirfd, const char* name, int flags, int mode) {
  return _stub(ENOENT);
}

int __wrap_close(int fd) {
  return _stub(EBADF);
}

int __wrap_execve(const char* name, char* const argv[], char* const env[]) {
  return _stub(ENOMEM);
}

int __wrap_fork() {
  return _stub(EAGAIN);
}

int __wrap_fstat(int fd, struct stat *st) {
  if (__wrap_isatty(fd)) {
    st->st_mode = S_IFCHR;
    return 0;
  }

  return _stub(EBADF);
}

int __wrap_getpid() {
  return 1;
}

int __wrap_kill(int pid, int sig) {
  return _stub(EINVAL);
}

int __wrap_link(const char *old_name, const char *new_name) {
  return _stub(EMLINK);
}

off_t __wrap_lseek(int fd, off_t ptr, int dir) {
  if (__wrap_isatty(fd))
    return 0;

  return _stub(EBADF);
}

ssize_t __wrap_read(int fd, void* ptr, size_t len) {
  
  if (__wrap_isatty(fd)) {
    return uart_read(fd, ptr, len);
  }

  return _stub(EBADF);
}

int __wrap_stat(const char* file, struct stat* st) {
  return _stub(EACCES);
}

clock_t __wrap_times(struct tms* buf) {
  return _stub(EACCES);
}

int __wrap_unlink(const char* name) {
  return _stub(ENOENT);
}

int __wrap_wait(int* status) {
  return _stub(ECHILD);
}


