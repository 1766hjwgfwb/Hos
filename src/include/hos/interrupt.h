#ifndef HOS_INTERRUPT_H
#define HOS_INTERRUPT_H


#include <hos/types.h>

#define IDT_SIZE 256


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
void exception_handler(int vector);

void send_eoi(int vector);
void default_handler(int vector);








#endif // HOS_INTERRUPT_H