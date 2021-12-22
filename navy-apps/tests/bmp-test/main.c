#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <NDL.h>
#include <BMP.h>
#include <stdint.h>
//#define SYS_yield 1
//extern int _syscall_(int, uintptr_t, uintptr_t, uintptr_t);
int main() {
  NDL_Init(0);//_syscall_(SYS_yield, 0, 0, 0);
  int w, h;
  void *bmp = BMP_Load("/share/pictures/projectn.bmp", &w, &h);
  assert(bmp);
  NDL_OpenCanvas(&w, &h);
  NDL_DrawRect(bmp, 0, 0, w, h);
  free(bmp);
  NDL_Quit();
  printf("Test ends! Spinning...\n");
  while (1);
  return 0;
}
