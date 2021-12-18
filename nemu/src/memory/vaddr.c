#include <isa.h>
#include <memory/paddr.h>

word_t vaddr_ifetch(vaddr_t addr, int len) {
  return paddr_read(addr, len);
}

word_t vaddr_read(vaddr_t addr, int len) {
  switch(isa_mmu_check(addr,len,0)){
    case MMU_TRANSLATE:
      addr=isa_mmu_translate(addr,len,0);
    case MMU_DIRECT:
      return paddr_read(addr,len);
    default: assert(false);
  }
}

void vaddr_write(vaddr_t addr, int len, word_t data) {
  printf("VADDR_write:write to virtual address%8x,len%8d,data%8x,",addr,len,data);
  switch(isa_mmu_check(addr,len,0)){
    case MMU_TRANSLATE:
      addr=isa_mmu_translate(addr,len,0);printf("VME!");
    case MMU_DIRECT:
      printf("physical address:%8x\n",addr);
      paddr_write(addr,len,data);
      break;
    default: assert(false);
  }
}
