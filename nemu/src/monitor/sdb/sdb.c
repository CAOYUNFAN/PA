#include <isa.h>
#include <cpu/cpu.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "sdb.h"
#include <elf.h>

static int is_batch_mode = false;

void init_regex();
void init_wp_pool();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  nemu_state.state=NEMU_END;
  return -1;
}

static int cmd_help(char *args);
static int cmd_si(char *args);
static int cmd_info(char *args);
static int cmd_x(char *args);
static int cmd_p(char *args);
static int cmd_w(char *args);
static int cmd_d(char *args);

static struct {
  const char *name;
  const char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },

  {"si","Let the program execute N instructions in a single step and pause execution, when N is not given, it defaults to 1",cmd_si},
  {"info"," r :print register status; w :print monitoring point information",cmd_info},
  {"x","Find the value of expression expr and take the result as the starting memeory address,which outputs n consecutive 4 bytes in hexadecimal form",cmd_x},
  {"p","Find the value of expression expr",cmd_p},
  {"w","When the value of expression expr changes, program execution is suspended",cmd_w},
  {"d","Delete the monitoring point with sequence number n",cmd_d}
  /* TODO: Add more commands */

};

#define NR_CMD ARRLEN(cmd_table)

 static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

static int cmd_si(char *args){
	char *temp=strtok(args," ");
	int x=(temp==NULL)?1:atol(temp);
	cpu_exec(x);
	return 0;
}

static int cmd_info(char *args){
	if(*args=='r') isa_reg_display();
	if(*args=='w'){
		extern void show_checkpoints();
		show_checkpoints();
	}
	return 0;
}

static int cmd_x(char *args){
	int n;
	char ex[100];
	bool success=1;
	extern word_t vaddr_read(vaddr_t addr,int len);
	sscanf(args,"%d %s",&n,ex);
	word_t j=expr(ex,&success);
	if(!success){
		printf("Invalid expr!\n");
		return 0;
	}
	for(int i=0;i<n;++i,j+=4)
		printf("0x%08x:\t0x%08x\n",j,vaddr_read(j,4));
	return 0;
}

static int cmd_p(char *args){
	bool success;
	word_t ans=expr(args,&success);
	if(success) printf("%u\t%x\n",ans,ans);
	else printf("Mistakes happen.\n");
	return 0;
}

static int cmd_w(char *args){
	extern void new_WP(char *args);
	new_WP(args);
	return 0;
}

static int cmd_d(char *args){
	int x;sscanf(args,"%d",&x);
	extern void checkpoint_del(int x);
	checkpoint_del(x);
	return 0;
}

void sdb_set_batch_mode() {
  is_batch_mode = true;
}

#ifdef CONFIG_FTRACE
int ftrace_mode = false;
int tot_sym;
Elf32_Ehdr ehdr;
Elf32_Shdr shdr[105];
Elf32_Sym sym[10005];
#endif

void ftrace_init(char *args) {
#ifdef CONFIG_FTRACE
	ftrace_mode=true;
	FILE *fp=fopen(args,"rb");
	Assert(fp,"Can not open '%s'",args);
  char str[100]; unsigned int temp;
  temp=fread(str,1,5,fp);
  if(str[0]!=0x7f||str[1]!='E'||str[2]!='L'||str[3]!='F'||str[4]!=1||temp!=5){
    panic("This is not an elf file or the the type is not 32_byte!");
  }
	Log("The elf is '%s'",args);
  fseek(fp,0,SEEK_SET);
  temp=fread(&ehdr,sizeof(Elf32_Ehdr),1,fp);
  Assert(temp==1,"Error happens.");
  fseek(fp,ehdr.e_shoff,SEEK_SET);
  temp=fread(shdr,sizeof(Elf32_Shdr),ehdr.e_shnum,fp);
  Assert(temp==ehdr.e_shnum,"Error happens.");
  for(int i=0;i<ehdr.e_shnum;++i)
  if(shdr[i].sh_type==SHT_SYMTAB){
    fseek(fp,shdr[i].sh_offset,SEEK_SET);
    tot_sym=shdr[i].sh_size/shdr[i].sh_entsize;
    break;
  }
  temp=fread(sym,sizeof(Elf32_Sym),tot_sym,fp);
  Assert(temp==tot_sym,"Error happens.");
  fclose(fp);
#else
  panic("FTRACE is not available!");
#endif
}

void sdb_mainloop() {
   if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

   for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}

void init_sdb() {
  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
  init_wp_pool();
}
