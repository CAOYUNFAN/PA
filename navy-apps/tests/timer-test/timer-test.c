#include<unistd.h>
#include<stdio.h>
#include<sys/time.h>

int main(){
  volatile struct timeval tv;
  volatile struct timezone tz;
  gettimeofday(&tv,&tz);
  volatile int temp=tv.tv_sec;
  while (1)  {
    gettimeofday(&tv,&tz);
    if(temp!=tv.tv_sec) {puts("One seconds passed!");temp=tv.tv_sec;}
  }
}
