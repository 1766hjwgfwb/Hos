#ifndef __ASSERT_H__
#define __ASSERT_H__



void assert_failed(char *exp, char *file, char *base, int line);


#define assert(exp) \
    if (exp)        \
        ;           \
    else            \
        assert_failed(#exp, __FILE__, __BASE_FILE__, __LINE__)


void panic(const char *fmt, ...);



#endif /* __ASSERT_H__ */