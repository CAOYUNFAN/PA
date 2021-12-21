#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <NDL.h>
#include <BMP.h>

int main() {
  NDL_Init(0);
  int w, h;
  void *bmp = BMP_Load("/share/pictures/projectn.bmp", &w, &h);
  assert(bmp);
  printf("DEBUG1!\n");
  NDL_OpenCanvas(&w, &h);
  printf("DEBUG2!\n");
  NDL_DrawRect(bmp, 0, 0, w, h);
  printf("DEBUG3!\n");
  free(bmp);
  printf("DEBUG4!\n");
  NDL_Quit();
  printf("Test ends! Spinning...\n");
  while (1);
  return 0;
}
