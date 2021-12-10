#include <proc.h>

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;

void switch_boot_pcb() {
  current = &pcb_boot;
}

/*typedef struct{
  void * start, *end;
}Area;*/

void context_kload(PCB * pcb,void * entry,void * args){
  Area temp={
    .start=(void *)pcb,
    .end=(void *)((char *)pcb+sizeof(pcb))
  };
  pcb->cp=kcontext(temp,entry,args);
  return;
}

void hello_fun(void *arg) {
  int j = 1;
  while (1) {
    Log("Hello World from Nanos-lite with arg '%s' for the %dth time!", arg, j);
    j ++;
    yield();
  }
}

void init_proc() {
  context_kload(&pcb[0], hello_fun, "pcb_0");
  context_kload(&pcb[1], hello_fun, "pcb_1");
  switch_boot_pcb();

  Log("Initializing processes...");

  // load program here
  extern void naive_uload(PCB *pcb, const char *filename);
//  naive_uload(NULL, "/bin/nterm");
}

Context* schedule(Context *prev) {
  current->cp=prev;
  current=(current==&pcb[0]?&pcb[1]:&pcb[0]);
//  Log("Context entry=%p",current->cp);
  return current->cp;
}
