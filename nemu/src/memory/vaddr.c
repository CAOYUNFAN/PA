#include <isa.h>
#include <memory/paddr.h>

word_t vaddr_read(vaddr_t addr, int len) {
  #ifndef isa_mmu_chek
  switch(isa_mmu_check(addr,len,0)){
    case MMU_TRANSLATE:
      addr=isa_mmu_translate(addr,len,0);
    case MMU_DIRECT:
      return paddr_read(addr,len);
    default: assert(false);
  }
  #else
  return paddr_read(addr,len);
  #endif
}

void vaddr_write(vaddr_t addr, int len, word_t data) {
  #ifndef isa_mmu_chek
  switch(isa_mmu_check(addr,len,0)){
    case MMU_TRANSLATE:
      addr=isa_mmu_translate(addr,len,0);
    case MMU_DIRECT:
      paddr_write(addr,len,data);
      break;
    default: assert(false);
  }
  #else
  paddr_write(addr,len);
  #endif
}

word_t vaddr_ifetch(vaddr_t addr, int len) {
  return vaddr_read(addr, len);
}