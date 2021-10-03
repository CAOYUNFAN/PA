def_EHelper(lui) {
  rtl_li(s, ddest, id_src1->imm);
}
def_EHelper(auipc){
	rtl_addi(s,id_dest->preg,&s->snpc,id_src1->imm);
}
def_EHelper(addi){
	rtl_addi(s,id_dest->preg,id_src1->preg,id_src2->imm);
}
def_EHelper(jal){
	rtl_mv(s,id_dest->preg,&s->snpc);
	rtl_j(s,id_src1->imm);
}
/*def_EHelper(sw){
	rtl_sm(s,id_dest->preg,id_src1->preg,id_src2->imm,4);
}*/
