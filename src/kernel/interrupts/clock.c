#include <hos/io.h>
#include <hos/interrupt.h>
#include <hos/debug.h>
#include <common/assert.h>

#define PIT_CHAN0_REG 0x40
#define PIT_CHAN2_REG 0x42
#define PIT_CTRL_REG 0x43

#define HZ 100
#define OSCILLATOR 1193182
#define CLOCK_COUNTER (OSCILLATOR / HZ)
#define JIFFY (1000 / HZ)

// 时间片计数器
u32 volatile jiffies = 0;
u32 jiffy = JIFFY;


// 时钟中断处理函数
void clock_handler(int vector) {
    assert(vector == 0x20);
    send_eoi(vector);

    jiffies++;
    DEBUGK("clock jiffies: %d ...\n", jiffies);
}


void pit_init() {
    outb(PIT_CTRL_REG, 0b00110100);
    outb(PIT_CHAN0_REG, CLOCK_COUNTER & 0xff);
    outb(PIT_CHAN2_REG, (CLOCK_COUNTER >> 8) & 0xff);
}


void clock_init() {
    pit_init();
    set_interrupt_handler(IRQ_CLOCK, clock_handler);
    set_interrupt_mask(IRQ_CLOCK, true);
}
