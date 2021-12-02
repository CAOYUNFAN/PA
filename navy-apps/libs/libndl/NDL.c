#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

static int evtdev = -1;
static int fbdev = -1;
static int screen_w = 0, screen_h = 0, canvas_w = 0, canvas_h = 0, posw = 0, posh = 0 ;

uint32_t NDL_GetTicks() {
  struct timeval tv;
  struct timezone tz;
  gettimeofday(&tv,&tz);
  return tv.tv_sec*1000+tv.tv_usec;
}

int NDL_PollEvent(char *buf, int len) {
  if(read(open("/dev/events",0,0),buf,len)) {
//    printf("%s\n",buf);
    return 1;
  }
  else return 0;
}

void prepare_screen(){
  FILE * fd=fopen("/dev/dispinfo","r");
  fscanf(fd,"WIDTH:%d\nHEIGHT:%d",&screen_w,&screen_h);
  fclose(fd);
}

void NDL_OpenCanvas(int *w, int *h) {
  if (getenv("NWM_APP")) {
    int fbctl = 4;
    fbdev = 5;
    screen_w = *w; screen_h = *h;
    char buf[64];
    int len = sprintf(buf, "%d %d", screen_w, screen_h);
    // let NWM resize the window and create the frame buffer
    write(fbctl, buf, len);
    while (1) {
      // 3 = evtdev
      int nread = read(3, buf, sizeof(buf) - 1);
      if (nread <= 0) continue;
      buf[nread] = '\0';
      if (strcmp(buf, "mmap ok") == 0) break;
    }
    close(fbctl);
  }else{
    prepare_screen();
    canvas_w=*w;canvas_h=*h;
    if(canvas_w==0&&canvas_h==0) canvas_w=screen_w,canvas_h=screen_h;
    posw=(screen_w-canvas_w)/2;
    posh=(screen_h-canvas_h)/2;
//    printf("WIDTH=%d,HEIGHT=%d\n",screen_w,screen_h);
    return;
  }
}

void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) {
//  printf("draw %d %d %d %d\n",x,y,w,h);
  int offset=((posh+y)*screen_w+(x+posw))*sizeof(uint32_t);
  int fd=open("/dev/fb",O_WRONLY,0);
  lseek(fd,offset,SEEK_SET);
  for(int i=0;i<h;++i,offset+=screen_w*sizeof(uint32_t),pixels+=w){
    lseek(fd,offset,SEEK_SET);
//    printf("%u\n",offset);
    write(fd,pixels,w*sizeof(uint32_t));
  }
}

void NDL_OpenAudio(int freq, int channels, int samples) {
}

void NDL_CloseAudio() {
}

int NDL_PlayAudio(void *buf, int len) {
  return 0;
}

int NDL_QueryAudio() {
  return 0;
}

int NDL_Init(uint32_t flags) {
  if (getenv("NWM_APP")) {
    evtdev = 3;
  }
  return 0;
}

void NDL_Quit() {
}
