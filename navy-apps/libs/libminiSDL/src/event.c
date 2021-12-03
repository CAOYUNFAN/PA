#include <NDL.h>
#include <SDL.h>
#include <string.h>
#include <assert.h>

#define keyname(k) #k,

static char temp[100];
static const char *keyname[] = {
  "NONE",
  _KEYS(keyname)
};

static inline void deal_event(SDL_Event *event){
//  printf("%s %s ",temp,temp+3);
  if(temp[1]=='d') event->type=SDL_KEYDOWN;else event->type=SDL_KEYUP;
  for(int i=1;i<sizeof(keyname)/sizeof(char *);++i) 
  if(strcmp(temp+3,keyname[i])==0){
    event->key.keysym.sym=i;
//    printf("%d\n",i);
    break;
  }
  return;
}

int SDL_PushEvent(SDL_Event *ev) {
  return 0;
}

int SDL_PollEvent(SDL_Event *ev) {
//  printf("EVENT==NULL:%d\n",ev==NULL?1:0);
  if(ev==NULL) return 1;
  if(!NDL_PollEvent(temp,100)) return /*printf("Nothing!\n"),*/0;
  deal_event(ev);
  return 1;
}

int SDL_WaitEvent(SDL_Event *event) {
  assert(event!=NULL);
  int len=NDL_PollEvent(temp,100);
  while (len==0) len=NDL_PollEvent(temp,100);
  deal_event(event);
  return 1;
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
  return 0;
}

uint8_t* SDL_GetKeyState(int *numkeys) {
  return malloc(83);
}
