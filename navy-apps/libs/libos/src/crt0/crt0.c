#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
//#include <stdio.h>

int main(int argc, char *argv[], char *envp[]);
extern char **environ;
void call_main(uintptr_t *args) {
//  printf("%p\n",args);
  int argc=(int)*args;
  char ** argv=(char **)(args+1);
  char ** envp=(char **)(args+argc+2);
  environ = envp;
  exit(main(argc, argv, envp));
/*  environ=NULL;
  exit(main(0,NULL,NULL));
  assert(0);*/
}
