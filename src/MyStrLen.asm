section .text
global MyStrlen

; str address in rdi
MyStrlen:
    vpxor ymm1, ymm1, ymm1

    vmovdqu ymm0, [rdi] ; ymm0 - 32 bytes from the word
    vpcmpeqb ymm0, ymm0, ymm1 ; 
    vpmovmskb eax, ymm0

    bsf eax, eax
    ret