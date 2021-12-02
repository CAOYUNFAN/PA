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
  sh_printf("%s\n",args);
  return 0;
}

static struct {
  const char * name;
  int (*handler) (char *);
} cmd_table [] ={
  {"echo",cmd_echo}
}
#define NR_CMD sizeof(cmd_table)/sizeof(cmd_table[0])

static void sh_handle_cmd(const char *cmd) {
  int i;
  char * cmd_=strtok(cmd," ");
  for(i=0;i<NR_CMD;++i)
  if(strcmp(cmd_,cmd_table[i].name)==0){
    if(cmd_table[i].handler(cmd+strlen(cmd_)+1)<0) sh_printf("Something Wrong Seems to happen :-(\n");
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
