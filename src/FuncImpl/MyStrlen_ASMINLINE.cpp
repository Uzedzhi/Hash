#include <stdlib.h>
extern "C" size_t __attribute__((always_inline)) MyStrlen(const char *str1) noexcept {
    unsigned int len = 1;
    __asm (
        ".intel_syntax noprefix\n"
        "vpxor ymm1, ymm1, ymm1\n"

        "vmovdqu ymm0, [%[str1]]\n"
        "vpcmpeqb ymm0, ymm0, ymm1\n"
        "vpmovmskb eax, ymm0\n"
        "bsf %[result], eax\n"

        "vzeroupper\n"
        ".att_syntax prefix\n"
        : [result] "=r" (len)
        : [str1]   "r"  (str1)
        : "rax", "ymm0", "ymm1", "cc"
    );

    return len;
}