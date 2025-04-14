[bits 32]


extern kernel_init



global _start
_start:
    xchg cx, cx
    call kernel_init
    xchg cx, cx
    jmp $