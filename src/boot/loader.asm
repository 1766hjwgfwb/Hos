[org 0x1000]

dw 0x55aa ; 写入魔数


; 初始化栈, 消除实模式下栈的影响
mov ax, 0
mov ss, ax
mov ds, ax
mov es, ax
mov fs, ax
mov gs, ax
mov sp, 0x7c00 ; 栈顶指针


; print string
mov si, loading
call print

; xchg bx, bx


detect_memory:
    ; ebx set zero
    xor ebx, ebx

    ; es:di 结构体的缓存位置
    mov ax, 0
    mov es, ax
    mov edi, ards_buffer
    
    mov edx, 0x534d4150 ; 'SMAP'


.next:
    ; 子功能号
    mov eax, 0xe820
    ; ards 结构的大小
    mov ecx, 20
    ; sys call
    int 0x15

    ; if CF set, error
    jc error

    ; 将缓存指针指向下一个结构体
    add di, cx

    ; struct count +1
    inc word[ards_count]

    cmp ebx, 0
    jnz .next

    mov si, detecting
    call print

    xchg bx, bx

    ; 在实模式下无法修改
    ; mov byte [0xb8000], 'P'

    jmp prepare_protected_mode


prepare_protected_mode:
    xchg bx, bx

    cli ; 关闭中断

    ; 打开 A20 端口
    in al, 0x92
    or al, 0b10
    out 0x92, al

    ; 加载 GDT
    lgdt [gdt_ptr]

    ; 进入保护模式
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    ; 用跳转指令来刷新缓存
    jmp dword code_selector:protect_mode


print:
    mov ah, 0x0e
.next:
    mov al, [si]
    cmp al, 0
    jz .done
    int 0x10
    inc si
    jmp .next
.done:
    ret

loading:
    db "Loading Hos...", 10, 13, 0; \n\r
detecting:
    db "Detecting memory suceessful!", 10, 13, 0; \n\r



error:
    mov si, .msg
    call print
    hlt ; 死机
    jmp $
    .msg db "Booting error!", 10, 13, 0; \n\r



[bits 32]
protect_mode:
    xchg bx, bx

    ; 初始化段寄存器
    mov ax, data_selector
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov esp, 0x10000 ; 设置栈顶

    mov edi, 0x10000 ; 内存地址
    mov ecx, 10 ; 起始扇区
    mov bl, 200 ; 扇区数量

    call read_disk ; 读取硬盘

    jmp dword code_selector:0x10000 ; 跳转到内核入口

    ud2 ; 未定义指令

    
jmp $ ; 阻塞


read_disk:

    ; 设置读写扇区的数量
    mov dx, 0x1f2;
    mov al, bl
    out dx, al

    inc dx ; 0x1f3
    mov al, cl ; 起始扇区的前8位
    out dx, al

    inc dx ; 0x1f4
    shr ecx, 8
    mov al, cl ; 起始扇区的中8位
    out dx, al

    inc dx ; 0x1f5
    shr ecx, 8
    mov al, cl ; 起始扇区的高8位
    out dx, al

    inc dx ; 0x1f6
    shr ecx, 8
    and cl, 0b1111 ;将高四位置0

    mov al, 0b1110_0000
    or al, cl
    out dx, al ; 主盘 - LBA模式

    inc dx ; 0x1f7
    mov al, 0x20 ; 读硬盘
    out dx, al

    xor ecx, ecx ; 清空ecx
    mov cl, bl ; 扇区数量

    .read:
        push cx ; 保存 cx
        call .waits ; 等待数据准备完毕
        call .reads ; 读取一个扇区
        pop cx ; 恢复 cx
        loop .read

    ret

    .waits:
        mov dx, 0x1f7
        .check:
            in al, dx
            jmp $+2; nop 跳转到下一行
            jmp $+2; 延迟
            jmp $+2
            and al, 0b1000_1000
            cmp al, 0b0000_1000
            jnz .check ; 等待准备完毕
        ret

    .reads:
        mov dx, 0x1f0 ; 选择主盘
        mov cx, 256 ; 一个扇区 256字(512字节)
        .readw:
            in ax, dx
            jmp $+2; nop 跳转到下一行
            jmp $+2; 延迟
            jmp $+2
            mov [edi], ax ; 读入内存
            add edi, 2 ; 指向下一个字
            loop .readw
        ret


; GDT 8字节偏移量 index 0 NULLptr 1 code_selector 2 data_selector
code_selector equ (1 << 3)
data_selector equ (2 << 3)


memory_base equ 0 ; 内存开始的位置 基地址


; 1024 * 1024 * 1024 * 4 = 4GB
; 1024 * 4 = 4KB
; 4GB / 4KB = 1024 * 1024 * 1024 - 1
; 减一是因为基地址从 0 开始
; 段大小 = 最大偏移 (memory_limit + 1)
; 如果G = 1， 段大小 = (memory_limit + 1) << 12
memory_limit equ ((1024 * 1024 * 1024 * 4) / (1024 * 4)) - 1

gdt_ptr:
    dw (gdt_end - gdt_base) - 1
    dd gdt_base

gdt_base:
    dd 0, 0; null descriptor

; gdt_code and gdt_data are segment descriptors
gdt_code:
    dw memory_limit & 0xffff ; 段界限 0 ~ 15 位
    dw memory_base & 0xffff ; 基地址 0 ~ 15 位    
    db (memory_base >> 16) & 0xff; 基地址 16 ~ 23 位

    ; 存在内存 - dlp 0(Ring 0) - 代码段(非系统段) - 可执行 - 非一致性(非可依从) - 可读 - CPU未访问
    db 0b_1_00_1_1_0_1_0

    db 0b1_1_0_0_0000 | (memory_limit >> 16) & 0xf
    db (memory_base >> 24) & 0xff; 基地址 24 ~ 31 位

gdt_data:
    dw memory_limit & 0xffff ; 段界限 0 ~ 15 位
    dw memory_base & 0xffff ; 基地址 0 ~ 15 位    
    db (memory_base >> 16) & 0xff; 基地址 16 ~ 23 位

    ; 存在内存 - dlp 0(Ring 0) - 数据段(非系统段) - 向上 - 可写 - 没有被访问过
    db 0b_1_00_1_0_0_1_0
    db 0b1_1_0_0_0000 | (memory_limit >> 16) & 0xf
    db (memory_base >> 24) & 0xff; 基地址 24 ~ 31 位
; 记录地址 gdt_data
gdt_end:

ards_count:
    dw 0

ards_buffer:
