.globl  _start
_start:
  movq $0, %rbp
  // (rsp + 8) should be multiple of 16 when
  // control is transfered to the function entry point.
  // See amd64 ABI manual for more details
  movq %rax, %rsp
  andq $0xfffffffffffffff0, %rsp
  subq $-8, %rsp
  movq %rax, %rdi
  call call_main
