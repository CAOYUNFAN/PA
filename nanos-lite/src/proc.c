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
    .start=(void *)pcb->stack,
    .end=(void *)(pcb->stack+STACK_SIZE)
  };
  pcb->cp=kcontext(temp,entry,args);
  return;
}

extern bool context_uload(PCB *pcb, const char *filename, char *const argv[], char *const envp[]);

#define FREQ 1000
void hello_fun(void *arg) {
  int j = 1;
  while (1) {
//    if(j%FREQ==0) 
//    Log("Hello World from Nanos-lite with arg '%s' for the %dth time!", arg, j);
    j ++;
    yield();
  }
}

void init_proc() {
  Log("Initializing processes...");

  context_kload(&pcb[0], hello_fun, "pcb_0");
  char * argv[]={NULL};
//  char * argv[]={NULL};
  char * envp[]={"PATH=/bin/:/usr/bin/",NULL};
  assert(context_uload(&pcb[1],"/bin/nemu",argv,envp));
  switch_boot_pcb();

  // load program here
//  extern void naive_uload(PCB *pcb, const char *filename);
//  naive_uload(NULL, "/bin/pal");
}

Context* schedule(Context *prev) {
  current->cp=prev;
  current=(current==&pcb[0]?&pcb[1]:&pcb[0]);
//  Log("Context entry=%p",current->cp);
  return current->cp;
}
