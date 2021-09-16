#include <common.h>

void init_monitor(int, char *[]);
void am_init_monitor();
void engine_start();
int is_exit_status_bad();

#include <isa.h>
void check(){
	FILE *fp;
	fp=fopen("/home/cao/ics2021/nemu/tools/gen-expr/input","r");
//	assert(fp!=NULL);
	if(fp!=NULL) printf("OPEN SUCCEED!\n");
	extern word_t expr(char *e,bool *success);
	word_t x,i=0;char st[10000],ch[10000];bool success=0;
 	while(fgets(ch,1000,fp)!=NULL){
		printf("%s\n",ch);
		sscanf(ch,"%u %s",&x,st);
		printf("%u %s ",++i,st);
		word_t temp=expr(st,&success);
		printf("%u\n",temp);
		if(!success||x!=temp) assert(0);
	}
}

int main(int argc, char *argv[]) {
	check();
  /* Initialize the monitor. */
#ifdef CONFIG_TARGET_AM
  am_init_monitor();
#else
  init_monitor(argc, argv);
#endif

  /* Start engine. */
  engine_start();

  return is_exit_status_bad();
}
