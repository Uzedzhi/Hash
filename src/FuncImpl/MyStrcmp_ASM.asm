section .text
global MyStrcmp

MyStrcmp:
    vmovdqu ymm0, [rdi]         ; ymm0 = str1
    vpxor ymm2, ymm2, ymm2      ; ymm2 = 0
    vmovdqu ymm1, [rsi]         ; ymm1 = str2

    vpcmpeqb ymm1, ymm0, ymm1   ; ymm1: 0xFF if str1[i] == str2[i]
    vpcmpeqb ymm2, ymm0, ymm2   ; ymm2: 0xFF if str1[i] == 0
    vpmovmskb ecx, ymm1         ; ecx[i] = 1 if str1[i] == str2[i]
    vpmovmskb edx, ymm2         ; edx[i] = 1 if str1[i] == 0

    not ecx                     ; ecx[i] = 1 if str1[i] != str2[i]

    xor eax, eax
    test ecx, ecx
    jz .Exit

    bsf ecx, ecx
    bsf edx, edx
    
    mov esi, 1
    cmp ecx, edx
    cmovbe eax, esi
    .Exit:
    vzeroupper
    ret
