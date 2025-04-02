[org 0x1000]

dw 0x55aa ; 判断错误


; print string
mov si, loading
call print

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