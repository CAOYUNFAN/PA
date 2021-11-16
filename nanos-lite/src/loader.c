#include <proc.h>
#include <elf.h>

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
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

static uintptr_t loader(PCB *pcb, const char *filename) {
  Elf_Ehdr ehdr;
  extern size_t ramdisk_read(void *buf, size_t offset, size_t len);
  extern size_t ramdisk_write(const void *buf, size_t offset, size_t len);
  ramdisk_read(&ehdr,0,sizeof(ehdr));
  check_elf(&ehdr);
  

  return 0;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}

