#include <hos/hos.h>
#include <hos/types.h>
#include <hos/io.h>
#include <hos/console.h>
#include <hos/string.h>


char message[] = "Hello, Hos!\t";
char buf[1024];

void kernel_init() {
    console_init();
    
    console_write(message, sizeof(message) - 1);

    
    return;
}
