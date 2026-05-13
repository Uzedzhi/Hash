#ifndef HASHFUNCS_H
#define HASHFUNCS_H

typedef const char * const string;
extern "C" unsigned int MyCRC32_Hash(const char *str) noexcept;
unsigned int SDBM_Hash(const char * str);
unsigned int Rollright_Hash(const char * str);
unsigned int Rollleft_Hash(const char * str);
unsigned int AsciiSum_Hash(const char * str);
unsigned int Strlen_Hash(const char * str);
unsigned int FirstAsciiChar_Hash(const char * str);
unsigned int AlwaysZero_Hash(const char * str);
unsigned int CRC32_Hash(const char * str);
unsigned int FNV1A_Hash(const char * str);

unsigned int CircRightShift(unsigned int value, unsigned int shift);
unsigned int CircLeftShift(unsigned int value, unsigned int shift);

#define IMG_DUMP_PATH  "img"
#define SIZE_DUMP_PATH "listsizes"
#define HTML_DUMP_PATH "listdump"

typedef struct HashDump_t {
    unsigned int (*const Ptr)(const char *);
    const char * Str;
    const char * SizeDumpStr;
    const char * HTMLDumpStr;
    const char * ImgDumpStr;
} HashDump_t;

#define MAKE_HASH_FUNCS_STRUCT(HashFunc)            \
    {                                               \
        HashFunc,                                   \
        #HashFunc,                                  \
        SIZE_DUMP_PATH  "/" #HashFunc ".txt"  ,     \
        HTML_DUMP_PATH  "/" #HashFunc ".html" ,     \
        IMG_DUMP_PATH   "/" #HashFunc ".png"        \
    }

const HashDump_t AllHashFuncs[] = {
    MAKE_HASH_FUNCS_STRUCT(AlwaysZero_Hash),
    MAKE_HASH_FUNCS_STRUCT(FirstAsciiChar_Hash),
    MAKE_HASH_FUNCS_STRUCT(Strlen_Hash),
    MAKE_HASH_FUNCS_STRUCT(AsciiSum_Hash),
    MAKE_HASH_FUNCS_STRUCT(Rollleft_Hash),
    MAKE_HASH_FUNCS_STRUCT(Rollright_Hash),
    MAKE_HASH_FUNCS_STRUCT(SDBM_Hash),
    MAKE_HASH_FUNCS_STRUCT(CRC32_Hash),
    MAKE_HASH_FUNCS_STRUCT(FNV1A_Hash),
    MAKE_HASH_FUNCS_STRUCT(MyCRC32_Hash),
};

enum AllHashFuncs_E {
    AlwaysZero_Hash_E             = 0,
    FirstAsciiChar_Hash_E         = 1,
    Strlen_Hash_E                 = 2,
    AsciiSum_Hash_E               = 3,
    Rollleft_Hash_E               = 4,
    Rollright_Hash_E              = 5,
    SDBM_Hash_E                   = 6,
    CRC32_Hash_E                  = 7,
    FNV1A_Hash_E                  = 8,
    MyCRC32_Hash_E                = 9,
};

string CSV_DUMP_FILE_NAME   = "tests/AllMeasurements.csv";
#ifndef SPEEDTESTS_FILE_NAME
#define SPEEDTESTS_FILE_NAME "tests/SpeedTest.csv"
#endif

#ifndef DUMP_FILE_NAME
#define DUMP_FILE_NAME "listdump/dump.html"
#endif // DUMP_FILE_NAME

#undef MAKE_HASH_FUNCS_STRUCT

#endif // HASHFUNCS_H