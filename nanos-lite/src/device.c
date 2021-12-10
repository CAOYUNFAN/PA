#include <common.h>

#if defined(MULTIPROGRAM) && !defined(TIME_SHARING)
# define MULTIPROGRAM_YIELD() yield()
#else
# define MULTIPROGRAM_YIELD()
#endif

#define NAME(key) \
  [AM_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [AM_KEY_NONE] = "NONE",
  AM_KEYS(NAME)
};

size_t serial_write(const void *buf, size_t offset, size_t len) {
  yield();
  const char *now=buf;
  for(int i=0;i<len;++i,++now) putch(*now);
  return 0;
}

inline static size_t min(size_t a,size_t b){return a<b?a:b;}

size_t events_read(void *buf, size_t offset, size_t len) {
  yield();
  struct AM_INPUT_KEYBRD_T{
    bool keydown; int keycode;
  }kbd;
  extern void __am_input_keybrd(struct AM_INPUT_KEYBRD_T *kbd);
  __am_input_keybrd(&kbd);
  if(kbd.keycode==0) return 0;
  if(kbd.keydown) sprintf(buf,"kd %s",keyname[kbd.keycode]);
  else sprintf(buf,"ku %s",keyname[kbd.keycode]);
  return min(len,strlen(buf));
}

struct AM_GPU_CONFIG_T{
  bool present, has_accel; int width, height, vmemsz;
} cfg;
extern void __am_gpu_config(struct AM_GPU_CONFIG_T *cfg);
size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  __am_gpu_config(&cfg);
  sprintf(buf,"WIDTH:%d\nHEIGHT:%d",cfg.width,cfg.height);
  return min(len,strlen(buf));
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
//  printf("%u %u\n",offset,len);
  yield();
  __am_gpu_config(&cfg);
  struct AM_GPU_FBDRAW_T{
    int x, y; const void *pixels; int w, h; bool sync;
  } ctl;
  extern void __am_gpu_fbdraw(struct AM_GPU_FBDRAW_T *ctl);

  offset/=sizeof(int32_t);len/=sizeof(int32_t);
  ctl.y=offset/cfg.width;ctl.x=offset%cfg.width;
  ctl.w=len;ctl.h=1;
  ctl.sync=1;
  ctl.pixels=buf;
  __am_gpu_fbdraw(&ctl);
  return 0;
}

void init_device() {
  Log("Initializing devices...");
  ioe_init();
}
