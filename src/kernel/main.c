#include <hos/hos.h>


int magic = HOS_MAGIC;
char message[] = "Hello, Hos!";
char buf[1024];


void kernel_init() {
    char *video = (char *) 0xb8000; // Video memory address
    for (int i = 0; i < sizeof(message); i++) {
        video[i * 2] = message[i];
    }
}
