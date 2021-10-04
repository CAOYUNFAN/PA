#include <cpu/decode.h>
#include "../local-include/rtl.h"

#define INSTR_NULLARY(f) f(inv) f(nemu_trap)
#define INSTR_UNARY(f) 
#define INSTR_BINARY(f) f(lui) f(auipc) f(jal)
#define INSTR_TERNARY(f) f(lw) f(sw) f(addi) f(jalr) f(add) f(sltiu) f(beq) f(bne) f(sub) f(sltu) f(xor) f(or) f(sb) f(blt) f(bltu) f(bge) f(bgeu) f(srai) f(sra) f(lb) f(lh) f(lbu) f(lhu)

def_all_EXEC_ID();
