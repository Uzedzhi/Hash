#include <immintrin.h>

extern "C" unsigned int __attribute__((always_inline)) MyCRC32_Hash(const char *str) {
    unsigned int hash = 0xFFFFFFFF;

    while (*str && *(str + 1) && *(str + 2) && *(str + 3)) {
        hash = _mm_crc32_u32(hash, *((unsigned int *) str));
        str += 4;
    }

    while (*str) {
        hash = _mm_crc32_u8(hash, *str);
        str++;
    }
    
    return hash ^ 0xFFFFFFFF; // Финальный XOR (стандарт CRC32)
}