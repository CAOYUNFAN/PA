#include <isa.h>

word_t isa_raise_intr(word_t NO, vaddr_t epc) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * Then return the address of the interrupt/exception vector.
   */
  cpu.sr[0x341]._32=epc;
  cpu.sr[0x342]._32=NO;
  return cpu.sr[0x305]._32;
}

word_t isa_query_intr() {
  return INTR_EMPTY;
}
