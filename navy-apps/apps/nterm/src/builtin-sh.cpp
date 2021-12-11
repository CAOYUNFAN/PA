#include <nterm.h>
#include <stdarg.h>
#include <unistd.h>
#include <SDL.h>

char handle_key(SDL_Event *ev);

static void sh_printf(const char *format, ...) {
  static char buf[256] = {};
  va_list ap;
  va_start(ap, format);
  int len = vsnprintf(buf, 256, format, ap);
  va_end(ap);
  term->write(buf, len);
}

static void sh_banner() {
  sh_printf("Built-in Shell in NTerm (NJU Terminal)\n\n");
}

static void sh_prompt() {
  sh_printf("sh> ");
}

static int cmd_echo(char * args){
  sh_printf("%s",args);
  return 0;
}

inline static void pp(char * arg1){
  for(int i=strlen(arg1)-1;i>=0;--i){
    if(arg1[i]==' '||arg1[i]=='\n'||arg1[i]=='\r') arg1[i]='\0';
    else return;
  }
}

static int cmd_export(char *args){
  char * arg=strtok(args," ");
  char * arg1=strtok(arg,"=");
  char * arg2=strtok(NULL,"=");
  pp(arg1);pp(arg2);
  setenv(arg1,arg2,0);
  return 0;
}

static struct {
  const char * name;
  int (*handler) (char *);
} cmd_table [] ={
  {"echo",cmd_echo},
  {"export",cmd_export}
};
#define NR_CMD sizeof(cmd_table)/sizeof(cmd_table[0])

static char temp[1000];

static char ** args(char * p){
  static char *ret[100];
  int i=0;
  for(;(ret[i]=strtok(p," "))!=NULL;++i) pp(ret[i]);
  ret[i]=NULL;
  return ret;
}

static void check(const char * filename,char * const argv[]){
  printf("%s\n",filename);
  for(int i=0;argv[i];++i) printf("%s\n",argv[i]);
}

static void sh_handle_cmd(const char *cmd) {
//  sh_printf("%s\n",cmd);
  strcpy(temp,cmd);
  if(cmd[0]=='/'){
    char * filename=strtok(temp," ");pp(filename);
    printf("AAAA\n");
    char * ndd=strtok(NULL," ");
    printf("BBBB\n");
    char ** my=args(ndd);
    check(filename,my);
    if(execv(filename,my)==-1) sh_printf("Program do not exist!\n");
    return;
  }
  int i;
  char * cmd_=strtok(temp," ");
  for(i=0;i<NR_CMD;++i)
  if(strcmp(cmd_,cmd_table[i].name)==0){
    if(cmd_table[i].handler(temp+strlen(cmd_)+1)<0) sh_printf("Something Wrong Seems to happen :-(\n");
    return;
  }
  char * ndd=strtok(NULL," ");
  char ** my=args(ndd);
  if(execvp(temp,my)==-1) sh_printf("Unkown or Not Handled Command '%s' :-(\n",cmd_);
  return;
}

void builtin_sh_run() {
  sh_banner();
  sh_prompt();

  while (1) {
    SDL_Event ev;
    if (SDL_PollEvent(&ev)) {
      if (ev.type == SDL_KEYUP || ev.type == SDL_KEYDOWN) {
        const char *res = term->keypress(handle_key(&ev));
        if (res) {
          sh_handle_cmd(res);
          sh_prompt();
        }
      }
    }
    refresh_terminal();
  }
}
