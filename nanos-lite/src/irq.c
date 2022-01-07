#include <common.h>

Context* schedule(Context *prev);
extern uintptr_t force_yield; 
inline Context * event_yield(Context *c){
//  Log("EVENT_YIELD!");
  force_yield=1;
  return schedule(c);
}

inline Context * event_irq_timer(Context * c){
//  Log("event_irq_timer!");
  return schedule(c);
}

/*enum event_type{
  EVENT_NULL = 0,
  EVENT_YIELD, EVENT_SYSCALL, EVENT_PAGEFAULT, EVENT_ERROR,
  EVENT_IRQ_TIMER, EVENT_IRQ_IODEV,
};*/
extern Context * do_syscall(Context *c) ;

static Context* do_event(Event e, Context* c) {
//  Log("Before:%08x,%08x",c,c->gpr[2]);
  #ifdef CONFIG_ETRACE
  Log("Event ID=%2d",e.event);
  #endif
  switch (e.event) {
    case EVENT_YIELD: c=event_yield(c); break;
    case EVENT_SYSCALL: c=do_syscall(c); break;
    case EVENT_IRQ_TIMER: c=event_irq_timer(c); break;
    default: panic("Unhandled event ID = %d", e.event);
  }
//  Log("After:%08x,%08x",c,c->gpr[2]);
  return c;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  cte_init(do_event);
}
