[org 0x1000]

dw 0x55aa ; 写入魔数


; print string
mov si, loading
call print

xchg bx, bx


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

    ; 结构体数量
    mov cx, word[ards_count]
    ; ponit
    mov si, 0

.show:
    mov eax, [ards_buffer + si]
    mov ebx, [ards_buffer + si + 8]
    mov edx, [ards_buffer + si + 16]
    add si, 20
    xchg bx, bx
    loop .show  ; 不同机器会有差异

; 阻塞
jmp $

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


ards_count:
    dw 0
ards_buffer:
