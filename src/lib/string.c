#include <lib/string.h>


int strcmp(const char *str1, const char *str2) {
    // if *str1 == *str2, compare the next character
    while (*str1 == *str2 && *str1!= EOS && *str2!= EOS) {
        str1++;
        str2++;
    }


    // return (*str1 == *str2) ? 0 : (*str1 < *str2) ? -1 : 1;
    return (unsigned char)*str1 - (unsigned char)*str2;
}


char *strchr(const char *s, int c) {
    unsigned char ch = (unsigned char)c;
    while (*s != EOS) {
        if ((unsigned char)*s == ch) {
            return (char *)s;
        }

        s++;
    }

    return (ch == 0) ? (char *)s : NULL;
}


// * char c last of string
char *strrchr(const char *str, int c) {
    char *ptr = (char *)str;
    char *last = NULL;

    while (*ptr != EOS) {
        if ((unsigned char)*ptr == (unsigned char)c) {
            last = ptr;
        }

        ptr++;
    }

    return last;
}


char *strcat(char *dest, const char *src) {
    char *ptr = dest;
    while (*ptr != EOS) {
        ptr++;
    } 

    while (*src != EOS) {
        *ptr++ = *src++;
    }    
    
    *ptr = '\0';

    return dest;
}


char *strcpy(char *dest, const char *src) {
    char *ptr = dest;
    while (*src != EOS) {
        *ptr++ = *src++;
    }    
    
    *ptr = '\0';

    return dest;
}


// * n bytes to compare
int memcmp(const void *s1, const void *s2, size_t n) {
    char *ptr1 = (char *)s1;
    char *ptr2 = (char *)s2;

    while (n-- > 0) {
        if (*ptr1 > *ptr2)
            return 1;
        else if (*ptr1 < *ptr2)
            return -1;

        ptr1++;
        ptr2++;
    }

    return 0;   // if return 0, s1 == s2
}


// * n bytes (char c) to set
void *memset(void *s, int c, size_t n) {
    char *ptr = (char *)s;

    while (n-- > 0) {
        *ptr++ = c;
    }

    return s;
}


// * n bytes to copy
void *memcpy(void *dest, const void *src, size_t n) {
    char *ptr1 = (char *)dest;
    char *ptr2 = (char *)src;

    while (n-- > 0) {
        *ptr1++ = *ptr2++;
    }

    return dest;
}


size_t strlen(const char *str) {
    const char *ptr = str;

    while (*str) {          // ! 原实现严重BUG !!!
        str++;
    }
    
    return str - ptr;
}


// * first char c of string
void *memchr(const void *s, int c, size_t n) {
    char *ptr = (char *)s;

    while (n-- > 0) {
        if (*ptr == c)
            return (void *)ptr;

        ptr++;
    }

    return NULL;  // if not found
}