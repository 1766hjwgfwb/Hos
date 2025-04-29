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

    task_init();

    return;
}
