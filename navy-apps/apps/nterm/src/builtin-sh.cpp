#include <nterm.h>
#include <stdarg.h>
#include <unistd.h>
#include <SDL.h>
#include <string.h>

char handle_key(SDL_Event *ev);

static char *strccpy (char *s1,const char *s2,char c){
  char *dest = s1;
  while (*s2 && *s2 != c)
    *s1++ = *s2++;
  *s1 = 0;
  return dest;
}


#define NR_CMD sizeof(cmd_table)/sizeof(cmd_table[0])
#define buf_len 1024
#define buf_name 128
//#define DEBUG

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

inline static void clean(char * str){
  for(int i=strlen(str)-1;i>=0;--i){
    if(str[i]==' '||str[i]=='\n'||str[i]=='\r') str[i]='\0';
    else return;
  }
}

static int cmd_echo(char * const args[]){
  static char empty[]={'\0'};
  bool first=1;
  for(;*args;++args){
    if(first) first=0;
    else sh_printf(" ");
    if(**args=='$'){
      char * ch=getenv((*args)+1);
      if (!ch) ch=empty;
      sh_printf("%s",ch);
    }else sh_printf("%s",*args);
  }
  sh_printf("\n");
  return 0;
}

static int cmd_export(char * const args[]){
  static char env[buf_len],data[buf_len];
  int ret=0;
  for(;*args;++args){
    const char * ch=*args;
    strccpy(env,ch,'=');
    while(*ch&&*ch!='=')++ch;
    while(*ch=='=')++ch;
    strccpy(data,ch,'=');
    if(strlen(data)==0) sh_printf("syntax error with %s",env),ret=1;
    else setenv(env,data,1);
    #ifdef CONFIG_DEBUG
    printf("%s",getenv(env));
    #endif
  }
  return ret;
}

static const struct {
  const char * name;
  int (*handler) (char * const *);
} cmd_table [] ={
  {"echo",cmd_echo},
  {"export",cmd_export}
};

static char ** args(const char * arg){
  static char * ret[buf_name];
  static char temp[buf_len];
  while(*arg==' '||*arg=='\n') ++arg;
  strccpy(temp,arg,' ');
  int tot=0;
  while(*temp!=0){
    clean(temp);
    ret[tot]=(char *)malloc(strlen(temp)+1);
    strcpy(ret[tot++],temp);
    while(*arg&&*arg!=' ') ++arg;
    while(*arg==' '||*arg=='\n') ++arg;
    strccpy(temp,arg,' ');
  }
  ret[tot]=NULL;
  return ret;
}

static void free_args(char ** f){
  for(;*f;++f) free(*f);
  return;
}

#ifdef DEBUG
static void check(const char * filename,char * const argv[]){
  printf("%s!NAME\n",filename);
  for(int i=0;argv[i];++i) printf("%s!ARGS\n",argv[i]);
}
#endif

static void sh_handle_cmd(const char *cmd) {
  char ** cmd_dealt=args(cmd);
  char * cmd_name=*cmd_dealt;
  #ifdef DEBUG
  check(cmd_name,cmd_dealt);
  #endif
  for(int i=0;i<NR_CMD;++i)
  if(strcmp(cmd_name,cmd_table[i].name)==0){
    int err_code;
    if((err_code=cmd_table[i].handler(cmd_dealt+1))) sh_printf("ERROR_CODE:%d\n",err_code);
    free_args(--cmd_dealt);
    return;
  }
  if(execvp(cmd_name,cmd_dealt)==-1) sh_printf("Program %s Not Found!\n",cmd_name);
  free_args(--cmd_dealt);
  return;
}

void env_init(){
  setenv("PATH","/bin/:/usr/bin/",0);
//  sh_printf("%s",getenv("PATH"));
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


/*  static char *ret[100];
  ret[0]=strtok(p," ");
  if(ret[0]){
    pp(ret[0]);
//    printf("0:%s\n",ret[0]);
    int i=1;
    for(;(ret[i]=strtok(NULL," "))!=NULL;++i){
      pp(ret[i]);
//      printf("%d:%s\n",i,ret[i]);
    }
    ret[i]=NULL;
  }
  return ret;*/

  /*
  //  sh_printf("%s\n",cmd);
  strcpy(temp,cmd);
  if(cmd[0]=='/'){
    char * filename=strtok(temp," ");pp(filename);
//    printf("%s\n",filename);
    char * ndd=strtok(NULL," ");
//    printf("%s\n",ndd);
    char ** my=args(ndd);
//    check(filename,my);
//    printf("From sh_handle_cmd %p %p\n",my,*my);
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
  if(execvp(cmd_,my)==-1) sh_printf("Unkown or Not Handled Command '%s' :-(\n",cmd_);
  return;*/