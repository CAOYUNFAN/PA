def_EHelper(lui) {
  rtl_li(s, ddest, id_src1->imm);
}

def_EHelper(auipc){
	rtl_addi(s,id_dest->preg,&s->pc,id_src1->imm);
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

//imm&&R

#define def_CYF_REG(name) def_EHelper(name){\
	concat(rtl_,name)(s,id_dest->preg,id_src1->preg,id_src2->preg);\
}

#define def_CYF_IMM(name) def_EHelper(name){\
	concat(rtl_,name)(s,id_dest->preg,id_src1->preg,id_src2->imm);\
}

#define def_CYF_ALL(name) def_CYF_REG(name)\
	def_CYF_IMM(concat(name,i))

def_CYF_ALL(add)
def_CYF_REG(sub)//Attention! I-type do not has the instr subi.
def_CYF_ALL(xor)
def_CYF_ALL(or)
def_CYF_ALL(sll)
def_CYF_ALL(srl)
def_CYF_ALL(sra)
def_CYF_ALL(and)

def_CYF_REG(mulu_lo)
def_CYF_REG(muls_hi)
def_CYF_REG(mulu_hi)
def_CYF_REG(mulsu_hi)
def_CYF_REG(divs_q)
def_CYF_REG(divs_r)
def_CYF_REG(divu_q)
def_CYF_REG(divu_r)

#define def_CYF_REG2(name,type) def_EHelper(name){\
	rtl_setrelop(s,concat(RELOP_,type),id_dest->preg,id_src1->preg,id_src2->preg);\
}

#define def_CYF_IMM2(name,type) def_EHelper(name){\
	rtl_setrelopi(s,concat(RELOP_,type),id_dest->preg,id_src1->preg,id_src2->imm);\
}

#define def_CYF_ALL2(name,type) def_CYF_REG2(name,type)\
	def_CYF_IMM2(concat(name,i),type)

def_CYF_ALL2(slt,LT)
def_CYF_ALL2(sltu,LTU)

//B

#define def_CYF_B(name,type) def_EHelper(name){\
	rtl_jrelop(s,concat(RELOP_,type),id_src1->preg,id_src2->preg,s->pc+id_dest->imm);\
}

def_CYF_B(beq,EQ)
def_CYF_B(bne,NE)
def_CYF_B(blt,LT)
def_CYF_B(bge,GE)
def_CYF_B(bltu,LTU)
def_CYF_B(bgeu,GEU)

//Codes below are replaced by macro. Not deleted.
/*def_EHelper(xori){
	rtl_xori(s,id_dest->preg,id_src1->preg,id_src2->imm);
}
def_EHelper(ori){
	rtl_ori(s,id_dest->preg,id_src1->preg,id_src2->imm);
}
def_EHelper(andi){
	rtl_andi(s,id_dest->preg,id_src1->preg,id_src2->imm);
}
def_EHelper(slli){
	rtl_slli(s,id_dest->preg,id_src1->preg,id_src2->imm);
}
def_EHelper(srli){
	rtl_srli(s,id_dest->preg,id_src1->preg,id_src2->imm);
}*/
/*def_EHelper(addi){
	rtl_addi(s,id_dest->preg,id_src1->preg,id_src2->imm);
}*/
/*def_EHelper(add){
	rtl_add(s,id_dest->preg,id_src1->preg,id_src2->preg);
}
def_EHelper(sub){
	rtl_sub(s,id_dest->preg,id_src1->preg,id_src2->preg);
}*/
/*def_EHelper(xor){
	rtl_xor(s,id_dest->preg,id_src1->preg,id_src2->preg);
}
def_EHelper(or){
	rtl_or(s,id_dest->preg,id_src1->preg,id_src2->preg);
}
def_EHelper(srai){
	rtl_srai(s,id_dest->preg,id_src1->preg,id_src2->imm);
}
def_EHelper(sra){
	rtl_sra(s,id_dest->preg,id_src1->preg,id_src2->preg);
}*/
/*def_EHelper(sltiu){
	rtl_setrelopi(s,RELOP_LTU,id_dest->preg,id_src1->preg,id_src2->imm);
}*/
/*def_EHelper(sltu){
	rtl_setrelop(s,RELOP_LTU,id_dest->preg,id_src1->preg,id_src2->preg);
}*/
/*def_EHelper(slti){
	rtl_setrelopi(s,RELOP_LT,id_dest->preg,id_src1->preg,id_src2->imm);
}*/
/*def_EHelper(beq){
	rtl_jrelop(s,RELOP_EQ,id_src1->preg,id_src2->preg,s->pc+id_dest->imm);
}
def_EHelper(bne){
	rtl_jrelop(s,RELOP_NE,id_src1->preg,id_src2->preg,s->pc+id_dest->imm);
}
def_EHelper(blt){
	rtl_jrelop(s,RELOP_LT,id_src1->preg,id_src2->preg,s->pc+id_dest->imm);
}
def_EHelper(bge){
	rtl_jrelop(s,RELOP_GE,id_src1->preg,id_src2->preg,s->pc+id_dest->imm);
}
def_EHelper(bltu){
	rtl_jrelop(s,RELOP_LTU,id_src1->preg,id_src2->preg,s->pc+id_dest->imm);
}
def_EHelper(bgeu){
	rtl_jrelop(s,RELOP_GEU,id_src1->preg,id_src2->preg,s->pc+id_dest->imm);
}*/

