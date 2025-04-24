#include <common/console.h>
#include <common/printk.h>
#include <lib/stdarg.h>
#include <lib/stdio.h>


static char buf[1024];

int printk(const char *fmt,...) {
    va_list args;
    int i;

    va_start(args, fmt);

    i = vsprintf(buf, fmt, args);

    va_end(args);



    console_write(buf, i);


    return i;
}

