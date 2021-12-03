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

static struct {
  const char * name;
  int (*handler) (char *);
} cmd_table [] ={
  {"echo",cmd_echo}
};
#define NR_CMD sizeof(cmd_table)/sizeof(cmd_table[0])

static char temp[1000];
static void sh_handle_cmd(const char *cmd) {
//  sh_printf("%s\n",cmd);
  if(cmd[0]=='/'){
    strcpy(temp,cmd);int n=strlen(temp);
    while(temp[n-1]==' '||temp[n-1]=='\n'||temp[n-1]=='\r') temp[--n]='\0';
//    printf("111%s111\n%d\n",temp,strlen(temp));
    execv(temp,NULL);
    return;
  }
  strcpy(temp,cmd);
  int i;
  char * cmd_=strtok(temp," ");
  for(i=0;i<NR_CMD;++i)
  if(strcmp(cmd_,cmd_table[i].name)==0){
    if(cmd_table[i].handler(temp+strlen(cmd_)+1)<0) sh_printf("Something Wrong Seems to happen :-(\n");
    return;
  }
  sh_printf("Unkown or Not Handled Command '%s' :-(\n",cmd_);
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
