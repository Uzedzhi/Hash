#include "hash.h"
#include "list.h"
#include "hashfuncs.h"
#include "hashdump.h"
#include "../my_libs/sassert.h"

#include <chrono>
#include <string.h>

string TESTING_FILE_NAME = "texts/WARANDPEACE.txt";
#define MEASURE_EXECTIME(lines_of_code, TimeVarName) {                                      \
    auto start  = std::chrono::steady_clock::now();                                         \
    lines_of_code                                                                           \
    auto end    = std::chrono::steady_clock::now();                                         \
    TimeVarName = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();\
    }                                                   

int main(int argc, char *argv[]) {
    RET_ASSERT(argc != 1, ERR_INVALID_ARGUMENTS_HMAP,
               "введите хотя бы один файл для инициализации hashmap\n");

    size_t NumOfTestingWords = 0;
    char **TestingWords = GetTestingWords(TESTING_FILE_NAME, &NumOfTestingWords);

    FILE *fp = fopen(SPEEDTESTS_FILE_NAME, "w");
    RET_ASSERT(fp, ERR_PTR_NULL, "не удалось открыть файл %s", SPEEDTESTS_FILE_NAME);

    // =====================tests==========================
    size_t ExecTime = 0;
    for (size_t FuncIdx = 1; FuncIdx < NUM_OF_HASH_FUNCS; FuncIdx++) {
        for (size_t Rep = 0; Rep < NUM_OF_MEASURE_REPS; Rep++) {
            unsigned int (*HashFunc)(const char *) = AllHashFuncs[FuncIdx];
            HashMapCtor(Hmap, HASH_TABLE_SIZE);
            
            printf(RED "++++++++++++++Повторение %zu++++++++++++++" WHITE "\n", Rep);
            printf(MAGENTA "Тестируем функцию %s" WHITE "\n", AllHashFuncsStr[FuncIdx]);

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            // ================Reading From File To Hmap==============
            size_t SumWordsRead = 0;
            MEASURE_EXECTIME(
                for (size_t FileIdx = 1; FileIdx < argc; FileIdx++) {
                    if (SumWordsRead >= MAX_READ_WORDS) break;
                    SumWordsRead += HashAllWordsFromFiletoHmap(Hmap, argv[FileIdx], HashFunc, MAX_READ_WORDS - SumWordsRead);
                }
                , ExecTime
            );
            printf("Слов Прочитано Из файлов: %zu, Слов для теста: %zu\n", SumWordsRead, NumOfTestingWords);
            // ===================Read Successuflly====================


            double DeltaTimeRead = (double) ExecTime / 1e6;
            printf(GREEN "\t·Данные прочитаны из файла за %lf мс" WHITE "\n", DeltaTimeRead);


            // ====================test finding=========================
            size_t FoundCount = 0;
            MEASURE_EXECTIME(
                for (size_t i = 0; i < NumOfTestingWords; i++) {
                    FoundCount += HmapFind(Hmap, TestingWords[i], HashFunc);
                    DUMP_AND_RET_IF_ERROR(hash_error_text, fclose(fp););
                }
                , ExecTime
            );
            // ================test finding finished====================


            double DeltaTimeFind = (double) ExecTime / 1e6;
            printf(GREEN "\t·Тест поиска пройден за %lf мс, найдено: %zu слов" WHITE "\n", DeltaTimeFind, FoundCount);
            

            // ====================test adding=========================
            MEASURE_EXECTIME(
                for (size_t i = 0; i < NumOfTestingWords; i++) {
                    HmapAdd(Hmap, TestingWords[i], HashFunc);
                    DUMP_AND_RET_IF_ERROR(hash_error_text, fclose(fp););
                }
                , ExecTime
            );
            // ================test adding finished====================
            
            
            double DeltaTimeAdd = (double) ExecTime / 1e6;
            printf(GREEN "\t·Тест добавления пройден за %lf мс" WHITE "\n", DeltaTimeAdd);
            
            fprintf(fp, "%lf,%lf,%lf\n", DeltaTimeRead, DeltaTimeFind, DeltaTimeAdd);

            if (Rep == NUM_OF_MEASURE_REPS - 1) {
                fflush(fp); // Чтобы DUMP_HMAP смог прочесть csv-файл из памяти диска
                DUMP_HMAP(  Hmap, AllHashFuncsStr[FuncIdx],
                            NumOfTestingWords, SumWordsRead, FuncIdx,
                            "%s/%s.html", HMAP_DUMP_DIR_NAME, AllHashFuncsStr[FuncIdx]);
            }
            
            HashMapDtor(Hmap);
        }

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    }
    // ===================tests end========================
    free(TestingWords);

    printf(MAGENTA "Все тесты проведены\n" WHITE);
    fclose(fp);
    return 0;
}