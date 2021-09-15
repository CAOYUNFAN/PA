#include <common.h>

void init_monitor(int, char *[]);
void am_init_monitor();
void engine_start();
int is_exit_status_bad();

#include <isa.h>
void check(){
	FILE *fp;
	fp=fopen("../../tools/gen-expr/input","r");
	assert(fp==NULL);
	extern word_t expr(char *e,bool *success);
	word_t x,i=0;char *st="";bool success=0;
	while(fscanf(fp,"%u%s",&x,st)!=EOF){
		printf("%d\n",++i);
		assert(x==expr(st,&success));
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
