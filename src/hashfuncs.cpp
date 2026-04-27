#include <immintrin.h>
#include <string.h>

#include "hashfuncs.h"
extern "C" size_t MyStrlen(const char *str);

unsigned int SDBM_Hash(const char * str) {
    unsigned int hash = 0;
    while (*str != '\0') {
        hash = *str + (hash << 6) + (hash << 16) - hash; // hash[i] = hash[i-1] * 65599 + str[i]
        str++;
    }
    return hash;
}

unsigned int CircLeftShift(unsigned int value, unsigned int shift) {
    return (value << shift) | (value >> (sizeof(value) * 8 - shift));
}

unsigned int CircRightShift(unsigned int value, unsigned int shift) {
    return (value >> shift) | (value << (sizeof(value) * 8 - shift));
}

unsigned int AlwaysZero_Hash(const char * str) {
    return 0;
}

unsigned int FirstAsciiChar_Hash(const char * str) {
    return (unsigned char) *str;
}

unsigned int Strlen_Hash(const char * str) {
    return strlen(str);
}

unsigned int StrlenIntrinsic_Hash(const char * str) {
    return MyStrlen(str);
}

unsigned int AsciiSum_Hash(const char * str) {
    unsigned int hash = 0;
    while (*str) {
        hash += (unsigned char) *(str++);
    }
    return hash;
}

unsigned int Rollleft_Hash(const char * str) {
    unsigned int hash = 0;
    while (*str) {
        hash = (CircLeftShift(hash, 1) ^ *(str++));
    }
    return hash;
}

unsigned int Rollright_Hash(const char * str) {
    unsigned int hash = 0;
    while (*str) {
        hash = (CircRightShift(hash, 1) ^ *(str++));
    }
    return hash;
}

unsigned int CRC32_Hash(const char *data) {
    unsigned int hash = 0xFFFFFFFF;
    
    while (*data) {
        hash ^= *data;
        //          num of bits in a byte
        //                 \|/
        for (int j = 0; j < 8; j++) {
            if (hash & 1) {
                hash = (hash >> 1) ^ 0xEDB88320;
            } else {
                hash >>= 1;
            }
        }
        data++;
    }
    return ~hash; 
}

unsigned int CRC32Intrinsic_Hash(const char *str) {
    unsigned int hash = 0xFFFFFFFF;

    while (*str) {
        hash = _mm_crc32_u8(hash, *str);
        str++;
    }
    
    return hash ^ 0xFFFFFFFF; // Финальный XOR (стандарт CRC32)
}

unsigned int FNV1A_Hash(const char *str) {
    unsigned int hash = 2166136261;
    while (*str) {
        hash ^= (unsigned char) *str++;
        hash *= 16777619;
    }
    return hash;
}