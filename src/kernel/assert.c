#include <common/printk.h>
#include <common/assert.h>
#include <lib/stdarg.h>
#include <lib/stdio.h>
#include <hos/types.h>



static u8 buf[1024];

// 强制阻塞
static void spin(char *name) {
    printk("spinning is %s ...\n", name);
    while (true);
}


void assert_failed(char *exp, char *file, char *base, int line) {
    
    printk(
        "\n--> assert(%s) failed!!!\n"
        "--> file:%s \n"
        "--> base:%s \n"
        "--> line:%d \n",
        exp, file, base, line);

    spin("assertion_failuer()");


    asm volatile("ud2");
}


void panic(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    int i = vsprintf(buf, fmt, args);
    va_end(args);

    printk("!!!! panic!!!!\n%s \n", buf);

    spin("panic()");

    
    asm volatile("ud2");
}








