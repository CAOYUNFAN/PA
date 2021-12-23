#include <isa.h>

word_t isa_raise_intr(word_t NO, vaddr_t epc) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * Then return the address of the interrupt/exception vector.
   */
  cpu.sr[0x341]._32=epc;
  cpu.sr[0x342]._32=NO;
  rtlreg_t old=(cpu.sr[0x300]._32>>3)&1;
  cpu.sr[0x300]._32^=(cpu.sr[0x300]._32&((1<<7)|(1<<3)))^(old<<7);
  return cpu.sr[0x305]._32;
}

word_t isa_query_intr() {Log("%08x",cpu.sr[0x300]._32);
  if(cpu.INTR&&((cpu.sr[0x300]._32>>3)&1)){
    cpu.INTR = false;Log("Timer!");
    return IRQ_TIMER;
  }
  return INTR_EMPTY;
}
