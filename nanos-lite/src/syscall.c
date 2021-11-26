#include <common.h>
#include "syscall.h"

/*enum SYS_type{
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
};*/

extern int fs_open(const char *pathname, int flags, int mode);
extern size_t fs_read(int fd, void *buf, size_t len);
extern size_t fs_write(int fd, const void *buf, size_t len);
extern size_t fs_lseek(int fd, size_t offset, int whence);
extern int fs_close(int fd);

void sys_exit(){
  halt(0);
}

int sys_brk(void *addr){
  return 0;
}

/*int sys_gettimeofday(timeval * tv,timezone * tz){
  panic("Not implemented!");
  return 0;
}*/

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;
  extern void event_yield(Context *c);
  #ifdef CONFIG_STRACE
  Log("System call %u with %12d:%8x,%12d:%8x,%12d:%8x",a[0],a[1],a[1],a[2],a[2],a[3],a[3]);
  #endif
  switch (a[0]) {
    case SYS_yield: event_yield(c); break;
    case SYS_exit: sys_exit(); break;
    case SYS_write: c->GPRx=fs_write(a[1],(unsigned char *)a[2],a[3]); break;
    case SYS_brk: c->GPRx=sys_brk((void *)a[1]); break;
    case SYS_open: c->GPRx=fs_open((void *)a[1],a[2],a[3]); break;
    case SYS_close: c->GPRx=fs_close(a[1]); break;
    case SYS_read: c->GPRx=fs_read(a[1],(void *)a[2],a[3]); break;
    case SYS_lseek: c->GPRx=fs_lseek(a[1],a[2],a[3]); break;
  //  case SYS_gettimeofday: c->GPRx=sys_gettimeofday((timeval *)a[1],(timezone *)a[2]);
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}

/*
int sys_write(int fd,const void *buf,size_t count){
  int i=0;
  const unsigned char * ch=buf;
  if(fd==1||fd==2) for(;i<count;++i,++ch) putch(*ch);
  else return fs_write(fd,buf,count);
  return i;
}

int sys_read(int fd,void *buf,size_t count){
  if(fd) return fs_read(fd,buf,count);
  else panic("STDIN not implemented!");
  return -1;
}
*/