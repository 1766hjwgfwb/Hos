#include <hos/hos.h>
#include <hos/types.h>
#include <hos/io.h>
#include <hos/string.h>


#define CRT_ADDR_REG 0x3d4
#define CRT_DATA_REG 0x3d5

#define CRT_CURSOR_H 0x0e
#define CRT_CURSOR_L 0x0f

char message[] = "Hello, world!!!";
char buf[1024];

void kernel_init() {
    int res;

    res = strcmp(buf, message);
    strcpy(buf, message);
    res = strcmp(buf, message);
    strcat(buf, message);
    res = strcmp(buf, message);

    res = strlen(message);
    res = sizeof(message);

    char *ptr = strchr(message, '!');
    ptr = strrchr(message, '!');
    memset(buf, 0, sizeof(buf));
    res = memcmp(buf, message, sizeof(message));
    memcpy(buf, message, sizeof(message));
    res = memcmp(buf, message, sizeof(message));
    ptr = memchr(message, '!', sizeof(message));

    return;
}
