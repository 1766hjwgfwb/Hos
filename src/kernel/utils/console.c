#include <common/console.h>
#include <hos/io.h>
#include <lib/string.h>

#define CRT_ADDR_REG 0x3D4   // CRT控制器索引寄存器
#define CRT_DATA_REG 0x3D5   // CRT控制器数据寄存器

// CRT控制器寄存器索引
#define CRT_START_ADDR_H 0xC // 显存起始地址高位
#define CRT_START_ADDR_L 0xD // 显存起始地址低位
#define CRT_CURSOR_H 0xE     // 光标位置高位
#define CRT_CURSOR_L 0xF     // 光标位置低位

// 显存相关常量
#define MEM_BASE 0xB8000     // 文本模式显存基地址
#define MEM_SIZE 0x4000      // 显存总大小 (16KB)
#define MEM_END (MEM_BASE + MEM_SIZE)
#define WIDTH 80             // 屏幕每行字符数
#define HEIGHT 25            // 屏幕总行数
#define ROW_SIZE (WIDTH * 2) // 每行字节数（字符+属性）
#define SCR_SIZE (ROW_SIZE * HEIGHT) // 整屏字节数

// 控制字符定义
#define ASCII_NUL 0x00
#define ASCII_ENQ 0x05
#define ASCII_BEL 0x07       // 蜂鸣符 \a
#define ASCII_BS 0x08        // 退格符 \b
#define ASCII_HT 0x09        // 水平制表符 \t
#define ASCII_LF 0x0A        // 换行符 \n
#define ASCII_VT 0x0B        // 垂直制表符 \v
#define ASCII_FF 0x0C        // 换页符 \f
#define ASCII_CR 0x0D        // 回车符 \r
#define ASCII_DEL 0x7F       // 删除符

static u32 screen;           // 当前显存起始位置
static u32 pos;              // 当前光标内存位置
static u32 x, y;             // 光标坐标（列，行）
static u8 attr = 0x07;       // 字符属性（灰底黑字）
static u16 erase = 0x0720;   // 擦除用空格（属性+字符）

/* 获取当前显存起始位置 */
static void get_screen() {
    outb(CRT_ADDR_REG, CRT_START_ADDR_H);
    screen = inb(CRT_DATA_REG) << 8;  // 读取高字节
    outb(CRT_ADDR_REG, CRT_START_ADDR_L);
    screen |= inb(CRT_DATA_REG);       // 组合低字节
    
    // 计算实际显存地址：CRT返回的是字符单元索引，需要转换为字节地址
    screen = MEM_BASE + (screen << 1); // 每个字符占2字节
}

/* 设置显存起始位置 */
static void set_screen() {
    u32 offset = (screen - MEM_BASE) >> 1; // 转换为字符单元
    
    outb(CRT_ADDR_REG, CRT_START_ADDR_H);
    outb(CRT_DATA_REG, (offset >> 8) & 0xFF);  // 写入高字节
    outb(CRT_ADDR_REG, CRT_START_ADDR_L);
    outb(CRT_DATA_REG, offset & 0xFF);         // 写入低字节
}

/* 获取光标位置 */
static void get_cursor() {
    outb(CRT_ADDR_REG, CRT_CURSOR_H);
    pos = inb(CRT_DATA_REG) << 8;     // 高字节
    outb(CRT_ADDR_REG, CRT_CURSOR_L);
    pos |= inb(CRT_DATA_REG);         // 低字节
    
    // 转换为字节地址并计算坐标
    pos = MEM_BASE + (pos << 1);
    get_screen();  // 确保screen值有效
    
    u32 delta = (pos - screen) >> 1;  // 转换为字符偏移
    x = delta % WIDTH;                // 列坐标
    y = delta / WIDTH;                // 行坐标
}

/* 设置光标位置 */
static void set_cursor() {
    u32 offset = (pos - MEM_BASE) >> 1; // 转换为字符单元
    
    outb(CRT_ADDR_REG, CRT_CURSOR_H);
    outb(CRT_DATA_REG, (offset >> 8) & 0xFF);
    outb(CRT_ADDR_REG, CRT_CURSOR_L);
    outb(CRT_DATA_REG, offset & 0xFF);
}

/* 清屏并重置光标 */
void console_clear() {
    screen = MEM_BASE;
    pos = MEM_BASE;
    x = y = 0;
    set_cursor();
    set_screen();

    // 用空格填充整个显存
    for (u16 *ptr = (u16 *)MEM_BASE; ptr < (u16 *)MEM_END; ptr++) {
        *ptr = erase;
    }
}

/* 向上滚动一行 */
static void scroll_up() {
    // 当显存即将溢出时，复位到基地址
    if (screen + SCR_SIZE + ROW_SIZE >= MEM_END) {
        memcpy((void *)MEM_BASE, (void *)screen, SCR_SIZE);
        pos -= (screen - MEM_BASE);  // 保持相对位置
        screen = MEM_BASE;
    }

    // 在底部添加新行（用空格填充）
    u16 *ptr = (u16 *)(screen + SCR_SIZE);
    for (size_t i = 0; i < WIDTH; i++) {
        *ptr++ = erase;
    }

    // 调整显存起始位置和光标
    screen += ROW_SIZE;
    pos += ROW_SIZE;
    set_screen();
}

/* 处理换行 */
static void command_lf() {
    if (y + 1 < HEIGHT) {  // 简单下移光标
        y++;
        pos += ROW_SIZE;
    } else {                // 需要滚屏
        scroll_up();
    }
}

/* 处理回车 */
static void command_cr() {
    pos = screen + y * ROW_SIZE;  // 回到行首
    x = 0;
}

/* 处理退格 */
static void command_bs() {
    if (x > 0) {
        x--;
        pos -= 2;
        *(u16 *)pos = erase;  // 用空格覆盖
    }
}

/* 处理删除 */
static void command_del() {
    *(u16 *)pos = erase;  // 删除当前字符
}

/* 控制台输出核心函数 */
void console_write(char *buf, u32 count) {
    while (count--) {
        char ch = *buf++;
        
        switch (ch) {
        case ASCII_NUL:           // 空字符
            break;
        case ASCII_BEL:           // 蜂鸣符（暂未实现）
            break;
        case ASCII_BS:            // 退格
            command_bs();
            break;
        case ASCII_HT:            // 制表符（暂未实现）
            break;
        case ASCII_LF:            // 换行
            command_lf();
            command_cr();         // 通常换行伴随回车
            break;
        case ASCII_VT:            // 垂直制表符
            break;
        case ASCII_FF:            // 换页（按换行处理）
            command_lf();
            break;
        case ASCII_CR:            // 回车
            command_cr();
            break;
        case ASCII_DEL:           // 删除
            command_del();
            break;
        default:                  // 可打印字符
            // 处理自动换行
            if (x >= WIDTH) {
                command_lf();
                command_cr();
            }
            
            // 写入字符及属性
            *((u8 *)pos) = ch;
            *((u8 *)pos + 1) = attr;
            pos += 2;
            x++;
            break;
        }
    }
    set_cursor();  // 更新光标位置
}

/* 控制台初始化 */
void console_init() {
    console_clear();
}