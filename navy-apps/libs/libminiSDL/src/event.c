#include <NDL.h>
#include <SDL.h>
#include <string.h>

#define keyname(k) #k,

static const char *keyname[] = {
  "NONE",
  _KEYS(keyname)
};

int SDL_PushEvent(SDL_Event *ev) {
  return 0;
}

int SDL_PollEvent(SDL_Event *ev) {
  return 0;
}

int SDL_WaitEvent(SDL_Event *event) {
/*  static char temp[100];int len=NDL_PollEvent(temp,100);
  while (len==0) len=NDL_PollEvent(temp,100);
  if(temp[1]=='d') event->type=SDL_KEYDOWN;else event->type=SDL_KEYUP;
  for(int i=1;i<sizeof(keyname)/sizeof(char *);++i) 
  if(strcmp(temp+3,keyname[i])==0){
    event->key.keysym.sym=i;
    break;
  }*/
  return 1;
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
  return 0;
}

uint8_t* SDL_GetKeyState(int *numkeys) {
  return NULL;
}
