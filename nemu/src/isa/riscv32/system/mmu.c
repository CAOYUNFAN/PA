#include <isa.h>
#include <memory/paddr.h>
#include <memory/vaddr.h>
#define satp cpu.sr[0x180]._32
extern riscv32_CPU_state cpu;
int isa_mmu_check(vaddr_t vaddr, int len, int type){
  enum { MMU_DIRECT, MMU_TRANSLATE, MMU_FAIL, MMU_DYNAMIC };
  assert((vaddr>>12)==((vaddr+len-1)>>12));
  if(((unsigned)satp>>31)==0) return MMU_DIRECT;
  else return MMU_TRANSLATE;
}
paddr_t isa_mmu_translate(vaddr_t vaddr, int len, int type) {
  assert((vaddr>>12)==((vaddr+len-1)>>12));
  vaddr_t high=vaddr>>22,low=(vaddr>>12)&0x3ff;
  printf("Memory:Virtual=%x,High=%x,Low=%x\n",vaddr,high,low);
  paddr_t pos1=paddr_read(((satp<<10)|high)<<2,4)&~0xfffu;
  paddr_t pos2=paddr_read(pos1|(low<<2),4)&~0xfffu;
  paddr_t ret=pos2|(vaddr&0xfffu);
  Assert(vaddr==ret,"virtual:%08x,physical:%08x",vaddr,ret);
  return ret;
}
