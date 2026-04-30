section .text
global MyCRC32_Hash

MyCRC32_Hash:
    mov eax, 0xFFFFFFFF
    xor ecx, ecx

    .Loop:
        mov cl, [rdi]
        test cl, cl
        jz .Exit

        crc32 eax, cl
        inc rdi
        jmp .Loop

    .Exit:
    xor eax, 0xFFFFFFFF
    ret
