#include<unistd.h>
#include<stdio.h>
#include<sys/time.h>

int main(){
  struct timeval tv;
  struct timezone tz;
  gettimeofday(&tv,&tz);
  time_t temp=tv.tv_sec;
  while (1)  {
    gettimeofday(&tv,&tz);
    if(temp!=tv.tv_sec) {printf("%ld seconds, %ld microseconds passed!\n",tv.tv_sec,tv.tv_usec);temp=tv.tv_sec;}
  }
}
