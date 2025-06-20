[bits 32]

global task_switch


task_switch:
    push ebp
    mov ebp, esp

    push ebx
    push esi
    push edi

    mov eax, esp
    and eax, 0xfffff000

    mov [eax], esp

    mov eax, [ebp+8]
    mov esp, [eax]

    pop edi
    pop esi
    pop ebx
    pop ebp     ;esp += 4     // 此时esp指向栈顶（函数地址）
    
    ret