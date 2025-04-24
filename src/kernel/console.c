#include <common/console.h>
#include <hos/io.h>
#include <lib/string.h>


static u32 screen; // 当前显示器开始的内存位置

static u32 pos; // 记录当前光标的内存位置

static u8 x, y; // 当前光标的坐标

static u8 attr = 7;        // 字符样式
static u16 erase = 0x0720; // 空格

// * Information functions
static void get_screen();
static void set_screen();
static void get_cursor();
static void set_cursor();

// * command functions
static void command_bs();
static void command_cr();
static void command_lf();
static void command_ht();
static void scroll_up();
static void update_position();


// * read char from screen memory
static void get_screen() {
    outb(CRT_ADDR_REG, CRT_START_ADDR_H);   // set high 8bits
    screen = inb(CRT_DATA_REG) << 8; // get high 8bits
    outb(CRT_ADDR_REG, CRT_START_ADDR_L);   // set low 8bits
    screen |= inb(CRT_DATA_REG); // get low 8bits

    screen <<=1; // screen * 2, 1 char = 2 bytes
    screen += MEM_BASE; // screen + MEM_BASE
}


static void set_screen() {
    outb(CRT_ADDR_REG, CRT_START_ADDR_H);   // set high 8bits
    // * (>> 9) = (>> 1 >> 8)
    // screen - mem_base = 字节偏移量
    // (screen - mem_base) / 2 = 字符偏移量
    outb(CRT_DATA_REG, ((screen - MEM_BASE) >> 9) & 0xff); // set high 8bits
    outb(CRT_ADDR_REG, CRT_START_ADDR_L);   // set low 8bits
    outb(CRT_DATA_REG, ((screen - MEM_BASE) >> 1) & 0xff); // set low 8bits

    // todoing
    // * if (x != 0), eax return != 0
}


static void get_cursor() {
    outb(CRT_ADDR_REG, CRT_CURSOR_H);
    pos = inb(CRT_DATA_REG) << 8; // get high 8bits
    outb(CRT_ADDR_REG, CRT_CURSOR_L);
    pos |= inb(CRT_DATA_REG); // get low 8bits

    get_screen();

    pos <<= 1; // pos * 2, 1 char = 2 bytes
    pos += MEM_BASE; // pos + mem_base

    u32 offset = (pos - screen) >> 1; // 计算偏移量

    // * math is belike: offset = y * width + x, so we can / y, % x
    x = offset % WIDTH;
    y = offset / WIDTH;
}


static void set_cursor() {
    u32 offset = y * WIDTH + x; // 计算偏移量

    u32 screen_offset = (screen - MEM_BASE) >> 1; // 计算屏幕偏移量

    u32 cursor_pos = screen_offset + offset; // 计算光标位置

    outb(CRT_ADDR_REG, CRT_CURSOR_H);   // set high 8bits
    outb(CRT_DATA_REG, (cursor_pos >> 8) & 0xff); // set high 8bits
    outb(CRT_ADDR_REG, CRT_CURSOR_L);   // set low 8bits
    outb(CRT_DATA_REG, cursor_pos & 0xff); // set low 8bits
}


// synchronize x,y and pos
static void update_position() {
    pos = screen + (y * WIDTH + x) * BYTES_PER_CHAR;    // 计算光标位置

    // 保持 pos 在显示器内
    if (pos < MEM_BASE || pos >= MEM_BASE + (HEIGHT * WIDTH * BYTES_PER_CHAR)) {
        x = y = 0;
        pos = screen;
    }
}


void console_init() {
    console_clear();
}


void console_clear() {
    screen = MEM_BASE;
    pos = MEM_BASE;
    x = 0, y = 0;

    u16 *ptr = (u16 *) screen;

    u32 _screen_Size = WIDTH * HEIGHT;

    for (u32 i = 0; i < _screen_Size; i++) {
        *ptr++ = erase; // 空格
    }

    set_cursor();
    set_screen();
}


static void command_bs() {
    // delete char

    if (x > 0) {
        x--;
        pos -= BYTES_PER_CHAR; // 2 bytes per char
        *(u16 *)pos = erase; // 强转为u16，空格
    }
}


static void command_del() {
    *(u16 *)pos = erase; // 强转为u16，空格
}


static void command_cr() {
    x = 0;
    // pos = screen + (y * WIDTH * 2); // 回到行首
    update_position();
}


static void command_lf() {
    if (y < HEIGHT - 1) {
        y++;
        x = 0;
        // pos = screen + (y * WIDTH * 2); // 换行
        update_position();
        return;
    }

    // scroll up
    scroll_up();
}


void command_ht() {
    // * tab
    x = (x + 8) & ~7; // 8 = 8 - 1, 8的整数倍

    if (x >= WIDTH) {
        x = 0;
        y++;
    }

    update_position();
}


void scroll_up() {
    // 将屏幕内容上移一行（从第二行开始复制到第一行）
    u16 *dest = (u16 *)screen;
    u16 *src = (u16 *)(screen + ROW_SIZE); // ROW_SIZE = WIDTH * 2
    /*  for (u32 i = 0; i < (HEIGHT - 1) * WIDTH; i++) {
        *dest++ = *src++;
    }*/
    memcpy(dest, src, (HEIGHT - 1) * WIDTH * BYTES_PER_CHAR); // 直接复制

    // 清空最后一行
    u16 *last_line = (u16 *)(screen + (HEIGHT - 1) * ROW_SIZE);
    for (u32 i = 0; i < WIDTH; i++) {
        *last_line++ = erase; // 填充空格
    }

    // 保持 screen 不变，通过硬件寄存器调整显示起始位置
    // 计算新的屏幕偏移量（上移一行）
    u32 screen_offset = ((screen - MEM_BASE) >> 1) + WIDTH; // 偏移量增加一行
    screen = MEM_BASE; // 始终保持 screen 为 MEM_BASE

    // 更新 pos，保持在最后一行
    y = HEIGHT - 1;
    x = 0;
    // pos = screen + (y * WIDTH * 2);
    update_position();

    // 更新硬件寄存器
    outb(CRT_ADDR_REG, CRT_START_ADDR_H);
    outb(CRT_DATA_REG, (screen_offset >> 8) & 0xff); // 高 8 位
    outb(CRT_ADDR_REG, CRT_START_ADDR_L);
    outb(CRT_DATA_REG, screen_offset & 0xff); // 低 8 位

    set_cursor();
}


void console_write(char *buf, u32 count) {
    char ch;

    while (count--) {
        ch = *buf++;
        char *ptr = (char *)pos;

        switch (ch) {
            case ASCII_NUL: // null 0x0
                break;
            case ASCII_ENQ: // enquiry 0x5
                break;
            case ASCII_BEL: // bell 0x7
                break;
            case ASCII_BS: // backspace 0x8
                command_bs();
                break;
            case ASCII_HT: // horizontal tab 0x9
                command_ht();
                break;
            case ASCII_LF: // line feed 0xa
                command_lf();
                break;
            case ASCII_VT: // vertical tab 0xb
                command_lf();
                break;
            case ASCII_FF: // form feed 0xc
                console_clear();
                break;
            case ASCII_CR: // carriage return 0xd
                command_cr();
                break;
            case ASCII_DEL: // delete 0x7f
                command_del();
                break;

            default:
                if (x >= WIDTH) {
                    x = 0;
                    y++;
                    // pos = screen + (y * WIDTH * 2); // 修正：直接计算新行的位置
                }

                if (y >= HEIGHT) {
                    scroll_up();
                    y = HEIGHT - 1;
                    // pos = screen + (y * WIDTH * 2);
                    x = 0;
                }

                *ptr++ = ch;    // 写入字符
                *ptr++ = attr;  // 写入属性

                x++;
                // pos += 2; // 2 bytes per char
                pos += BYTES_PER_CHAR; // 2 bytes per char
                update_position();

                break;
        }
    }
    set_cursor();
}