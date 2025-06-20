#ifndef IO_H
#define IO_H

#include <hos/types.h>


// * extern functions *

extern u8 inb(u16 port);    // input byte
extern u16 inw(u16 port);   // input word

extern void outb(u16 port, u8 value);    // output byte
extern void outw(u16 port, u16 value);   // output word














#endif /* IO_H */