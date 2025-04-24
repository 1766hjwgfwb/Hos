#include <common/hos.h>
#include <hos/types.h>
#include <hos/io.h>
#include <common/console.h>
#include <lib/string.h>
#include <common/printk.h>


char message[] = "Hello, Hos!";
char buf[1024];




void kernel_init() {
    console_init();


    printk("%x", 0x12345678);


    
    return;
}
