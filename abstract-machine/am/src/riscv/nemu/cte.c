#include <am.h>
#include <riscv/riscv.h>
#include <klib.h>

static Context* (*user_handler)(Event, Context*) = NULL;

#define a7 c->gpr[17]
Context* __am_irq_handle(Context *c) {
//  for(int i=0;i<32;++i) printf("%d:%x %d\n",i,c->gpr[i],c->gpr[i]);
//  printf("%x %d\n%x %d\n%x %d\n",c->mcause,c->mcause,c->mstatus,c->mstatus,c->mepc,c->mepc);
  if (user_handler) {
    Event ev = {0};
//    printf("Error %2d,pc=%8x",c->mcause,c->mepc);
    switch (c->mcause) {
      case 11: 
        if(a7==-1) ev.event = EVENT_YIELD; else ev.event=EVENT_SYSCALL; break;
      default: ev.event = EVENT_ERROR; break;
    }

    c = user_handler(ev, c);
    if(ev.event==EVENT_YIELD) c->mepc+=4;
    assert(c != NULL);
  }

  return c;
}

extern void __am_asm_trap(void);

bool cte_init(Context*(*handler)(Event, Context*)) {
  // initialize exception entry
  asm volatile("csrw mtvec, %0" : : "r"(__am_asm_trap));

  // register event handler
  user_handler = handler;

  return true;
}

Context *kcontext(Area kstack, void (*entry)(void *), void *arg) {
  return NULL;
}

void yield() {
  asm volatile("li a7, -1; ecall");
}

bool ienabled() {
  return false;
}

void iset(bool enable) {
}
