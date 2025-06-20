[org 0x7c00]


; 设置屏幕模式为文本模式
; ax(16bit) = ah + al(contrl) 
mov ax, 3
int 0x10

; 初始化段寄存器
mov ax, 0
mov ds, ax
mov es, ax
mov ss, ax
mov fs, ax
mov gs, ax
mov sp, 0x7c00



mov si, booting
call print




; jmp error test


mov edi, 0x1000 ; 内存地址
mov ecx, 2 ; 起始扇区
mov bl, 4 ; 扇区数量

call read_disk ; 读取硬盘

; xchg bx, bx ;　断点


; 比较地址魔数
cmp word [0x1000], 0x55aa
jnz error ; 地址魔数不匹配


jmp 0:0x1002 ; 跳转到内核入口



; 阻塞
jmp $


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



error:
    mov si, .msg
    call print
    hlt ; 死机
    jmp $
    .msg db "Booting error!", 10, 13, 0; \n\r



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

booting:
    db "Booting Hos...", 10, 13, 0; \n\r


; 填充0
times 510 - ($ - $$) db 0

db 0x55, 0xaa