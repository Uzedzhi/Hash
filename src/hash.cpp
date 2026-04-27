#include "list.h"
#include "list_dump.h"
#include "hash.h"
#include "hashfuncs.h"
#include "../my_libs/error_manage.h"
#include "../my_libs/sassert.h"
#include <chrono>

#define EXTENSIVELOGS
extern error_t error;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void HashMapCtor_internal(Hashmap_t *Hmap, size_t size) {
    Hmap->size = size;
    Hmap->Table = (list_t **) calloc(size, sizeof(list_t *));

    size_t HmapIndex = 0;
    for (size_t i = 0; i < Hmap->size; i++) {
        listCtor(List);
        Hmap->Table[HmapIndex++] = List;
    }
}

HmapError_t HashMapDtor_internal(Hashmap_t *Hmap) {
    if (Hmap == NULL)
        return ERR_PTR_NULL_HMAP;
    
    size_t HmapIndex = 0;
    for (size_t i = 0; i < Hmap->size; i++) {
        listDtor(Hmap->Table[HmapIndex++]);
    }
    
    free(Hmap->Table);
    free(Hmap);
    return OK_HMAP;
}

size_t GetFileSize(FILE *fp) {
    fseek(fp, 0, SEEK_END);
    size_t FileSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    return FileSize;
}

char * GetBufferFromFile(FILE *fp, size_t FileSize) {
    char *FileBuffer = (char *) calloc(FileSize + 32, sizeof(char));
    sassert(FileBuffer, ERR_PTR_NULL_HMAP);

    size_t CharsRead = fread(FileBuffer, sizeof(char), FileSize, fp);
    FileBuffer[CharsRead] = '\0';
    return FileBuffer;
}

int add_with_intercept_check(list_t *List, const char * value) {
    sassert(List, ERR_PTR_NULL);

    list_iterator_ctor(ListIter, List);
    while (!list_iterator_end(&ListIter)) {
        data_t CurValue = list_iterator_value(&ListIter);
        if (CurValue != POISON && strcmp(CurValue, value) == 0)
            return 0;
        list_iterator_next(&ListIter);
    }
    
    add_in_head(List, (char*) value);
    return 1;
}

size_t HashAllWordsFromFiletoHmap(Hashmap_t *Hmap, const char * FileName,
                                       unsigned int (*HashFunc) (const char *), size_t RemainingLimit) {
    sassert(Hmap,       ERR_PTR_NULL_HMAP);
    sassert(FileName,   ERR_PTR_NULL_HMAP);
    sassert(HashFunc,   ERR_PTR_NULL_HMAP);

    FILE *fp = fopen(FileName, "rb");
    RET_ASSERT(fp, 0, "файл \"%s\" не существует", FileName);
    
    size_t FileSize     = GetFileSize(fp);
    char *FileBuffer    = GetBufferFromFile(fp, FileSize);
    char *token         = strtok(FileBuffer, WORD_DELIMS);

    size_t WordsCount = 0;

    while (token && *token) {
        if (WordsCount >= RemainingLimit)
            break;

        HmapAdd(Hmap, token, HashFunc);
        WordsCount++;
    
        token = strtok(NULL, WORD_DELIMS);
    }
    
    fclose(fp);
    return WordsCount;
}

// hmap бует храниться в формате <hash>,<val1>,<val2>,<val3>,...
HmapError_t WriteHmapToFile(Hashmap_t *Hmap, const char * FileName) {
    sassert(FileName, ERR_PTR_NULL);

    FILE *fp = fopen(FileName, "w");
    RET_ASSERT(fp, ERR_PTR_NULL_HMAP,
                   "файла \"%s\" не может открыться", FileName);

    fprintf(fp, "%zu\n", Hmap->size);
    for (size_t i = 0; i < Hmap->size; i++) {
        fprintf(fp, "[%zu]\n", i);
        DumplistCsv(Hmap->Table[i], fp);
    }
    fclose(fp);

    return OK_HMAP;
}

void SkipToNext(char ** str, char ch) {
    sassert(str,  ERR_PTR_NULL);
    sassert(*str, ERR_PTR_NULL);

    while (**str && **str != ch)
        (*str)++;
    (*str)++;
}

Hashmap_t *ReadHmapFromFile(const char * FileName) {
    sassert(FileName, ERR_PTR_NULL);

    FILE *fp = fopen(FileName, "r");
    RET_ASSERT(fp, NULL, "файла \"%s\" не существует", FileName);

    size_t FileSize = GetFileSize(fp);
    char *Buffer = GetBufferFromFile(fp, FileSize);
    fclose(fp);

    char *Pos = strtok(Buffer, "\n");
    size_t CurHash = 0;

    size_t HmapSize = strtoull(Buffer, &Buffer, 10);
    HashMapCtor(Hmap, HmapSize);

    while (*Pos) {
        if (*Pos == '[')
            CurHash = strtoull(Pos + 1, &Pos, 10);
        else
            add_with_intercept_check(Hmap->Table[CurHash], Pos);
        Pos = strtok(NULL, "\n");
    }

    free(Buffer);
    return Hmap;
}

int HmapAdd(Hashmap_t *Hmap, const char * value, unsigned int (*HashFunc)(const char *)) {
    sassert(Hmap,   ERR_PTR_NULL);
    sassert(value,  ERR_PTR_NULL);
    CHECK_FUNC_AND_RET_IF_ERR(HmapVerifier(Hmap, HashFunc));

    size_t Index = HashFunc(value) % Hmap->size;
    bool FuncRetValue = add_with_intercept_check(Hmap->Table[Index], value);
    return FuncRetValue;
}

int HmapFind(Hashmap_t *Hmap, const char *key, unsigned int (*HashFunc)(const char *)) {
    sassert(Hmap, ERR_PTR_NULL);
    sassert(key,  ERR_PTR_NULL);
    CHECK_FUNC_AND_RET_IF_ERR(HmapVerifier(Hmap, HashFunc));

    size_t index = HashFunc(key) % Hmap->size;
    list_iterator_ctor(ListIter, Hmap->Table[index]);

    while (!list_iterator_end(&ListIter)) {
        if (strcmp(list_iterator_value(&ListIter), key) == 0)
            return 1;
        list_iterator_next(&ListIter);
    }
    return 0;
}

char **GetTestingWords(const char *FileName, size_t *NumOfWords) {
    sassert(FileName,   ERR_PTR_NULL_HMAP);

    FILE *fp = fopen(FileName, "rb");
    sassert(fp, ERR_FILE_NOT_FOUND_HMAP,
                "файл \"%s\" не существует", FileName);
    
    size_t FileSize  = GetFileSize(fp);
    char *FileBuffer = GetBufferFromFile(fp, FileSize);

    size_t capacity = BUFFER_START_CAPACITY;
    char **TestingWords = (char **) calloc(capacity, sizeof(char *));
    sassert(TestingWords, ERR_PTR_NULL_HMAP);

    size_t WordsRead = 0;
    char *token = strtok(FileBuffer, WORD_DELIMS);
    while (token && *token) {
        if (WordsRead >= capacity) {
            capacity *= 2;
            TestingWords = (char **) realloc(TestingWords, capacity * sizeof(char *));
            sassert(TestingWords, ERR_REALLOC_FAIL_HMAP);
        }

        TestingWords[WordsRead] = strdup(token);
        WordsRead++;
        if (WordsRead >= MAX_READ_WORDS)
            break;

        token = strtok(NULL, WORD_DELIMS);
    }

    *NumOfWords = WordsRead;
    fclose(fp);
    free(FileBuffer);
    return TestingWords;
}

bool ValueInArray(char * value, char * Array[], size_t ListSize) {
    sassert(value, ERR_PTR_NULL);
    sassert(Array, ERR_PTR_NULL);

    for (size_t i = 0; i < ListSize; i++) {
        if (value && strcmp(value, Array[i]) == 0)
            return 1;
    }
    return 0;
}

#ifndef NDEBUG
HmapError_t HmapVerifier(Hashmap_t *Hmap, unsigned int (*HashFunc)(const char *)) {
    sassert(Hmap, ERR_PTR_NULL);
    if (Hmap->size >= HMAP_MAX_SIZE || Hmap->size == 0)
        add_error(ERR_INVALID_SIZE_HMAP,
                             "размер хеш таблицы не корректный: size = %zu", Hmap->size);
    if (!Hmap->Table)
        ADD_ERROR_AND_RETURN(ERR_PTR_NULL_HMAP,
                             "Хеш таблица имеет нулевой адрес");

    for (size_t hash = 0; hash < Hmap->size; hash++) {
        if (verify_list(Hmap->Table[hash]) != OK_HMAP)
            ADD_ERROR_AND_RETURN(ERR_VERIFY_FAILED_HMAP);
        
        size_t ListSize = getListSize(Hmap->Table[hash]);
        if (ListSize == 0)
            continue;
        char * ArrayOfWords[ListSize] = {};

        list_iterator_ctor(ListIter, Hmap->Table[hash]);
        for (size_t j = 0; j < ListSize; j++) {
            data_t value = list_iterator_value(&ListIter);
            ArrayOfWords[j] = value;

            if (HashFunc(value) % Hmap->size != hash)
                add_error(ERR_INVALID_HASH_HMAP, 
                                     "хеш для строки <%s> не совпадает с ее позицией в хеш функции", value);
            if (ValueInArray(value, ArrayOfWords, j))
                ADD_ERROR_AND_RETURN(ERR_MULTIPLE_VALUES_HMAP, 
                                     "несколько одинаковых строк <%s> в одной ячейке", value);
            list_iterator_next(&ListIter);
        }
    }
    return OK_HMAP;
}
#else
HmapError_t HmapVerifier(Hashmap_t *Hmap, unsigned int (*HashFunc)(const char *)) {return OK_HMAP;}
#endif 

HmapError_t PrintListSizeToFile(Hashmap_t *Hmap, const char * FileName) {
    sassert(Hmap,       ERR_PTR_NULL);
    sassert(FileName,   ERR_PTR_NULL);

    FILE *fp = fopen(FileName, "w");
    RET_ASSERT(fp, ERR_PTR_NULL_HMAP, "не удалось открыть файл %s", FileName);

    for (size_t i = 0; i < Hmap->size; i++) {
        fprintf(fp, "%zu\n", getListSize(Hmap->Table[i]));
    }

    fclose(fp);
    return OK_HMAP;
}