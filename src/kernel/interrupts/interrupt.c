#include <hos/interrupt.h>
#include <hos/global.h>
#include <hos/debug.h>
#include <hos/task.h>
#include <hos/io.h>
#include <common/printk.h>
#include <lib/stdlib.h>
#include <common/assert.h>


#define ENTRY_SIZE 0x30

#define LOGK(fmt, args...) DEBUGK(fmt, ##args)
// #define LOGK(fmt, args...)


#define PIC_M_CTRL 0x20 // 主片的控制端口
#define PIC_M_DATA 0x21 // 主片的数据端口
#define PIC_S_CTRL 0xA0 // 从片的控制端口
#define PIC_S_DATA 0xA1 // 从片的数据端口

#define PIC_EOI 0x20 // 向PIC发送结束中断命令


gate_t idt[IDT_SIZE];
pointer_t idt_ptr;


handler_t handler_table[IDT_SIZE];   // 保存中断处理函数的数组

extern handler_t handler_entry_table[ENTRY_SIZE];    // 保存中断入口函数地址的数组 (跳转到宏的地址执行) IDT

/*
中断 0x0D 触发
      ↓
CPU 查 IDT → 跳转到 handler_entry_table[0x0D] = interrupt_handler_0x0D
      ↓
interrupt_handler_0x0D:
    push 0x0D
    jmp interrupt_entry
      ↓
interrupt_entry:
    eax = [esp] = 0x0D
    call handler_table[0x0D]
      ↓
执行 general_protection_handler()
*/


// * func
static void idt_init();
static void pic_init();
static void default_handler(int vector);
static void exception_handler(int vector, u32 edi, u32 esi, u32 ebp, u32 esp, u32 ebx, u32 edx, u32 ecx, u32 eax, u32 gs, u32 fs, u32 es, u32 ds, u32 vector0, u32 error, u32 eip, u32 cs, u32 eflags);

// error messages
static char *messages[] = {
    "#DE Divide Error\0",
    "#DB RESERVED\0",
    "--  NMI Interrupt\0",
    "#BP Breakpoint\0",
    "#OF Overflow\0",
    "#BR BOUND Range Exceeded\0",
    "#UD Invalid Opcode (Undefined Opcode)\0",
    "#NM Device Not Available (No Math Coprocessor)\0",
    "#DF Double Fault\0",
    "    Coprocessor Segment Overrun (reserved)\0",
    "#TS Invalid TSS\0",
    "#NP Segment Not Present\0",
    "#SS Stack-Segment Fault\0",
    "#GP General Protection\0",
    "#PF Page Fault\0",
    "--  (Intel reserved. Do not use.)\0",
    "#MF x87 FPU Floating-Point Error (Math Fault)\0",
    "#AC Alignment Check\0",
    "#MC Machine Check\0",
    "#XF SIMD Floating-Point Exception\0",
    "#VE Virtualization Exception\0",
    "#CP Control Protection Exception\0",
};


// 通知中断控制器，结束当前中断
void send_eoi(int vector) {
    if (vector >= 0x28 && vector < 0x30) {
        outb(PIC_M_CTRL, PIC_EOI);  // 从 PIC
        outb(PIC_S_CTRL, PIC_EOI);
    }

    if (vector >= 0x20 && vector < 0x28)
        outb(PIC_M_CTRL, PIC_EOI);  // 主 PIC
    
}


void set_interrupt_handler(u32 irq, handler_t handler) {
    assert(irq >= 0 && irq < 16);
    // 对应中断号的处理函数
    handler_table[IRQ_MASTER_NR + irq] = handler;
}


void set_interrupt_mask(u32 irq, bool enable) {
    assert(irq >= 0 && irq < 16);
    u16 port;

    if (irq < 8) {
        port = PIC_M_DATA;
    } else {
        port = PIC_S_DATA;
        irq -= 8;
    }

    if (enable) {
        outb(port, inb(port) & ~(1 << irq));
    } else {
        outb(port, inb(port) | (1 << irq));
    }
}


u32 counter = 0;


static void default_handler(int vector) {
    send_eoi(vector);
    // schedule();
    DEBUGK("[%dx] default interrupt called %d...\n", vector, counter);
}


static void exception_handler(int vector, u32 edi, u32 esi, u32 ebp, u32 esp, u32 ebx, u32 edx, u32 ecx, u32 eax, u32 gs, u32 fs, u32 es, u32 ds, u32 vector0, u32 error, u32 eip, u32 cs, u32 eflags) {
    char *msg = NULL;
    if (vector < 22)
        msg = messages[vector];
    else
        msg = messages[15];

    printk("\nException: %s\n", messages[vector]);

    printk("    VECTOR   : 0x%02X\n", vector);
    printk("    ERROR    : 0x%08X\n", error);
    printk("    EFLAGS   : 0x%08X\n", eflags);
    printk("    CS       : 0x%02X\n", cs);
    printk("    EIP      : 0x%08X\n", eip);
    printk("    ESP      : 0x%08X\n", esp);

    // while(1);
    hang();
}


static void idt_init() {
    for (size_t i = 0; i < ENTRY_SIZE; i++) {
        gate_t *gate = &idt[i];

        // 查表找到中断入口函数地址
        handler_t handler = handler_entry_table[i];

        gate->offset0 = ((u32)handler & 0xFFFF);
        gate->offset1 = ((u32)handler >> 16) & 0xFFFF;

        gate->selector = 1 << 3; // 代码段
        gate->reserved = 0;     // 保留
        gate->type = 0b1110;    // 中断门
        gate->segment = 0;      // 系统段
        gate->DPL = 0;          // 内核态
        gate->present = 1;      // 有效
    }

    // 指定异常处理函数
    for (size_t i = 0; i < 0x20; i++) {
        handler_table[i] = exception_handler;
    }

    // 指定外中断处理函数
    for (size_t i = 0x20; i < ENTRY_SIZE; i++) {
        handler_table[i] = default_handler;
    }

    idt_ptr.base = (u32)idt;
    idt_ptr.limit = (u16)(sizeof(idt) - 1);
    asm volatile("lidt idt_ptr\n");
}


static void pic_init() {
    // 初始化主 PIC
    outb(PIC_M_CTRL, 0b00010001); // ICW1: 边沿触发, 级联 8259, 需要ICW4.
    outb(PIC_M_DATA, 0x20);       // ICW2: 起始端口号 0x20
    outb(PIC_M_DATA, 0b00000100); // ICW3: IR2接从片.
    outb(PIC_M_DATA, 0b00000001); // ICW4: 8086模式, 正常EOI

    outb(PIC_S_CTRL, 0b00010001); // ICW1: 边沿触发, 级联 8259, 需要ICW4.
    outb(PIC_S_DATA, 0x28);       // ICW2: 起始端口号 0x28
    outb(PIC_S_DATA, 2);          // ICW3: 设置从片连接到主片的 IR2 引脚
    outb(PIC_S_DATA, 0b00000001); // ICW4: 8086模式, 正常EOI

    outb(PIC_M_DATA, 0b11111111); // 关闭所有中断
    outb(PIC_S_DATA, 0b11111111); // 关闭所有中断
}


void interrupt_init() {
    pic_init();
    idt_init();
}