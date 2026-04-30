#include <immintrin.h>

extern "C" unsigned int MyStrlen_Hash(const char *str1) noexcept {
    __m256i ymm1 = _mm256_setzero_si256();
    __m256i ymm0 = _mm256_loadu_si256((const __m256i*)str1);
    ymm0 = _mm256_cmpeq_epi8(ymm0, ymm1);
    
    int mask = _mm256_movemask_epi8(ymm0);
    
    if (mask == 0)
        return 32;
    return __builtin_ctz(mask); 
}