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

void check_elf(const Elf_Ehdr *ehdr){
  assert(ehdr->e_ident[EI_MAG0]==0x7f&&ehdr->e_ident[EI_MAG1]=='E');
  assert(ehdr->e_ident[EI_MAG2]=='L'&&ehdr->e_ident[EI_MAG3]=='F');
  #ifdef __LP64__
  assert(ehdr->e_ident[EI_CLASS]==ELFCLASS64);
  #else
  assert(ehdr->e_ident[EI_CLASS]==ELFCLASS32);
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
  assert(ehdr->e_machine==EXPECT_TYPE);
  return;
}

enum file_lseek_related {SEEK_SET,SEEK_CUR,SEEK_END};

static uintptr_t loader(PCB *pcb, const char *filename) {
extern int fs_open(const char *pathname, int flags, int mode);
extern size_t fs_read(int fd, void *buf, size_t len);
extern size_t fs_lseek(int fd, size_t offset, int whence);
//  extern size_t ramdisk_read(void *buf, size_t offset, size_t len);
//  extern size_t ramdisk_write(const void *buf, size_t offset, size_t len);

#define CAO_set_and_read(name,offset) ({\
  fs_lseek(fd,offset,SEEK_SET);\
  assert(fs_read(fd,&name,sizeof(name))==sizeof(name));\
  })

  int fd=fs_open(filename,0,0);
  static Elf_Ehdr ehdr;
  fs_read(fd,&ehdr,sizeof(ehdr));
  check_elf(&ehdr);
  size_t total=ehdr.e_phnum;
  if(total==PN_XNUM){
    Elf_Shdr shdr;
    CAO_set_and_read(shdr,ehdr.e_shoff);
    total=shdr.sh_info;
  }
  static Elf_Phdr phdr;
  for(size_t i=0,j=ehdr.e_phoff;i<total;++i,j+=ehdr.e_phentsize){
    CAO_set_and_read(phdr,j);
    if(phdr.p_type==PT_LOAD){
      fs_lseek(fd,phdr.p_offset,SEEK_SET);
      assert(fs_read(fd,(void *)phdr.p_vaddr,phdr.p_filesz)==phdr.p_filesz);
      memset((void *)(phdr.p_vaddr+phdr.p_filesz),0,phdr.p_memsz-phdr.p_filesz);
    }
  }
//  panic("Successfully read. Not Implemnted");
  Log("loading ready.");
  return ehdr.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}

