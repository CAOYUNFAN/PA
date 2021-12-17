#include <memory.h>

static char *pf = NULL;

void* new_page(size_t nr_page) {
  if(!pf) pf=heap.end;
  return (pf=(void *)((char *)pf-nr_page*PGSIZE));
}

#ifdef HAS_VME
static void* pg_alloc(int n) {
  void * ret=new_page(n/PGSIZE);
  ret=(void *)((char *)ret-n*PGSIZE);
  memset(ret,0,n*PGSIZE);
  return ret;
}
#endif

void free_page(void *p) {
  return;
  //panic("not implement yet");
}

/* The brk() system call handler. */
int mm_brk(uintptr_t brk) {
  return 0;
}

void init_mm() {
  pf = (void *)ROUNDUP(heap.start, PGSIZE);
  Log("free physical pages starting from %p", pf);

#ifdef HAS_VME
  vme_init(pg_alloc, free_page);
#endif
}
