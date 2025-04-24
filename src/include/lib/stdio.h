#ifndef _STDIO_H
#define _STDIO_H


#include <lib/stdarg.h>

int vsprintf(char *buf, const char* fmt, va_list args);
int sprintf(char *buf, const char* fmt, ...);










#endif /* _STDIO_H */