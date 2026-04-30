extern "C" unsigned int MyCRC32_Hash(const char *str) noexcept {
    unsigned int hash = 0xFFFFFFFF;
    const char *ptr = str;
    __asm (
        ".intel_syntax noprefix\n"
        "xor ecx, ecx\n"

        ".Loop:\n"
        "   mov cl, [%[ptr]]\n"
        "   test cl, cl\n"
        "   jz .Exit\n"

        "   crc32 %[hash], cl\n"
        "   inc %[ptr]\n"
        "   jmp .Loop\n"

        ".Exit:\n"
        "xor %[hash], 0xFFFFFFFF\n"
        ".att_syntax prefix\n"
        : [hash] "+r" (hash), [ptr] "+r" (ptr)
        : 
        : "rcx", "cc"
    );
    return hash;
}