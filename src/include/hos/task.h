#ifndef __TASK_H__
#define __TASK_H__


#include <hos/types.h>


typedef u32 target_t(); // 入口地址

typedef struct {
    u32 *stack;     // kernel stack
}task_t;


typedef struct {    // stack frame of a task
    u32 edi;
    u32 esi;
    u32 ebp;
    u32 ebx;
    void (*eip)(void);
}task_frame_t;


void task_init();
void schedule();





#endif /* __TASK_H__ */