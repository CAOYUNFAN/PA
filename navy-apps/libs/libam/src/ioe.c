#include <am.h>

bool ioe_init() {
  return true;
}

static void do_io(int reg, void *buf) {
  ((handler_t)lut[reg])(buf);
}

void ioe_read (int reg, void *buf) { do_io(reg, buf); }
void ioe_write(int reg, void *buf) { do_io(reg, buf); }
