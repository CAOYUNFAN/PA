#include<unistd.h>
#include<stdio.h>
#include<sys/time.h>
#include<NDL.h>

int main(){
  NDL_Init(0);
  uint32_t old=NDL_GetTicks();
  while (1)  {
    /*if(tv.tv_usec-500>=0&&temp==1||tv.tv_usec-500<0&&temp==0){
      printf("%ld seconds, %ld microseconds passed!\n",tv.tv_sec,tv.tv_usec);
      temp=1-temp;
    }*/
    uint32_t now=NDL_GetTicks();
    if(now-old>=500) printf("%u microseconds passed!\n",now),old=now;
  }
  return 0;
}
