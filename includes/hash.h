#ifndef HASH_H
#define HASH_H

#include "list.h"
const  size_t HMAP_MAX_SIZE             = 0xfffffff;
const  size_t HASH_TABLE_SIZE           = 4999;
const  size_t NUM_OF_MEASURE_REPS       = 100;
const  int    VIEW_DISTANCE             = 10;
const  size_t MAX_READ_WORDS            = 10000000;
const  size_t BUFFER_START_CAPACITY     = 4096;
string WORD_DELIMS                      = " \n\r";

#define MEASURE_EXECTIME(lines_of_code, TimeVarName) {                                      \
    auto start  = std::chrono::steady_clock::now();                                         \
    lines_of_code                                                                           \
    auto end    = std::chrono::steady_clock::now();                                         \
    TimeVarName = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();\
    }  

typedef struct {
    size_t size;
    list_t **Table;
    unsigned int (*HashFunc)(const char *);
} Hashmap_t;

// =======================================================================
#define x(n) \
    n(OK_HMAP                               ,   0)  \
    n(ERR_CMD_INVALID_HMAP                  ,   1)  \
    n(ERR_INVALID_INDEX_HMAP                ,   2)  \
    n(ERR_OUT_OF_BOUNDS_HMAP                ,   3)  \
    n(ERR_NOTHING_TO_POP_HMAP               ,   4)  \
    n(ERR_INVALID_OPERATION_HMAP            ,   5)  \
    n(ERR_REALLOC_FAIL_HMAP                 ,   6)  \
    n(ERR_INCORRECT_LIST_HMAP               ,   7)  \
    n(ERR_CYCLING_LIST_HMAP                 ,   8)  \
    n(ERR_INVALID_ARGUMENTS_HMAP            ,   9)  \
    n(ERR_INVALID_PREV_HMAP                 ,   10) \
    n(ERR_INVALID_NEXT_HMAP                 ,   11) \
    n(ERR_INVALID_SIZE_HMAP                 ,   12) \
    n(ERR_INVALID_RELATION_WITH_PREV_HMAP   ,   13) \
    n(ERR_INVALID_RELATION_WITH_NEXT_HMAP   ,   14) \
    n(ERR_REALLOC_DOWN_FAIL_HMAP            ,   15) \
    n(ERR_PTR_NULL_HMAP                     ,   16) \
    n(ERR_NO_FLAGS_HMAP                     ,   17) \
    n(ERR_FILE_NOT_FOUND_HMAP               ,   18) \
    n(ERR_INVALID_SYNTAX_HMAP               ,   19) \
    n(ERR_INVALID_HASH_HMAP                 ,   20) \
    n(ERR_VERIFY_FAILED_HMAP                ,   21) \
    n(ERR_MULTIPLE_VALUES_HMAP              ,   22)

#define init_enum(val, ind) \
    val = ind,
#define init_str(val, ...) \
    #val, 

string hash_error_text[] = {x(init_str)};
enum HmapError_t {
    x(init_enum)
};
#undef x
#undef init_enum
#undef init_str
// =======================================================================

#define HashMapCtor(name, size) \
    Hashmap_t *name = (Hashmap_t *) calloc(1, sizeof(Hashmap_t));\
    sassert(name, ERR_PTR_NULL, "не удалось создать hmap размера %zu", size);\
    HashMapCtor_internal(name, size);

#define HashMapDtor(Hmap) \
    HashMapDtor_internal(Hmap);

// =======================================================================


void HashMapCtor_internal(Hashmap_t *Hmap, size_t size);
HmapError_t HashMapDtor_internal(Hashmap_t *Hmap);

void SkipToNext(char ** str, char ch);
size_t HashAllWordsFromFiletoHmap(Hashmap_t *Hmap, const char * FileName, size_t RemainingLimit);
HmapError_t WriteHmapToFile(Hashmap_t *Hmap, const char * FileName);
Hashmap_t *ReadHmapFromFile(const char * FileName);

char *GetBufferFromFile(FILE *fp, size_t FileSize);
size_t GetFileSize(FILE *fp);

char **GetTestingWords(const char *FileName, size_t *NumOfWords);
int HmapFind(Hashmap_t *Hmap, const char *key);
int HmapAdd(Hashmap_t *Hmap, const char * value);
int add_with_intercept_check(list_t *List, const char *value);
HmapError_t HmapVerifier(Hashmap_t *Hmap);
#endif // HASH_H