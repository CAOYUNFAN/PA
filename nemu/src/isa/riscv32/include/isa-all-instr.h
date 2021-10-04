#include <cpu/decode.h>
#include "../local-include/rtl.h"

#define INSTR_LIST(f) f(inv) f(nemu_trap) f(lui) f(auipc) f(jal) f(mulu_lo) f(muls_hi) f(mulsu_hi) f(mulu_hi) f(divs_q) f(divu_q) f(divs_r) f(divu_r) f(lw) f(sw) f(addi) f(jalr) f(add) f(slt) f(beq) f(bne) f(sub) f(sltu) f(xor) f(or) f(sb) f(blt) f(bltu) f(bge) f(bgeu) f(srai) f(sra) f(lb) f(lh) f(lbu) f(lhu) f(andi) f(slti) f(xori) f(ori) f(slli) f(srli) f(srl) f(and) f(sltui) f(sll) f(sh) 

def_all_EXEC_ID();
