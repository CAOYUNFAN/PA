#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  panic("Not implemented");
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  panic("Not implemented");
}

int sprintf(char *out, const char *fmt, ...) {
	va_list ap; va_start(ap,fmt);
	int d;char c;char *st=out;char* s;
	while(*fmt){
		if(*fmt=='%'){
			switch(*(++fmt)){
				case 's': s=va_arg(ap,char *);
						for(char *ss=s;*ss;++ss,++st) *st=*ss;
						break;
				case 'd':d=va_arg(ap,int);
						if(d==0){*st++='0';break;}
						if(d<0){*st++='-';d=-d;}
						for(;d;d/=10) *st++=d%10+'0';
						break;
				case 'c':c=(char)va_arg(ap,int);
						*st++=c;
						break;
				default:panic("Not implemented or error happens");
			}
			++fmt;
		}else *st++=*fmt++;
	}
	*st='\0';
	return st-out;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
