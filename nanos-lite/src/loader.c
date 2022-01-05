#include <proc.h>
#include <elf.h>

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
# define Elf_Shdr Elf64_Shdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
# define Elf_Shdr Elf32_Shdr
#endif

bool check_elf(const Elf_Ehdr *ehdr){
  if(ehdr->e_ident[EI_MAG0]!=0x7f||ehdr->e_ident[EI_MAG1]!='E') return 0;
  if(ehdr->e_ident[EI_MAG2]!='L'||ehdr->e_ident[EI_MAG3]!='F') return 0;
  #ifdef __LP64__
  if(ehdr->e_ident[EI_CLASS]!=ELFCLASS64) return 0;
  #else
  if(ehdr->e_ident[EI_CLASS]!=ELFCLASS32) return 0;
  #endif
  
  #if defined(__ISA_AM_NATIVE__)
  # define EXPECT_TYPE EM_X86_64
  #elif defined(__ISA_X86__)
  # define EXPECT_TYPE EM_386
  #elif defined(__ISA_MIPS32__)
  # define EXPECT_TYPE EM_MIPS_X
  #elif defined(__ISA_RISCV32__)
  # define EXPECT_TYPE EM_RISCV
  #elif defined(__ISA_RISCV64__)
  # define EXPECT_TYPE EM_RISCV
  #else
  # error Unsupported ISA
  #endif
  if(ehdr->e_machine!=EXPECT_TYPE) return 0;
  return 1;
}

enum file_lseek_related {SEEK_SET,SEEK_CUR,SEEK_END};

#ifdef HAS_VME
void* new_page(size_t nr_page);
/*
typedef struct{
  uintptr_t virtual,physical;
}checker;
static checker a[1024];
static int tot=0;
static inline void initial_page_manager(){tot=0;return;}

static uintptr_t get_page(AddrSpace * as,uintptr_t vaddr){
  for(int i=0;i<tot;++i) if(a[i].virtual==vaddr) return a[i].physical;
  a[tot].virtual=vaddr;a[tot].physical=(uintptr_t) new_page(1);
  map(as,(void *)vaddr,(void *)a[tot].physical,0);
  return a[tot++].physical;
}*/
extern uintptr_t check_map(AddrSpace *as,void * va);
static inline uintptr_t get_page(AddrSpace *as,uintptr_t vaddr){
  uintptr_t ret=check_map(as,(void *)vaddr);
  if(!ret) map(as,(void *)vaddr,(void *)(ret=(uintptr_t)new_page(1)),0);
  return ret;
}
#else
static inline void initial_page_manager(){}
static inline uintptr_t get_page(AddrSpace *as,uintptr_t vaddr){return vaddr;}
#endif

static inline int Min(int a,int b) {return a<b?a:b;}
#define Max(a,b) ((a)>(b)?(a):(b))
static uintptr_t loader(PCB *pcb, const char *filename) {//Log("%p %s",pcb,filename);
  extern int fs_open(const char *pathname, int flags, int mode);
  extern size_t fs_read(int fd, void *buf, size_t len);
  extern size_t fs_lseek(int fd, size_t offset, int whence);
  extern int fs_close(int fd);

#define CAO_set_and_read(name,offset) ({\
  fs_lseek(fd,offset,SEEK_SET);\
  assert(fs_read(fd,&name,sizeof(name))==sizeof(name));\
  })
//  initial_page_manager();
  int fd=fs_open(filename,0,0);
  if(fd==-1) return 0;
  static Elf_Ehdr ehdr;
  fs_read(fd,&ehdr,sizeof(ehdr));
  if(!check_elf(&ehdr)) return 0;
  size_t total=ehdr.e_phnum;
  if(total==PN_XNUM){
    Elf_Shdr shdr;
    CAO_set_and_read(shdr,ehdr.e_shoff);
    total=shdr.sh_info;
  }
  AddrSpace * as=&pcb->as;
#ifdef HAS_VME
  int pgsize=as->pgsize;
#else
  int pgsize=4096;
#endif
  static Elf_Phdr phdr;
  for(size_t i=0,j=ehdr.e_phoff;i<total;++i,j+=ehdr.e_phentsize){
    CAO_set_and_read(phdr,j);
    if(phdr.p_type==PT_LOAD){
      fs_lseek(fd,phdr.p_offset,SEEK_SET);
      uintptr_t virtual_page=phdr.p_vaddr&~0xfffu,offset=phdr.p_vaddr&0xfffu;
      int total1=phdr.p_filesz,total2=phdr.p_memsz-phdr.p_filesz;
//      Log("%08x,V=%08x,ALL=%08x",phdr.p_vaddr,phdr.p_filesz,phdr.p_memsz);

      for(;total1;){
        uintptr_t physical_page=get_page(as,virtual_page);
      #ifndef HAS_VME
        assert(physical_page==virtual_page);
      #endif
        int bytes=fs_read(fd,(void *)physical_page+offset,Min(pgsize-offset,total1));
        assert(bytes==Min(pgsize-offset,total1));
//        Log("V:%08x:%08x->%08x,with%08x",physical_page,physical_page+offset,physical_page+offset+bytes,virtual_page);
        offset+=bytes;
        virtual_page+=offset&~0xfff;offset&=0xfff;
        total1-=bytes;
      }
      assert(virtual_page+offset==phdr.p_vaddr+phdr.p_filesz);

      for(;total2;){
        uintptr_t physical_page=get_page(as,virtual_page);
      #ifndef HAS_VME
        assert(physical_page==virtual_page);
      #endif
        int bytes=Min(pgsize-offset,total2);
        memset((void *)(physical_page+offset),0,bytes);
//        Log("Z:%08x:%08x->%08x,with%08x",physical_page,physical_page+offset,physical_page+offset+bytes,virtual_page);
        offset+=bytes;
        virtual_page+=offset&~0xfff;offset&=0xfff;
        total2-=bytes;
      }
      assert(virtual_page+offset==phdr.p_vaddr+phdr.p_memsz);
//      assert(fs_read(fd,(void *)phdr.p_vaddr,phdr.p_filesz)==phdr.p_filesz);
//      memset((void *)(phdr.p_vaddr+phdr.p_filesz),0,phdr.p_memsz-phdr.p_filesz);
    }
  }
  assert(fs_close(fd)==0);
  return ehdr.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  if(entry){
    Log("Jump to entry = %p", entry);
    ((void(*)())entry) ();
  }
  return;
}

static char * prepare_args_and_stack(AddrSpace *as,char * const argv[],char * const envp[]){
  int argv_count=0; 
#ifdef HAS_VME
  int pgsize=as->pgsize;
#else
  int pgsize=4096;
#endif
//  int envp_count=0;
  #define stack_page_num 8
  static uintptr_t begin_ptr[1024];
  char * temp=new_page(stack_page_num);
#ifdef HAS_VME
  for(int i=0,j=(uintptr_t)as->area.end-stack_page_num*pgsize,k=(uintptr_t)temp;i<8;i++,j+=pgsize,k+=pgsize){
    map(as,(void *)j,(void *)k,0);
//    Log("%08x->%08x",j,k);
  }
#endif
  temp+=stack_page_num*pgsize;
  char * end_ptr=temp;
//  Log("%p",end_ptr);
  int now=1;
  for(;*argv;++argv){
    begin_ptr[now++]=(uintptr_t)(end_ptr=strcpy(end_ptr-strlen(*argv)-1,*argv));
    ++argv_count;
  }
  *begin_ptr=argv_count;
  begin_ptr[now++]=0;
  for(;*envp;++envp){
    begin_ptr[now++]=(uintptr_t)(end_ptr=strcpy(end_ptr-strlen(*envp)-1,*envp));
//    ++envp_count;
  }
//  Log("envp_count:%d",envp_count);
  end_ptr=(char *)((uintptr_t)end_ptr&~0x3u);
  begin_ptr[now++]=0;
#if defined __ISA_AM_NATIVE__
  end_ptr=(char *)((((uintptr_t)end_ptr-now*sizeof(uintptr_t))&~0xf)+now*sizeof(uintptr_t));
#endif
  end_ptr=memcpy(end_ptr-now*sizeof(uintptr_t),begin_ptr,now*sizeof(uintptr_t));
#ifdef HAS_VME
  return (char *)as->area.end-(temp-end_ptr);
#else
  return end_ptr;
#endif
}

bool context_uload(PCB *pcb, const char *filename, char *const argv[], char *const envp[]){
#ifdef HAS_VME
  protect(&pcb->as);//Log("%10s:%08x,%p",filename,pcb->as.ptr,pcb);
#endif
  uintptr_t ret=(uintptr_t)prepare_args_and_stack(&pcb->as,argv,envp);
  Area mystack={
    .start=(void *)pcb->stack,
    .end=(void *)(pcb->stack+STACK_SIZE)
  };
  void * entry=(void *)loader(pcb,filename);
  if(!entry) return 0;
  pcb->cp=ucontext(&pcb->as,mystack,entry);
  pcb->cp->GPRx=ret;
  pcb->max_brk=0;
  Log("File%s:entry=%p,Stack starts From%p(%08x),pcb->cp=%08x",filename,entry,pcb->cp->GPRx,&pcb->cp->GPRx,pcb->cp);
  return 1;
}