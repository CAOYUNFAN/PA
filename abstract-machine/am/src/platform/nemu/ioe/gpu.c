#include <am.h>
#include <nemu.h>
#include <stdio.h>

#define SYNC_ADDR (VGACTL_ADDR + 4)
#define wide (uint32_t)inw(VGACTL_ADDR+2)
#define high (uint32_t)inw(VGACTL_ADDR)

void __am_gpu_init() {
  /*int i;
  int w = (int)wide;  // TODO: get the correct width
  int h = (int)high;  // TODO: get the correct height
  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  for (i = 0; i < w * h; i ++) fb[i] = i;
  outl(SYNC_ADDR, 1);*/
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  uint32_t w=wide,h=high;
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = w, .height = h,
    .vmemsz = w*h*sizeof(uint32_t)
  };
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  size_t *data=ctl->pixels;
//  printf("x=%3u,y=%3u,h=%u,w=%u,width=%u,height=%u\n",ctl->x,ctl->y,ctl->h,ctl->w,wide,high);
  for(int i=0;i<ctl->h;++i)
  for(int j=0;j<ctl->w;++j)
  outl(((ctl->x+j)+wide*(ctl->y+i))*4+FB_ADDR,*(data++));
  if (ctl->sync) {
    outl(SYNC_ADDR, 1);
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}
