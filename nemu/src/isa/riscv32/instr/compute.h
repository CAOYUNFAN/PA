def_EHelper(lui) {
  rtl_li(s, ddest, id_src1->imm);
}
def_EHelper(auipc){
	rtl_addi(s,id_dest->preg,&s->pc,id_src1->imm);
}
def_EHelper(addi){
	rtl_addi(s,id_dest->preg,id_src1->preg,id_src2->imm);
}
def_EHelper(jal){
	rtl_mv(s,id_dest->preg,&s->snpc);
	rtl_j(s,id_src1->imm+s->pc);
}
def_EHelper(jalr){
	rtl_mv(s,id_dest->preg,&s->snpc);
	rtl_addi(s,t0,id_src1->preg,id_src2->imm);
	rtl_andi(s,t0,t0,0xFFFFFFFE);
	rtl_jr(s,t0);
}
def_EHelper(add){
	rtl_add(s,id_dest->preg,id_src1->preg,id_src2->preg);
}
def_EHelper(sltiu){
	rtl_setrelopi(s,RELOP_LTU,id_dest->preg,id_src1->preg,id_src2->imm);
}
/*def_EHelper(sw){
	rtl_sm(s,id_dest->preg,id_src1->preg,id_src2->imm,4);
}*/
