#include "list.h"
#include "list_dump.h"
#include "hash.h"
#include "hashfuncs.h"
#include "../my_libs/error_manage.h"
#include "../my_libs/sassert.h"
#include <chrono>
#include <immintrin.h>

#define EXTENSIVELOGS
extern error_t error;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void HashMapCtor_internal(Hashmap_t *Hmap, size_t size) {
    Hmap->size  = size;
    Hmap->Table = (bucket_t **) calloc(size, sizeof(bucket_t *));

    size_t HmapIndex = 0;
    for (size_t i = 0; i < Hmap->size; i++) {
        char **ThreeFoldArray = (char **) calloc(START_ARR_SIZE * 3, sizeof(char *));
        sassert(ThreeFoldArray, ERR_PTR_NULL_HMAP,
                "не удалось выделить место для хем таблицы");
        Hmap->Table[i]           = (bucket_t *) calloc(1, sizeof(bucket_t));
        Hmap->Table[i]->Str      = ThreeFoldArray;
        Hmap->Table[i]->Hashes   = (size_t *) ThreeFoldArray + START_ARR_SIZE;
        Hmap->Table[i]->Strlens  = (size_t *) ThreeFoldArray + START_ARR_SIZE * 2;
        Hmap->Table[i]->capacity = START_ARR_SIZE;
        Hmap->Table[i]->size = 0;
    }
}

HmapError_t HashMapDtor_internal(Hashmap_t *Hmap) {
    if (Hmap == NULL)
        return ERR_PTR_NULL_HMAP;
    
    for (size_t i = 0; i < Hmap->size; i++) {
        if (Hmap->Table[i]) {
            for (size_t j = 0; j < Hmap->Table[i]->size; j++) {
                free(Hmap->Table[i]->Str[j]);
            }
            free(Hmap->Table[i]->Str);
            free(Hmap->Table[i]);
        }
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

HmapError_t ReallocBucket(bucket_t *Bucket) {
    sassert(Bucket, ERR_PTR_NULL_HMAP);

    size_t OldCapacity = Bucket->capacity;
    size_t NewCapacity = Bucket->capacity * 2;
    void *ThreeFoldBuffer = safe_realloc((void **) &Bucket->Str, NewCapacity * 3 * sizeof(char *));
    
    char *Base = (char *) ThreeFoldBuffer;
    memmove(Base + NewCapacity * 2 * sizeof(char *), 
            Base + OldCapacity * 2 * sizeof(char *), 
            OldCapacity *            sizeof(char *));
            
    memmove(Base + NewCapacity * sizeof(char *), 
            Base + OldCapacity * sizeof(char *), 
            OldCapacity *        sizeof(char *));

    Bucket->Str     = (char **)     ThreeFoldBuffer;
    Bucket->Hashes  = (size_t *)    Base + NewCapacity;
    Bucket->Strlens = (size_t *)    Base + NewCapacity * 2;
    Bucket->capacity = NewCapacity;
    sassert(Bucket->Str, ERR_PTR_NULL_HMAP,
            "не удалось реаллоцировать место для бакета");

    return OK_HMAP;
}

int add_with_intercept_check(size_t ValueHash, bucket_t *Bucket, const char * value) {
    sassert(Bucket, ERR_PTR_NULL_HMAP);
    sassert(value,  ERR_PTR_NULL_HMAP);

    if (Bucket->size >= Bucket->capacity)
        ReallocBucket(Bucket);
    
    size_t BucketSize   = Bucket->size;
    size_t ValueStrlen  = strlen(value);
    
    for (size_t i = 0; i < BucketSize; i++) {
        if (ValueHash   == Bucket->Hashes[i]    &&
            ValueStrlen == Bucket->Strlens[i]   &&
            strcmp(value, Bucket->Str[i]) == 0)
            return 0;
    }

    Bucket->Str[BucketSize]         = strdup(value);
    Bucket->Strlens[BucketSize]     = ValueStrlen;
    Bucket->Hashes[BucketSize]      = ValueHash;
    Bucket->size++;
    return 1;
}

size_t HashAllWordsFromFiletoHmap(Hashmap_t *Hmap, const char * FileName, size_t RemainingLimit) {
    sassert(Hmap,       ERR_PTR_NULL_HMAP);
    sassert(FileName,   ERR_PTR_NULL_HMAP);

    FILE *fp = fopen(FileName, "rb");
    RET_ASSERT(fp, 0, "файл \"%s\" не существует", FileName);
    
    size_t FileSize     = GetFileSize(fp);
    char *FileBuffer    = GetBufferFromFile(fp, FileSize);
    char *token         = strtok(FileBuffer, WORD_DELIMS);

    size_t WordsCount = 0;
    while (token && *token) {
        if (WordsCount >= RemainingLimit)
            break;

        if (HmapAdd(Hmap, token))
            WordsCount++;
    
        token = strtok(NULL, WORD_DELIMS);
    }
    
    free(FileBuffer);
    fclose(fp);
    return WordsCount;
}

void DumpBucketCsv(bucket_t *Bucket, FILE *fp) {
    sassert(Bucket, ERR_PTR_NULL);
    sassert(fp,     ERR_PTR_NULL);

    for (size_t i = 0; i < Bucket->size; i++) {
        char * Data = Bucket->Str[i];
        if (Data == POISON)
            fputs("PSN\n", fp);
        else
            fprintf(fp, "%s\n", Data);
    }
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
        DumpBucketCsv(Hmap->Table[i], fp);
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
    char *OrigBuffer = Buffer;
    fclose(fp);

    char *Pos = strtok(Buffer, "\n");
    size_t CurHash = 0;

    char *NextBuffer = Buffer;
    size_t HmapSize = strtoull(NextBuffer, &NextBuffer, 10);
    HashMapCtor(Hmap, HmapSize);

    while (Pos && *Pos) {
        if (*Pos == '[')
            CurHash = strtoull(Pos + 1, &Pos, 10);
        else
            add_with_intercept_check(CurHash, Hmap->Table[CurHash], Pos);
        Pos = strtok(NULL, "\n");
    }
    free(OrigBuffer);
    return Hmap;
}

int HmapAdd(Hashmap_t *Hmap, const char * value) {
    sassert(Hmap,   ERR_PTR_NULL);
    sassert(value,  ERR_PTR_NULL);
    CHECK_FUNC_AND_RET_IF_ERR(HmapVerifier(Hmap));

    size_t ValueHash = Hmap->HashFunc(value);
    size_t Index = ValueHash % Hmap->size;
    bool FuncRetValue = add_with_intercept_check(ValueHash, Hmap->Table[Index], value);
    return FuncRetValue;
}

int HmapFind(Hashmap_t *Hmap, const char *value) {
    sassert(Hmap,   ERR_PTR_NULL);
    sassert(value,  ERR_PTR_NULL);
    CHECK_FUNC_AND_RET_IF_ERR(HmapVerifier(Hmap));
    
    size_t ValueHash    = Hmap->HashFunc(value);
    size_t index        = ValueHash % Hmap->size;
    bucket_t *Bucket    = Hmap->Table[index];
    size_t BucketSize   = Bucket->size;
    size_t ValueStrlen  = strlen(value);

    size_t  *Hashes         = Bucket->Hashes;
    size_t  *Strlens        = Bucket->Strlens;
    char    **Str           = Bucket->Str;
    
    for (size_t i = 0; i < BucketSize; i++) {
        if (ValueHash   == Hashes[i]    &&
            ValueStrlen == Strlens[i]   &&
            strcmp(value, Str[i]) == 0)
            return 1;
    }
    return 0;
}

int HmapRemove(Hashmap_t *Hmap, const char *value) {
    sassert(Hmap,   ERR_PTR_NULL);
    sassert(value,  ERR_PTR_NULL);
    CHECK_FUNC_AND_RET_IF_ERR(HmapVerifier(Hmap));
    
    size_t ValueHash    = Hmap->HashFunc(value);
    size_t index        = ValueHash % Hmap->size;
    bucket_t *Bucket    = Hmap->Table[index];
    size_t BucketSize   = Bucket->size;
    size_t ValueStrlen  = strlen(value);
    
    for (size_t i = 0; i < BucketSize; i++) {
        if (ValueHash   == Bucket->Hashes[i]    &&
            ValueStrlen == Bucket->Strlens[i]   &&
            strcmp(value, Bucket->Str[i]) == 0) {
            
            free(Bucket->Str[i]);
            
            if (i != BucketSize - 1) {
                Bucket->Str[i]     = Bucket->Str[BucketSize - 1];
                Bucket->Hashes[i]  = Bucket->Hashes[BucketSize - 1];
                Bucket->Strlens[i] = Bucket->Strlens[BucketSize - 1];
            }
            
            Bucket->size--;
            return 1;
        }
    }
    return 0;
}

char **GetTestingWords(const char *FileName, size_t *NumOfWords, char **TestingWordsArr) {
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

        TestingWords[WordsRead] = token;
        WordsRead++;
        if (WordsRead >= MAX_READ_WORDS)
            break;

        token = strtok(NULL, WORD_DELIMS);
    }

    *NumOfWords = WordsRead;
    fclose(fp);
    *TestingWordsArr = FileBuffer;
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
HmapError_t HmapVerifier(Hashmap_t *Hmap) {
    sassert(Hmap, ERR_PTR_NULL);
    if (Hmap->size >= HMAP_MAX_SIZE || Hmap->size == 0)
        add_error(ERR_INVALID_SIZE_HMAP,
                             "размер хеш таблицы не корректный: size = %zu", Hmap->size);
    if (!Hmap->Table)
        ADD_ERROR_AND_RETURN(ERR_PTR_NULL_HMAP,
                             "Хеш таблица имеет нулевой адрес");

    for (size_t hash = 0; hash < Hmap->size; hash++) {
        size_t BucketSize = Hmap->Table[hash]->size;
        if (BucketSize == 0)
            continue;
        char * ArrayOfWords[BucketSize] = {};

        for (size_t j = 0; j < BucketSize; j++) {
            char *value = Hmap->Table[hash]->Str[j];
            ArrayOfWords[j] = value;

            if (Hmap->HashFunc(value) % Hmap->size != hash)
                add_error(ERR_INVALID_HASH_HMAP, 
                                     "хеш для строки <%s> не совпадает с ее позицией в хеш функции", value);
            if (ValueInArray(value, ArrayOfWords, j))
                ADD_ERROR_AND_RETURN(ERR_MULTIPLE_VALUES_HMAP, 
                                     "несколько одинаковых строк <%s> в одной ячейке", value);
        }
    }
    return OK_HMAP;
}
#else
HmapError_t HmapVerifier(Hashmap_t *Hmap) {return OK_HMAP;}
#endif 

HmapError_t PrintListSizeToFile(Hashmap_t *Hmap, const char * FileName) {
    sassert(Hmap,       ERR_PTR_NULL);
    sassert(FileName,   ERR_PTR_NULL);

    FILE *fp = fopen(FileName, "w");
    RET_ASSERT(fp, ERR_PTR_NULL_HMAP, "не удалось открыть файл %s", FileName);

    fprintf(fp, "%zu\n", Hmap->size);
    for (size_t i = 0; i < Hmap->size; i++) {
        fprintf(fp, "%zu\n", Hmap->Table[i]->size);
    }

    fclose(fp);
    return OK_HMAP;
}