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
	int len=vsprintf(temp,fmt,ap);
	va_end(ap);
	putstr(temp);
	return len;
}
static char temp2[100];
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
				case 'X': case 'x':d=va_arg(ap,int);
						st=unum_to_str(st,(unsigned)d,16);
						break;
				case '0':{int len=0;
						for(++fmt;(*fmt)>='0'&&(*fmt)<='9';++fmt) len=(len*10)+*fmt-'0';
						char *stt=temp2;
						switch (*(fmt)){
						case 's': s=va_arg(ap,char *);
								for(char *ss=s;*ss;++ss,++stt) *stt=*ss;
								break;
						case 'd':d=va_arg(ap,int);
								stt=num_to_str(stt,d);
								break;
						case 'c':c=(char)va_arg(ap,int);
								*stt++=c;
								break;
						case '%':*stt++='%';
								break;
						case 'u':d=va_arg(ap,int);
								stt=unum_to_str(stt,(unsigned)d,10);
								break;
						case 'o':d=va_arg(ap,int);
								stt=unum_to_str(stt,(unsigned)d,8);
								break;
						case 'X': case 'x':d=va_arg(ap,int);
								stt=unum_to_str(stt,(unsigned)d,16);
								break;
						default:panic("Not implemented or error happens");
							break;
						}
						*stt='\0';
						for(int i=0;i+strlen(temp2)<len;++i) *(st++)='0';
						strcpy(st,temp2);
						st+=strlen(temp2);
						break;}
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
	return vsprintf(out,fmt,ap);
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
	va_list ap; va_start(ap,fmt);
	return vsnprintf(out,n,fmt,ap);
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
	int len=vsprintf(temp,fmt,ap);
	strncpy(out,temp,n-1);
	out[n-1]='\0';
	return len;
}

#endif
