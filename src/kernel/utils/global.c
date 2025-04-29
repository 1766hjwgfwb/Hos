#include <hos/global.h>
#include <hos/debug.h>
#include <lib/string.h>



descriptor_t gdt[GDT_SIZE];   // 内核全局描述符表
pointer_t gdt_ptr;          // 全局描述符表指针


// init
void gdt_init() {
    // DEBUGK("init_gdt!!!\n");

    asm volatile("sgdt gdt_ptr");


    // 复制全局描述符表到内核全局描述符表
    // limit + 1 (3^8 - 1) + 1
    memcpy(&gdt, (void *)gdt_ptr.base, gdt_ptr.limit + 1);

    gdt_ptr.base = (u32)&gdt;
    gdt_ptr.limit = (u16)(sizeof(gdt) - 1);
    
    asm volatile("lgdt gdt_ptr\n");

    BMB;
}