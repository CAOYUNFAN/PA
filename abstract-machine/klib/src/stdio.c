#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)
static const char num_table[]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
inline static char* num_to_str(char *st,int d){
	if(d==0){*st++='0';return st;}
	if(d<0){*st++='-';d=-d;}
	char temp[30];
	int top=0;
	for(;d;d/=10) temp[++top]=d%10+'0';
	while(top) *st++=temp[top--];
	return st;
}

inline static char* unum_to_str(char *st,unsigned int d,int base){
	if(d==0){*st++='0';return st;}
	char temp[30];
	int top=0;
	for(;d;d/=base) temp[++top]=num_table[d%base];
	while(top) *st++=temp[top--];
	return st;
}

#define MAX_NUM_stdio 1000
static char temp[MAX_NUM_stdio];

int printf(const char *fmt, ...) {
	va_list ap; va_start(ap,fmt);
	int t=vsprintf(temp,fmt,ap);
	va_end(ap);
	putstr(temp);
	return t;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  	int d;char c;char *st=out;char* s;
	while(*fmt){
		if(*fmt=='%'){
			switch(*(++fmt)){
				case 's': s=va_arg(ap,char *);
						for(char *ss=s;*ss;++ss,++st) *st=*ss;
						break;
				case 'd':d=va_arg(ap,int);
						st=num_to_str(st,d);
						break;
				case 'c':c=(char)va_arg(ap,int);
						*st++=c;
						break;
				case '%':*st++='%';
						break;
				case 'u':d=va_arg(ap,int);
						st=unum_to_str(st,(unsigned)d,10);
						break;
				case 'o':d=va_arg(ap,int);
						st=unum_to_str(st,(unsigned)d,8);
						break;
				case 'x':d=va_arg(ap,int);
						st=unum_to_str(st,(unsigned)d,16);
						break;
				default:panic("Not implemented or error happens");
			}
			++fmt;
		}else *st++=*fmt++;
	}
	*st='\0';
	return st-out;
}

int sprintf(char *out, const char *fmt, ...) {
	va_list ap; va_start(ap,fmt);
	int temp=vsprintf(out,fmt,ap);
	va_end(ap);
	return temp;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
	va_list ap; va_start(ap,fmt);
	int temp=vsnprintf(out,n,fmt,ap);
	va_end(ap);
	return temp;
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
	int temp=vsprintf(out,fmt,ap);
	if(temp>=n) out[n-1]='\0';
	return temp;
}

#endif
