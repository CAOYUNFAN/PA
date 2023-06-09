#include <common.h>

void init_monitor(int, char *[]);
void am_init_monitor();
void engine_start();
int is_exit_status_bad();

#include <isa.h>
void check(){
	FILE *fp;
	fp=fopen("/home/cao/ics2021/nemu/tools/gen-expr/input","r");
	assert(fp!=NULL);
//	if(fp!=NULL) printf("OPEN SUCCEED!\n");
	extern word_t expr(char *e,bool *success);
	word_t x,cc=0;char st[10000],ch[10000];bool success=0;
  	while(fgets(ch,1000,fp)!=NULL){
		++cc;
//		printf("%s\n",ch);
		int i;
		x=0;
		for(i=0;ch[i]!=' ';++i) x=(x*10u)+ch[i]-'0';
//		printf("%u ",x);
		int j=0;
		for(i++;i<strlen(ch)-1;i++,j++) st[j]=ch[i];
		st[j]='\0';
		printf("%u:%s\n",cc,st);
		word_t temp=expr(st,&success);
//		printf("%u\n",temp);
		if(!success||x!=temp) assert(0);
	}
}

int main(int argc, char *argv[]) {
  /* Initialize the monitor. */
#ifdef CONFIG_TARGET_AM
  am_init_monitor();
#else
  init_monitor(argc, argv);
#endif
//	check();

  /* Start engine. */
  engine_start();

  return is_exit_status_bad();
}
