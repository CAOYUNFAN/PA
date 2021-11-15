#include <isa.h>
#include "local-include/reg.h"

const char *regs[] = {
  "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

void isa_reg_display() {
	for(int i=0;i<32;++i)
	printf("%s\t0x%x\t%d\n",regs[i],gpr(i),gpr(i));
	printf("pc\t0x%x\t%d\n",cpu.pc,cpu.pc);
	printf("mcause\t0x%x\t%d\n",sr(0x342),sr(0x342));
	printf("mstatus\t0x%x\t%d\n",sr(0x300),sr(0x300));
	printf("mepc\t0x%x\t%d\n",sr(0x341),sr(0x341));
	return;
}

word_t isa_reg_str2val(const char *s, bool *success) {
//	printf("%s\n",s+1);
	for(int i=0;i<32;++i){
//		printf("%s ",regs[i]);
		if(strcmp(s+1,regs[i])==0){
			*success=true;
			return gpr(i);
		}
//		printf("%d\n",strcmp(s+1,regs[i]));
	}
	if(strcmp(s+1,"pc")==0){
		*success=true;
		return cpu.pc;
	}
	*success=false;
	return 0;
}
