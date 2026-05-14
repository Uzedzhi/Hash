#include "hash.h"
#include "list.h"
#include "hashfuncs.h"
#include "hashdump.h"
#include "../my_libs/sassert.h"

#include <string.h>

const size_t REP_COUNT   = 30;
string TESTING_FILE_NAME = "texts/ALLTEXTSINONE.txt";

#ifndef INIT_FUNC_IDX
#define INIT_FUNC_IDX CRC32_Hash_E
#endif

size_t HashAllFilesToHmap(Hashmap_t *Hmap, int argc, char *argv[]) {
    size_t SumWordsRead = 0;
    for (size_t FileIdx = 1; FileIdx < argc; FileIdx++) {
        if (SumWordsRead >= MAX_READ_WORDS) break;
        SumWordsRead += HashAllWordsFromFiletoHmap(Hmap, argv[FileIdx], MAX_READ_WORDS - SumWordsRead);
    }
    return SumWordsRead;
}

int main(int argc, char *argv[]) {
    RET_ASSERT(argc != 1, ERR_INVALID_ARGUMENTS_HMAP,
               "укажите хотя бы один файл для загрузки в хэш таблицу");

    size_t NumOfTestingWords = 0;
    char *TestingWordsBuffer = NULL;
    char **TestingWords = GetTestingWords(TESTING_FILE_NAME, &NumOfTestingWords, &TestingWordsBuffer);
    printf(MAGENTA "всего %zu слов для поиска\n" WHITE, NumOfTestingWords);

    HashMapCtor(Hmap, HASH_TABLE_SIZE);
    Hmap->HashFunc = AllHashFuncs[INIT_FUNC_IDX].Ptr;
    size_t InitWords = HashAllFilesToHmap(Hmap, argc, argv);
    printf(MAGENTA "всего %zu слов в хешмапе\n" WHITE, InitWords);
    
    #if defined(DUMP_ENABLE)
        DumpHmapToHtml(Hmap, NumOfTestingWords,
                       InitWords, INIT_FUNC_IDX);
    #endif

    volatile size_t SumFoundWords = 0;
    // for (size_t rep = 0; rep < 100; rep++) {
    //     for (size_t i = 0; i < NumOfTestingWords; i++) {
    //         SumFoundWords += HmapFind(Hmap, TestingWords[i]);
    //     }
    // }
    
    printf(GREEN "найдено %zu слов\n" WHITE, SumFoundWords);
    free(TestingWords);
    free(TestingWordsBuffer);
    HashMapDtor(Hmap);
    return 0;
}