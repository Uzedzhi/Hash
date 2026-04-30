extern "C" int MyStrcmp(const char *str1, const char *str2) noexcept {
    int cmp;
    __asm (
        ".intel_syntax noprefix\n"
        
        "vpxor ymm2, ymm2, ymm2\n"
        "vmovdqu ymm0, [%[str1]]\n"

        "vpcmpeqb ymm1, ymm0, [%[str2]]\n"
        "vpcmpeqb ymm2, ymm0, ymm2\n"

        "vpmovmskb ecx, ymm1\n"
        "vpmovmskb edx, ymm2\n"

        "not ecx\n"

        "xor %[result], %[result]\n"
        "test ecx, ecx\n"
        "jz .Exit\n"
        
        "bsf ecx, ecx\n"
        "bsf edx, edx\n"

        "mov esi, 1\n"

        "cmp ecx, edx\n"
        "cmovbe %[result], esi\n"

        ".Exit:\n"
        "vzeroupper\n"
        ".att_syntax prefix\n"
        : [result] "=r" (cmp)
        : [str1] "r" (str1), [str2] "r" (str2)
        : "eax", "ecx", "edx", "ymm0", "ymm1", "ymm2", "cc"
    );

    return cmp;
}