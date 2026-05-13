section .text
global MyFNV1A_Hash

MyFNV1A_Hash:
    mov eax, 2166136261d

    .Loop:
        movzx ecx, byte [rdi]

        test ecx, ecx
        je .ExitLoop

        xor eax, ecx
        imul eax, eax, 16777619
        inc rdi

        jmp .Loop
    .ExitLoop:
    ret
