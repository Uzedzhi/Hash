section .text
global MySDBM_Hash

MySDBM_Hash:
    xor eax, eax
    .Loop:
        movzx rcx, byte [rdi]

        test rcx, rcx
        je .LoopExit

        mov edx, eax
        mov esi, eax

        shl edx, 6
        shl esi, 16
        add edx, esi
        sub edx, eax
        add edx, ecx
        mov eax, edx
        inc rdi
        jmp .Loop
    .LoopExit:
    ret
