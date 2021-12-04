#include <am.h>
#include <stdio.h>
#include <unistd.h>

Area heap;

void putch(char ch) {
    putchar(ch);
}

void halt(int code) {
    _exit(code);    
}
