#include<unistd.h>
#include<stdio.h>
#include<sys/time.h>

int main(){
  struct timeval tv;
  struct timezone tz;
  gettimeofday(&tv,&tz);
  __suseconds_t temp=500;
  while (1)  {
    gettimeofday(&tv,&tz);
    if(tv.tv_usec==500-temp) printf("%ld seconds, %ld microseconds passed!\n",tv.tv_sec,tv.tv_usec),temp=500-temp;
  }
}
