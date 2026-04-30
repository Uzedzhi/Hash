#include <immintrin.h>

extern "C" unsigned int MyCRC32_Hash(const char *str) {
    unsigned int hash = 0xFFFFFFFF;

    while (*str) {
        hash = _mm_crc32_u8(hash, *str);
        str++;
    }
    
    return hash ^ 0xFFFFFFFF; // Финальный XOR (стандарт CRC32)
}