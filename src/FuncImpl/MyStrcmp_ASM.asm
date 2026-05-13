section .text
global MyStrcmp

MyStrcmp:
    vmovdqu ymm0, [rdi]         ; ymm0 = str1
    vmovdqu ymm1, [rsi]         ; ymm1 = str2
    vpxor ymm2, ymm2, ymm2      ; ymm2 = 0

    vpcmpeqb ymm1, ymm0, ymm1   ; ymm1: 0xFF if str1[i] == str2[i]
    vpcmpeqb ymm2, ymm0, ymm2   ; ymm2: 0xFF if str1[i] == 0
    vpmovmskb edx, ymm1         ; edx[i] = 1 if str1[i] == str2[i]
    vpmovmskb ecx, ymm2         ; ecx[i] = 1 if str1[i] == 0

    not edx                     ; edx[i] = 1 if str1[i] != str2[i] (mismatches)
    lea eax, [ecx - 1]
    xor ecx, eax
    
    test ecx, edx
    setnz al
    movzx eax, al

    vzeroupper
    ret
