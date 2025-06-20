#ifndef HOS_INTERRUPT_H
#define HOS_INTERRUPT_H


#include <hos/types.h>

#define IDT_SIZE 256

#define IRQ_CLOCK 0 // 时钟中断
#define IRQ_KEYBOARD 1 // 键盘中断
#define IRQ_CASCADE 2 // 级联中断  8259 从片控制器
#define IRQ_SERIAL2 3 // 串行端口 2
#define IRQ_SERIAL1 4 // 串行端口 1
#define IRQ_PARALLEL2 5 // 并行端口 2
#define IRQ_FLOPPY 6 // 软驱
#define IRQ_PARALLEL1 7 // 并行端口 1
#define IRQ_RTC 8 // 实时时钟
#define IRQ_REDIRECT 9 // 重定向 IRQ2
#define IRQ_MOUSE 12 // 鼠标
#define IRQ_MATH 13 // 协处理器 x87
#define IRQ_HARDDISK 14 // ATA 磁盘第一通道
#define IRQ_HARDDISK2 15 // ATA 磁盘第二通道

#define IRQ_MASTER_NR 0x20  // 主片起始向量号
#define IRQ_SLAVE_NR 0x28   // 从片起始向量号


typedef struct {
    u16 offset0;    // 段内偏移 0 - 15位
    u16 selector;   // 选择子
    u8 reserved;    // 保留
    u8 type : 4 ;        // 任务门 / 中断门 / 陷阱门
    u8 segment : 1;     // segment = 0 系统段
    u8 DPL : 2 ;         // 特权级 int
    u8 present : 1;     // 有效位
    u16 offset1;    // 段内偏移 16 - 31位
}_packed gate_t;


typedef void *handler_t;    // handler func


void interrupt_init();
void send_eoi(int vector);

// interrupt handler function
void set_interrupt_handler(u32 irq, handler_t handler);
void set_interrupt_mask(u32 irq, bool enable);






#endif // HOS_INTERRUPT_H