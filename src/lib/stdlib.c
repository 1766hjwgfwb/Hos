#include <lib/stdlib.h>


void delay(u32 count) {
    while (count--)
        ;
}


void hang() {
    while (1)
        ;
}