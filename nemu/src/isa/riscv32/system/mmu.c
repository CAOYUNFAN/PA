#include <isa.h>
#include <memory/paddr.h>
#include <memory/vaddr.h>
extern riscv32_CPU_state cpu;
paddr_t isa_mmu_translate(vaddr_t vaddr, int len, int type) {
  assert((vaddr>>12)==((vaddr+len-1)>>12));
  vaddr_t high=vaddr>>22,low=(vaddr>>12)&0x3fu;
  paddr_t pos1=paddr_read(((satp<<10)|high)<<2,4)&~0xfffu;
  paddr_t pos2=paddr_read(pos1|(low<<2),4)&~0xfffu;
  return pos2|(vaddr&0xfffu);
}
