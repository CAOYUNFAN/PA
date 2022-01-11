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
//  Log("pcb->cp:%08x,pcb->cp->sp:%08x",pcb->cp,pcb->cp->gpr[2]);
  return;
}

extern bool context_uload(PCB *pcb, const char *filename, char *const argv[], char *const envp[]);

#define FREQ 1000
void hello_fun(void *arg) {
  int j = 1;
  while (1) {
//    if(j%FREQ==0) 
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
  context_kload(&pcb[0], hello_fun, "pcb_0");
  assert(context_uload(&pcb[1],"/bin/nterm",argv_for_main,envp_for_main));
  assert(context_uload(&pcb[2],"/bin/pal",argv,envp_for_main));
  assert(context_uload(&pcb[3],"/bin/bird",argv_for_main,envp_for_main));
  switch_boot_pcb();
//  Log("pcb0:%08x,pcb1:%08x,pcb2:%08x,pcb3:%08x",&pcb[0],&pcb[1],&pcb[2],&pcb[3]);

  // load program here
//  extern void naive_uload(PCB *pcb, const char *filename);
//  naive_uload(NULL, "/bin/pal");
}
uintptr_t fg_pcb=1,force_yield=0;

Context* schedule(Context *prev) {
  #ifdef cyf_DBG
  bool flag;
  if(!cycle_num||current!=&pcb[fg_pcb]) flag=1;else flag=0;
  if(flag)Log("Previous:%08x",current);
  #endif

  current->cp=prev;
//  static int xx=0;++xx;
  static int cycle_num=0;
  #ifdef cyf_DBG
  if(flag) for(int i=0;i<4;++i) Log("%d:%08x,%08x,%08x",i,&pcb[i],pcb[i].cp->pdir,pcb[i].cp->np);
  #endif
  assert(fg_pcb>0&&fg_pcb<4);
  Log("current:%08x->%08x,prev=%08x",current,(uintptr_t)current+sizeof(PCB),prev);
  assert((uintptr_t)current<=(uintptr_t)prev&&(uintptr_t)current+sizeof(PCB)>=(uintptr_t)prev+sizeof(Context));
  if(current!=&pcb[fg_pcb]) current=&pcb[fg_pcb];
  else if(force_yield||!cycle_num){
    current=&pcb[fg_pcb];
    cycle_num=force_yield=0;
  }else cycle_num=(cycle_num+1)&((1<<7)-1);

  #ifdef cyf_DBG
  if(flag) Log("Later:%08x",current);
  #endif

//  if(xx<=2) 
//  Log("current%08x,%08x,%d",current,&pcb[0],current-&pcb[0]);
  return current->cp;
}
