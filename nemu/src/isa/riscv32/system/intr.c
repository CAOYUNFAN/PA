#include <isa.h>
int cyf_M=0;
word_t isa_raise_intr(word_t NO, vaddr_t epc) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * Then return the address of the interrupt/exception vector.
   */
  cyf_M=0;
  cpu.sr[0x341]._32=epc;
  cpu.sr[0x342]._32=NO;//Log("ERRERNO:%08x",NO);
  rtlreg_t old=(cpu.sr[0x300]._32>>3)&1;
  cpu.sr[0x300]._32^=(cpu.sr[0x300]._32&((1<<7)|(1<<3)))^(old<<7);
  return cpu.sr[0x305]._32;
}

word_t isa_query_intr() {//Log("%08x",cpu.sr[0x300]._32);//&&((cpu.sr[0x304]._32>>7)&1)&&((cpu.sr[0x344]._32>>7)&1)
  if(cpu.INTR&&((cpu.sr[0x300]._32>>3)&1)&&cyf_M){
    cpu.INTR = false;//Log("%08x",IRQ_TIMER);
    return IRQ_TIMER;
  }
  return INTR_EMPTY;
}
