#ifndef HOS_STRING_H
#define HOS_STRING_H


#include <hos/types.h>


// * functions *

char *strcpy(char *dest, const char *src);
char *strcat(char *dest, const char *src);
size_t strlen(const char *str);
int strcmp(const char *str1, const char *str2);
char *strchr(const char *str, int c);
char *strrchr(const char *str, int c);


int memcmp(const void *s1, const void *s2, size_t n);
void *memset(void *s, int c, size_t n);
void *memcpy(void *dest, const void *src, size_t n);
void *memchr(const void *s, int c, size_t n);








#endif /* HOS_STRING_H */