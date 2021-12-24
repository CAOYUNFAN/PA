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
    int temp;asm("csrr %0,satp":"=r"(temp));Log("%08x",temp);
    Log("Hello World from Nanos-lite with arg '%s' for the %dth time!", arg, j);
    j ++;
    yield();
  }
}

char main_name[]="/bin/nterm";
char * argv_for_main[]={NULL};
char * envp_for_main[]={"PATH=/bin/:/usr/bin/",NULL};

void init_proc() {
  Log("Initializing processes...");
  static char * argv[]={"--skip",NULL};
  context_kload(&pcb[0], hello_fun, "pcb_0");//Log("pcb0:%08x",&pcb[0]);
  assert(context_uload(&pcb[1],main_name,argv_for_main,envp_for_main));
  assert(context_uload(&pcb[2],"/bin/pal",argv,envp_for_main));
  assert(context_uload(&pcb[3],"/bin/bird",argv_for_main,envp_for_main));
  switch_boot_pcb();

  // load program here
//  extern void naive_uload(PCB *pcb, const char *filename);
//  naive_uload(NULL, "/bin/pal");
}
uintptr_t fg_pcb=1,cycle_num=0;

Context* schedule(Context *prev) {
  Log("Previous:%08x",current);
  current->cp=prev;
//  for(int i=0;i<4;++i) Log("%d:%08x,%08x,%08x",i,&pcb[i],pcb[i].cp->pdir,pcb[i].cp->np);
  if(!cycle_num) current=&pcb[0];
  else current=&pcb[fg_pcb];
  cycle_num=(cycle_num+1)&((1<<7)-1);
//  Log("Later:%08x",current);
  return current->cp;
}
