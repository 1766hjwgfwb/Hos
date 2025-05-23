#include <common/hos.h>
#include <hos/types.h>
#include <hos/io.h>
#include <hos/global.h>
#include <common/console.h>
#include <lib/string.h>
#include <common/printk.h>
#include <common/assert.h>
#include <hos/debug.h>
#include <lib/stdarg.h>
#include <hos/task.h>
#include <hos/interrupt.h>
#include <lib/stdlib.h>


char message[] = "Hello, Hos!";
char buf[1024];


void print_message() {
    printk("\n");
    printk("\t\t\t  _   _   ____   _____ \n");
    printk("\t\t\t | | | | / ___| | ____|\n");
    printk("\t\t\t | |_| | \\___ \\ |  _|  \n");
    printk("\t\t\t |  _  |  ___) || |___ \n");
    printk("\t\t\t |_| |_| |____/ |_____|\n");
    printk("\n");
}


void kernel_init() {
    console_init();

    gdt_init();

    // print_message();
    // task_init();
    interrupt_init();

    asm volatile(
        "sti\n"
    );

    u32 counter = 3;
    while (counter--) {
        DEBUGK("looping in kernel_init: %d\n", counter);
        delay(100000000);
    }

    return;
}
