def_EHelper(inv) {
  rtl_hostcall(s, HOSTCALL_INV, NULL, NULL, NULL, 0);
}

def_EHelper(nemu_trap) {
  rtl_hostcall(s, HOSTCALL_EXIT, NULL, &gpr(10), NULL, 0); // gpr(10) is $a0
}

#define Cao_get_reg(csr) (&cpu.sr[csr]._32)
#define MEPC cpu.sr[0x341]._32

def_EHelper(csrrw) {
  rtlreg_t *now=Cao_get_reg(id_src2->imm);
  rtl_mv(s,id_dest->preg,now);
  rtl_mv(s,now,id_src1->preg);
}

def_EHelper(csrrs) {
  rtlreg_t *now=Cao_get_reg(id_src2->imm);
  rtl_mv(s,id_dest->preg,now);
  rtl_or(s,now,now,id_src1->preg);
}

def_EHelper(mret){
  rtl_mv(s,&s->dnpc,&MEPC);
}

#define def_e_call_from_M_mode 11

def_EHelper(ecall) {
  rtl_li(s,&s->dnpc,isa_raise_intr(def_e_call_from_M_mode,s->pc));
}