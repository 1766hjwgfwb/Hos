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
mov sp, 0x7c00


; 设置显存段
mov ax, 0xb800
mov es, ax

mov si, msg
xor di, di
mov ah, 0x07


print_loop:
    lodsb
    test al, al
    jz halt
    stosw
    jmp print_loop

halt:
    jmp $


msg db 'Hello, world!', 0


times 510 - ($ - $$) db 0

db 0x55, 0xaa