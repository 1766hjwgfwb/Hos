#include <hos/types.h>
#include <stdio.h>


// * 8byte * 
typedef struct descriptor {
    unsigned short limit_low;       // 段界限低16位
    unsigned int base_low : 24;   // 段基地址低24位（0~23）
    unsigned char type : 4;        // 段类型（代码/数据）
    unsigned char segment : 1;     // 段类别（1 = 普通段，0 = 系统段）
    unsigned char DPL : 2;         // 描述符特权等级（0~3）
    unsigned char present : 1;     // 是否存在于内存
    unsigned char limit_high : 4;  // 段界限高4位（共20位）
    unsigned char available : 1;   // 操作系统保留
    unsigned char long_mode : 1;   // 64位代码段标志（x64专用）
    unsigned char big : 1;         // 默认操作数大小（1=32位，0=16位）
    unsigned char granularity : 1; // 粒度（1=4KB，0=1字节）
    unsigned char base_high;       // 段基地址高8位（24~31）
} _packed descriptor;


int main() {
    printf("size of u8 %d\n", sizeof(u8));
    printf("size of u16 %d\n", sizeof(u16));
    printf("size of u32 %d\n", sizeof(u32));
    printf("size of u64 %d\n", sizeof(u64));
    printf("size of descriptor %d\n", sizeof(descriptor));
    return 0;
}