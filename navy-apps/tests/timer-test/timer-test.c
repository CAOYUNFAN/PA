#include<unistd.h>
#include<stdio.h>
#include<sys/time.h>

int main(){
  struct timeval tv;
  struct timezone tz;
  gettimeofday(&tv,&tz);
  __suseconds_t temp=tv.tv_sec;
  while (1)  {
    gettimeofday(&tv,&tz);
    /*if(tv.tv_usec-500>=0&&temp==1||tv.tv_usec-500<0&&temp==0){
      printf("%ld seconds, %ld microseconds passed!\n",tv.tv_sec,tv.tv_usec);
      temp=1-temp;
    }*/
    if(temp!=tv.tv_sec) printf("%ld seconds, %ld microseconds passed!\n",tv.tv_sec,tv.tv_usec),temp=tv.tv_sec;
  }
}
