section .text
global MyStrlen_Hash

; str address in rdi
MyStrlen_Hash:
    vpxor ymm1, ymm1, ymm1

    vmovdqu ymm0, [rdi] ; ymm0 - 32 bytes from the word
    vpcmpeqb ymm0, ymm0, ymm1 ; 
    vpmovmskb eax, ymm0

    bsf eax, eax
    vzeroupper
    ret