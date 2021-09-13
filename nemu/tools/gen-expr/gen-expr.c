#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static char buf[65536] = {};
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";
int len=0;
uint32_t choose(uint32_t x){
	return (long long)rand()*rand()%x;
}

void gen_num(){
	len+=sprintf(buf+len,"%u",choose(100));
	return;
}

void gen(char ch){
	len+=sprintf(buf+len,"%c",ch);
	return;
}

void gen_rand_op(){
	char ch;
	switch(choose(4)){
		case 0:ch='+';break;
		case 1:ch='-';break;
		case 2:ch='*';break;
		default:ch='/';break;
	}
	len+=sprintf(buf+len,"%c",ch);
	return;
}

static void gen_rand_expr() {
	switch(choose(3)){
		case 0: gen_num();break;
		case 1: gen('('); gen_rand_expr();gen(')');break;
		default: gen_rand_expr(); gen_rand_op();gen_rand_expr();break;
	}
}

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop=20;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
	len=0;
    gen_rand_expr();

    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    fscanf(fp, "%d", &result);
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}
