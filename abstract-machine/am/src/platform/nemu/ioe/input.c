#include <am.h>
#include <nemu.h>
#include <stdio.h>

#define KEYDOWN_MASK 0x8000
#define totnum_keyboard 101
//static bool keyboard_status[totnum_keyboard]={};
void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  uint32_t temp=inl(KBD_ADDR);
  if(temp)printf("testing!%x",temp);
  kbd->keydown = (temp&KEYDOWN_MASK);//(keyboard_status[temp]=!keyboard_status[temp]);
  kbd->keycode = temp&(KEYDOWN_MASK-1);
}
