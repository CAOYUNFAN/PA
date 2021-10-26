#include <am.h>
#include <nemu.h>

#define KEYDOWN_MASK 0x8000

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  static bool keyboard_status[AM_KEY_PAGEDOWN+1]={};
  uint32_t temp=inl(KBD_ADDR);
  kbd->keydown = (keyboard_status[temp]=!keyboard_status[temp]);
  kbd->keycode = temp;
}
