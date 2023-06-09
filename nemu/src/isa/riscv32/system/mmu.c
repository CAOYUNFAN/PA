#include <isa.h>
#include <memory/paddr.h>
#include <memory/vaddr.h>
#define satp cpu.sr[0x180]._32
extern riscv32_CPU_state cpu;
#ifndef isa_mmu_check
int isa_mmu_check(vaddr_t vaddr, int len, int type){
  enum { MMU_DIRECT, MMU_TRANSLATE, MMU_FAIL, MMU_DYNAMIC };
  if(((unsigned)satp>>31)==0) return MMU_DIRECT;
  else return MMU_TRANSLATE;
}
#endif
paddr_t isa_mmu_translate(vaddr_t vaddr, int len, int type) {
  Assert((vaddr>>12)==((vaddr+len-1)>>12),"%08x+%d=%08x",vaddr,len,vaddr+len);
  vaddr_t high=(unsigned)vaddr>>22,low=(vaddr>>12)&0x3ff;
//  if(vaddr<=0x80000000) 
//  Log("Memory:Virtual=%x,High=%x,Low=%x,satp=%x",vaddr,high,low,satp);
  paddr_t pos1=paddr_read(((satp<<10)|high)<<2,4)&~0xfffu;
  Assert(pos1>=0x80000000u,"SATP=%08x,POS1=%08x,vaddr=%08x",satp,pos1,vaddr);
  paddr_t pos2=paddr_read(pos1|(low<<2),4)&~0xfffu;
  paddr_t ret=pos2|(vaddr&0xfffu);
  Assert(pos2>=0x80000000u,"SATP=%08x,POS1=%08x,POS2=%08x,vaddr=%08x",satp,pos1,pos2,vaddr);
//  if(vaddr<=0x80000000) 
//  Log("virtual:%08x,pos1:%08x,delta=%08x,pos2:%08x,physical:%08x",vaddr,pos1,pos1|(low<<2),pos2,ret);
  return ret;
}
