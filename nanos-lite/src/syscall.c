#include <common.h>
#include "syscall.h"

enum SYS_type{
  SYS_exit,
  SYS_yield,
  SYS_open,
  SYS_read,
  SYS_write,
  SYS_kill,
  SYS_getpid,
  SYS_close,
  SYS_lseek,
  SYS_brk,
  SYS_fstat,
  SYS_time,
  SYS_signal,
  SYS_execve,
  SYS_fork,
  SYS_link,
  SYS_unlink,
  SYS_wait,
  SYS_times,
  SYS_gettimeofday
};

void sys_exit(Context *c){
  halt(0);
}

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;
  extern void event_yield(Context *c);
  Log("System call %u from pc %x,with %12d:%8x,%12d:%8x,%12d:%8x",a[0],c->mepc,a[1],a[1],a[2],a[2],a[3],a[3]);
  switch (a[0]) {
    case SYS_yield: event_yield(c); break;
    case SYS_exit: sys_exit(c); break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
