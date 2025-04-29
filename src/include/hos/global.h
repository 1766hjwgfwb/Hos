#ifndef HOS_GLOBAL_H
#define HOS_GLOBAL_H


#include <hos/types.h>


#define GDT_SIZE 128


// 全局描述符
typedef struct descriptor_t /* 共 8 个字节 */
{
    u16 limit_low;      // 段界限 0 ~ 15 位
    u32 base_low : 24;    // 基地址 0 ~ 23 位 16M
    u8 type : 4;        // 段类型
    u8 segment : 1;     // 1 表示代码段或数据段，0 表示系统段
    u8 DPL : 2;         // Descriptor Privilege Level 描述符特权等级 0 ~ 3
    u8 present : 1;     // 存在位，1 在内存中，0 在磁盘上
    u8 limit_high : 4;  // 段界限 16 ~ 19;
    u8 available : 1;   // 该安排的都安排了，送给操作系统吧
    u8 long_mode : 1;   // 64 位扩展标志
    u8 big : 1;         // 32 位 还是 16 位;
    u8 granularity : 1; // 粒度 4KB 或 1B
    u8 base_high;       // 基地址 24 ~ 31 位
} _packed descriptor_t;


// 段选择子
typedef struct selector_t {
    u8 RPL : 2;         // 请求特权级（0~3）
    u8 TI : 1;          // TI位（0=系统段，1=任务段）
    u16 index : 13;     // 段选择子索引（0~8191）
}selector_t;


// 全局描述符表指针
typedef struct pointer_t {
    u16 limit;
    u32 base;
}_packed pointer_t;


void gdt_init();


#endif /* HOS_GLOBAL_H */