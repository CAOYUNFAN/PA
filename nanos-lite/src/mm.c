#include <memory.h>
#include <proc.h>

static char *pf = NULL;

void* new_page(size_t nr_page) {
  if(!pf) pf=heap.end;
  void * ret=pf;
  pf=(void *)((char *)pf+nr_page*4096);
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
#define CAO_DEBUG
/* The brk() system call handler. */
extern uintptr_t check_map(AddrSpace *as,void * va);
int mm_brk(intptr_t increament,uintptr_t brk) {
  Log("USED HERE! %08x=%08x+%08x, %08x",brk+increament,brk,increament,current->max_brk);
#ifdef HAS_VME
  if(!current->max_brk){Log("%d,%08x",increament,brk);
    current->max_brk=(brk&~0xfff)-1;
  }
  brk+=increament;
  if(brk>current->max_brk){
    for(uintptr_t i=(current->max_brk&~0xfffu)+0x1000;i<brk;i+=0x1000)
    if(!check_map(&current->as,(void *)i)){
#ifdef CAO_DEBUG
      void * temp;
      map(&current->as,(void *)i,temp=new_page(1),0);
      Log("Virtual %08x to Physical %08x",i,temp);
#else
      map(&current->as,(void *)i,new_page(1),0);
#endif
    }
    current->max_brk=brk;
  }
#endif
  return 0;
}

void init_mm() {
  pf = (void *)ROUNDUP(heap.start, PGSIZE);
  Log("free physical pages starting from %p", pf);

#ifdef HAS_VME
  vme_init(pg_alloc, free_page);
#endif
}
