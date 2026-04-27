#include "hash.h"
#include "list.h"
#include "hashfuncs.h"
#include "hashdump.h"
#include "../my_libs/sassert.h"

#ifndef INIT_FUNC_IDX
#define INIT_FUNC_IDX CRC32_Hash_E
#endif

#include <chrono>
#include <string.h>

string TESTING_FILE_NAME = "texts/ALLTEXTSINONE.txt";

void ClearFile(const char *FileName) {
    FILE *fp = fopen(FileName, "w");
    fclose(fp);
}

#define MEASURE_EXECTIME(lines_of_code, TimeVarName) {                                      \
    auto start  = std::chrono::steady_clock::now();                                         \
    lines_of_code                                                                           \
    auto end    = std::chrono::steady_clock::now();                                         \
    TimeVarName = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();\
    }  

size_t HashAllFilesToHmap(Hashmap_t *Hmap, unsigned int (*HashFunc)(const char *), int argc, char *argv[]) {
    size_t SumWordsRead = 0;
    for (size_t FileIdx = 1; FileIdx < argc; FileIdx++) {
        if (SumWordsRead >= MAX_READ_WORDS) break;
        SumWordsRead += HashAllWordsFromFiletoHmap(Hmap, argv[FileIdx], HashFunc, MAX_READ_WORDS - SumWordsRead);
    }
    return SumWordsRead;
}

int main(int argc, char *argv[]) {
    size_t NumOfTestingWords = 0;
    char **TestingWords = GetTestingWords(TESTING_FILE_NAME, &NumOfTestingWords);
    printf(MAGENTA "всего %zu слов для поиска\n" WHITE, NumOfTestingWords);


    size_t FuncIdx = INIT_FUNC_IDX;
    unsigned int (*HashFunc)(const char *) = AllHashFuncs[FuncIdx];
    

    HashMapCtor(Hmap, HASH_TABLE_SIZE);
    size_t InitWords = HashAllFilesToHmap(Hmap, HashFunc, argc, argv);
    printf(MAGENTA "всего %zu слов в хешмапе\n" WHITE, InitWords);
    
    #ifdef DUMP_HASH
    DumpHmapToHtml(Hmap, ListDumpPathStr[INIT_FUNC_IDX], NumOfTestingWords,
                   InitWords, INIT_FUNC_IDX);
    #endif

    volatile size_t SumFoundWords = 0;
    for (size_t i = 0; i < NumOfTestingWords; i++) {
        if ((i + 1) % (NumOfTestingWords / 2) == 0)
            printf("половина слов найдена\n");
        SumFoundWords += HmapFind(Hmap, TestingWords[i], HashFunc);
    }
    
    printf(GREEN "найдено %zu слов\n" WHITE, SumFoundWords);
    for (size_t i = 0; i < NumOfTestingWords; i++) {
        free(TestingWords[i]);
    }
    free(TestingWords);
    HashMapDtor(Hmap);
    return 0;
}