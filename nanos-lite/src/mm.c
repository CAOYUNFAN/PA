#include <memory.h>
#include <proc.h>

static char *pf = NULL;

void* new_page(size_t nr_page) {
  if(!pf) pf=heap.end;
  void * ret=pf;
  pf=(void *)((char *)pf+nr_page*PGSIZE);
  return ret;
}

#ifdef HAS_VME
static void* pg_alloc(int n) {
  void * ret=new_page(n/PGSIZE);
  memset(ret,0,n);
  return ret;
}
#endif

void free_page(void *p) {
  return;
  //panic("not implement yet");
}

/* The brk() system call handler. */
int mm_brk(uintptr_t brk) {//Log("USED HERE!");
  if(brk>current->max_brk){
    for(uintptr_t i=(current->max_brk&~0xfffu)+0x1000;i<=brk;++i)
      map(&current->as,(void *)i,pg_alloc(1),0);
    current->max_brk=brk;
  }
  return 0;
}

void init_mm() {
  pf = (void *)ROUNDUP(heap.start, PGSIZE);
  Log("free physical pages starting from %p", pf);

#ifdef HAS_VME
  vme_init(pg_alloc, free_page);
#endif
}
