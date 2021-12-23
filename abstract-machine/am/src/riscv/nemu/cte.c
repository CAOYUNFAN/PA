#include <am.h>
#include <riscv/riscv.h>
#include <klib.h>

static Context* (*user_handler)(Event, Context*) = NULL;

#define a7 c->gpr[17]
extern void __am_get_cur_as(Context *c);
extern void __am_switch(Context *c);
Context* __am_irq_handle(Context *c) {
  if(c->pdir!=NULL) __am_get_cur_as(c);
  printf("From __am_:%08x,%08x",c,c->pdir);
  if (user_handler) {
    Event ev = {0};
   
    switch (c->mcause) {
      case 11: 
        if(a7==-1) ev.event = EVENT_YIELD; else ev.event=EVENT_SYSCALL; break;
      case 0x80000007:
        ev.event=EVENT_IRQ_TIMER; break;
      default: ev.event = EVENT_ERROR; break;
    }

    if(ev.event==EVENT_YIELD||ev.event==EVENT_SYSCALL) c->mepc+=4;
    c = user_handler(ev, c);
    assert(c != NULL);
  }
  __am_switch(c);
  return c;
}

extern void __am_asm_trap(void);

bool cte_init(Context*(*handler)(Event, Context*)) {
  // initialize exception entry
  asm volatile("csrw mtvec, %0" : : "r"(__am_asm_trap));
  asm volatile("csrw mscratch, zero");
  // register event handler
  user_handler = handler;//  printf("%p\n",handler);
  return true;
}

Context *kcontext(Area kstack, void (*entry)(void *), void *arg) {
  Context * ret=(Context *)((char *)kstack.end-sizeof(Context));
  memset(ret,0,sizeof(Context));
//  printf("Context=%p,pc=%x\n",ret,entry);
  ret->GPR2=(uintptr_t)arg;
  ret->mstatus=0x1808;
  ret->mepc=(uintptr_t)entry;
  ret->mcause=11;
  ret->pdir=NULL;
  ret->np=0;
  return ret;
}

void yield() {
  asm volatile("li a7, -1; ecall");
}

bool ienabled() {
  return false;
}

void iset(bool enable) {
}

/*  if((uintptr_t)user_handler!=0x8000118c){
      for(int i=0;i<32;++i) printf("%d:%x %d\n",i,c->gpr[i],c->gpr[i]);
      printf("%x %d\n%x %d\n%x %d\n",c->mcause,c->mcause,c->mstatus,c->mstatus,c->mepc,c->mepc);
      printf("Error %2d,pc=%8x\n",c->mcause,c->mepc);
      printf("%p %p\n",&user_handler,user_handler);
    }*/
 