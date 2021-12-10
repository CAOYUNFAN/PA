#include <common.h>

Context* schedule(Context *prev);

inline Context * event_yield(Context *c){
//  printf("EVENT_YIELD!\n");
  return schedule(c);
}

/*enum event_type{
  EVENT_NULL = 0,
  EVENT_YIELD, EVENT_SYSCALL, EVENT_PAGEFAULT, EVENT_ERROR,
  EVENT_IRQ_TIMER, EVENT_IRQ_IODEV,
};*/
extern void do_syscall(Context *c) ;

static Context* do_event(Event e, Context* c) {
  #ifdef CONFIG_ETRACE
  Log("Event ID=%2d",e.event);
  #endif
  switch (e.event) {
    case EVENT_YIELD: c=event_yield(c); break;
    case EVENT_SYSCALL: do_syscall(c); break;
    default: panic("Unhandled event ID = %d", e.event);
  }

  return c;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  cte_init(do_event);
}
