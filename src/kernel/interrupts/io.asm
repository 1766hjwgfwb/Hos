[bits 32]

section .text

global inb
inb:
    push ebp
    mov ebp, esp; save ebp

    xor eax, eax; clear eax
    mov edx, [ebp + 8]; portcv              
    in al, dx; 将端口号 dx 的8 bit输入到ax

    jmp $+2;
    jmp $+2;
    jmp $+2;

    leave; restore ebp
    ret

global inw
inw:
    push ebp
    mov ebp, esp; save ebp

    xor eax, eax; clear eax
    mov edx, [ebp + 8]; portcv              
    in ax, dx; 将端口号 dx 的8 bit输入到ax

    jmp $+2;
    jmp $+2;
    jmp $+2;

    leave; restore ebp
    ret
    

global outb
outb:
    push ebp
    mov ebp, esp; save ebp

    mov edx, [ebp + 8]; portcv
    mov eax, [ebp + 12]; data
    out dx, al; 将 al 中的 8bit 输入到 端口号dx

    jmp $+2;
    jmp $+2;
    jmp $+2;

    leave; restore ebp
    ret 


global outw
outw:
    push ebp
    mov ebp, esp; save ebp

    mov edx, [ebp + 8]; portcv
    mov eax, [ebp + 12]; data
    out dx, ax; 将 ax 中的 16bit 输入到 端口号dx

    jmp $+2;
    jmp $+2;
    jmp $+2;

    leave; restore ebp
    ret 