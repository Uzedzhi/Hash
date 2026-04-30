#include <immintrin.h>

extern "C" int MyStrcmp(const char *str1, const char *str2) noexcept {
    __m256i ymm0 = _mm256_loadu_si256((const __m256i*) str1);
    __m256i ymm1 = _mm256_loadu_si256((const __m256i*) str2);
    __m256i ymm2 = _mm256_setzero_si256();

    ymm1 = _mm256_cmpeq_epi8(ymm0, ymm1);
    ymm2 = _mm256_cmpeq_epi8(ymm0, ymm2);

    unsigned int ecx = _mm256_movemask_epi8(ymm1);
    unsigned int edx = _mm256_movemask_epi8(ymm2);

    ecx = ~ecx;

    int rax = 0;
    if (ecx == 0)
        return rax;

    int bsf_ecx = __builtin_ctz(ecx);
    int bsf_edx = __builtin_ctz(edx);

    if (bsf_ecx <= bsf_edx)
        rax = 1;

    return rax;
}