extern "C" int __attribute__((always_inline)) MyStrcmp(const char *str1, const char *str2) noexcept {
    int cmp;
    __asm (
        ".intel_syntax noprefix\n"
        
        "vpxor ymm2, ymm2, ymm2\n"
        "vmovdqu ymm0, [%[str1]]\n"

        "vpcmpeqb ymm1, ymm0, [%[str2]]\n"
        "vpcmpeqb ymm2, ymm0, ymm2\n"

        "vpmovmskb edx, ymm1\n"
        "vpmovmskb ecx, ymm2\n"

        "not edx\n"
        "lea eax, [ecx - 1]\n"
        "xor ecx, eax\n"

        "test ecx, edx\n"
        "setnz al\n"
        "movzx %[result], al\n"

        "vzeroupper\n"
        ".att_syntax prefix\n"
        : [result] "=r" (cmp)
        : [str1] "r" (str1), [str2] "r" (str2)
        : "eax", "ecx", "edx", "ymm0", "ymm1", "ymm2", "cc"
    );

    return cmp;
}