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
  const char *now=buf;
  for(int i=0;i<len;++i,++now) putch(*now);
  return 0;
}

inline static size_t min(size_t a,size_t b){return a<b?a:b;}

struct AM_INPUT_KEYBRD_T{
  bool keydown;
  int keycode;
};

size_t events_read(void *buf, size_t offset, size_t len) {

  extern void __am_input_keybrd(struct AM_INPUT_KEYBRD_T *kbd);
  struct AM_INPUT_KEYBRD_T kbd;
  __am_input_keybrd(&kbd);
  if(kbd.keycode==0) return 0;
  if(kbd.keydown) sprintf(buf,"kd %s",keyname[kbd.keycode]);
  else sprintf(buf,"ku %s",keyname[kbd.keycode]);
  return min(len,strlen(buf));
}

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  return 0;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  return 0;
}

void init_device() {
  Log("Initializing devices...");
  ioe_init();
}
