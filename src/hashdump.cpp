#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#include "hashdump.h"
#include "list.h"
#include "hash.h"
#include "hashfuncs.h"
#include "../my_libs/sassert.h"

static size_t CurUniquePicId_gl = 0;

void PrintHTMLHeaders(FILE *fp) {
    sassert(fp, ERR_PTR_NULL);
    fprintf(fp, "<!DOCTYPE html>\n"
            "<html>\n"
            "<head>\n"
            "<meta charset=\"utf-8\">\n"
            "<title>Hash Map Dump</title>\n"
            "<style>\n"
                "body {\n"
                    "color: #222222;\n"
                    "padding: 2vw;\n"
                    "margin: 0;\n"
                "}\n"
                ".stats {\n"
                    "background: #d4e9d1;\n"
                    "border: 7px solid #0c65155a;\n"
                    "border-radius: 10px;\n"
                    "margin: 10px;\n"
                    "padding: 10px;\n"
                    "margin-bottom: 30px;\n"
                "}\n"
                ".stats h2 {\n"
                    "text-align: center;\n"
                    "font-size: 40px;\n"
                    "padding: 0px;\n"
                    "margin: 0px;\n"
                "}\n"
                ".stats h3 {\n"
                    "padding: 0px;\n"
                    "margin: 0px;\n"
                "}\n"
                ".value {\n"
                    "display: inline;\n"
                "}\n"
                ".det {\n"
                    "background: #fbffdbab;\n"
                    "border: 2px solid #737947ab;\n"
                    "margin-bottom: 6px;\n"
                    "padding: 6px;\n"
                    "border-radius: 4px;\n"
                "}\n"
                "summary {\n"
                    "font-weight: 600;\n"
                "}\n"
                ".ListExt {\n"
                    "display: flex;\n"
                    "flex-wrap: wrap;\n"
                    "align-items: center;\n"
                    "padding: 10px 5px 5px 15px;\n"
                    "gap: 8px;\n"
                "}\n"
                ".node {\n"
                    "background: #e3f2fd;\n"
                    "border: 1px solid #bbdefb;\n"
                    "padding: 4px 10px;\n"
                    "border-radius: 4px;\n"
                    "color: #0d47a1;\n"
                    "font-size: 14px;\n"
                "}\n"
                ".arrow {\n"
                    "color: #868585;\n"
                    "font-weight: bold;\n"
                    "font-size: 16px;\n"
                "}\n"
                ".tag-container {\n"
                    "display: grid;\n"
                    "grid-template-columns: repeat(4, 1fr); \n"
                    "gap: 10px;\n"
                    "text-align: center;\n"
                "}\n"
                ".tiny {\n"
                    "font-size: 10px;\n"
                    "color:     #858585;\n"
                    "text-align: center;\n"
                    "margin: 5px;\n"
                "}\n"
                ".stats-speed {\n"
                    "background: #d4e9d1;\n"
                    "border: 7px solid #0c65155a;\n"
                    "border-radius: 10px;\n"
                    "margin: 10px;\n"
                    "padding: 10px;\n"
                    "margin-bottom: 30px;\n"
                "}\n"
                ".stats-speed h2 {\n"
                    "text-align: center;\n"
                    "font-size: 40px;\n"
                    "padding: 0px;\n"
                    "margin: 0px;\n"
                "}\n"
                ".stats-speed h3 {\n"
                    "padding: 0px;\n"
                    "margin: 0px;\n"
                "}\n"
                ".tag-container-speed {\n"
                    "display: grid;\n"
                    "grid-template-columns: repeat(3, 1fr); \n"
                    "gap: 10px;\n"
                    "text-align: center;\n"
                "}\n"
            "</style>\n"
            "</head>\n"
            "<body>\n"
        );
}

double GetHiSquared(size_t *SizeArr,size_t Size, size_t Mean) {
    sassert(SizeArr, ERR_PTR_NULL_HMAP);

    double hiSquare = 0.0;
    for (size_t i = 0; i < Size; i++) {
        double Diff = SizeArr[i] - (double) Mean;
        hiSquare += (Diff * Diff);
    }

    return hiSquare / Mean;
}

HmapError_t GetHmapStats(HmapStats_t * Stats, size_t HashFuncIdx) {
    sassert(Stats,  ERR_PTR_NULL_HMAP);

    FILE *fp = fopen(AllHashFuncs[HashFuncIdx].SizeDumpStr, "r");
    RET_ASSERT(fp, ERR_PTR_NULL_HMAP,
                "не удалось открыть файл %s", AllHashFuncs[HashFuncIdx].SizeDumpStr);

    size_t FileSize  = GetFileSize(fp);
    char *FileBuffer = GetBufferFromFile(fp, FileSize);
    char *Pos  = FileBuffer;
    char *Next = FileBuffer;
    fclose(fp);

    size_t HmapSize = strtol(Pos, &Next, 10);
    RET_ASSERT(HmapSize && HmapSize < HMAP_MAX_SIZE && Pos != Next, ERR_INVALID_SYNTAX_HMAP,
                   "неправильный синтаксис файла %s", AllHashFuncs[HashFuncIdx].SizeDumpStr);
    Pos = Next + 1;
    
    size_t *SizeArr = (size_t *) calloc(HmapSize, sizeof(size_t));
    for (size_t i = 0; i < HmapSize; i++) {
        long size = strtol(Pos, &Next, 10);
        RET_ASSERT(Pos != Next, ERR_INVALID_SYNTAX_HMAP,
                   "неправильный синтаксис файла %s", AllHashFuncs[HashFuncIdx].SizeDumpStr);
        Pos = Next + 1;

        SizeArr[i] = size;
    }

    size_t Empty        = 0;
    size_t Sum          = 0;
    size_t SumSquares   = 0;
    size_t MaxSize      = 0;
    size_t MinSize      = -1;
    size_t MaxIdx       = 0;

    for (size_t i = 0; i < HmapSize; i++) {
        size_t Size = SizeArr[i];

        if (Size == 0) {
            Empty++;
            MinSize = 0;
            continue;
        }

        SumSquares += Size * Size;
        Sum += Size;
        MaxIdx = i;
        
        if (Size > MaxSize)
            MaxSize = Size;
        if (Size < MinSize)
            MinSize = Size;
    }

    double Mean         = (double) Sum / HmapSize;
    double MeanSquares  = (double) SumSquares / HmapSize;

    Stats->Dispersion   = MeanSquares - Mean * Mean;
    Stats->SumListSizes = Sum;
    Stats->EmptyLists   = Empty;
    Stats->LoadFactor   = (double) Stats->SumListSizes / HmapSize;
    Stats->MaxSize      = MaxSize;
    Stats->MinSize      = MinSize;
    Stats->MaxIdx       = MaxIdx;
    Stats->HiSquared    = GetHiSquared(SizeArr, HmapSize, Mean);
    
    free(SizeArr);
    free(FileBuffer);

    return OK_HMAP;
}

double GetMeanVal(double Array[NUM_OF_MEASURE_REPS]) {
    sassert(Array,      ERR_PTR_NULL_HMAP);

    double Sum = 0;
    for (size_t i = 0; i < NUM_OF_MEASURE_REPS; i++) {
        Sum += Array[i];
    }

    return Sum / (double) NUM_OF_MEASURE_REPS;
}

double GetStdVal(double Array[NUM_OF_MEASURE_REPS], double MeanVal) {
    sassert(Array, ERR_PTR_NULL_HMAP);

    double Dispersion = 0.f;
    for (size_t i = 0; i < NUM_OF_MEASURE_REPS; i++) {
        Dispersion += (Array[i] - MeanVal) * (Array[i] - MeanVal);
    }
    Dispersion /= (double) (NUM_OF_MEASURE_REPS);
    
    return sqrt(Dispersion);
}

HmapError_t GetSpeedStats(SpeedStats_t *Stats) {
    sassert(Stats,    ERR_PTR_NULL_HMAP);

    FILE *fp = fopen(SPEEDTESTS_FILE_NAME, "r");
    RET_ASSERT(fp, ERR_PTR_NULL_HMAP,
                "не удалось открыть файл %s", SPEEDTESTS_FILE_NAME);

    fscanf(fp,  "command,mean,stddev,median,user,system,min,max\n"
                "%*[^,],%lf,%lf,%lf,%lf,%lf,%lf,%lf",   &(Stats->mean), &(Stats->std), &(Stats->median),
                                                        &(Stats->user), &(Stats->SystemTime),
                                                        &(Stats->MinTime), &(Stats->MaxTime));
    fclose(fp);

    return OK_HMAP;
}

void PrintGnuplotHeaderInFile(FILE *fp, const char * file_name, int MaxIdx, size_t HashFuncIdx) {
    sassert(fp, ERR_PTR_NULL);

    fprintf(fp, "set style data boxes\n");
    fprintf(fp, "set boxwidth 0.8 relative\n");
    fprintf(fp, "set style fill solid 1.0 noborder\n");
    fprintf(fp, "set terminal pngcairo size 1800,600\n");
    fprintf(fp, "set output '%s'\n", file_name);
    fprintf(fp, "set grid\n");
    fprintf(fp, "set title \"%s\" font \",20\" noenhanced\n", AllHashFuncs[HashFuncIdx].Str);
    fprintf(fp, "set xzeroaxis lt 1 lw 1 lc rgb \"black\"\n");
    fprintf(fp, "set yzeroaxis lt 1 lw 1 lc rgb \"black\"\n");
    fprintf(fp, "set xrange [%d:%d]\n", -VIEW_DISTANCE, MaxIdx + VIEW_DISTANCE);
}

void PrintGnuplotHistOfHmap(Hashmap_t *Hmap, size_t HashFuncIdx, int MaxIdx) {
    sassert(Hmap, ERR_PTR_NULL);

    FILE *fp = popen("gnuplot", "w");
    sassert(fp, ERR_PTR_NULL);

    PrintGnuplotHeaderInFile(fp, AllHashFuncs[HashFuncIdx].ImgDumpStr, MaxIdx, HashFuncIdx);
    fprintf(fp, "plot \"%s\" skip 1 title \"\" \n", AllHashFuncs[HashFuncIdx].SizeDumpStr);

    pclose(fp);
}

HmapError_t DumpToAllMeasurementsCsv(size_t HashFuncIdx, HmapStats_t HmapStats, SpeedStats_t SpeedStats) {
    const char *GroupStart = strchr(SPEEDTESTS_FILE_NAME, '/');
    if (!GroupStart)
        return OK_HMAP;
    
    const char *FileStart = strchr(GroupStart + 1, '/');
    if (!FileStart)
        return OK_HMAP;
    
    char Group[MAX_STR_SIZE] = {};
    strncpy(Group, GroupStart + 1, FileStart - GroupStart - 1);
    
    FILE *fp = fopen(CSV_DUMP_FILE_NAME, "a");
    RET_ASSERT(fp, ERR_PTR_NULL_HMAP, 
               "не удалось открыть %s", CSV_DUMP_FILE_NAME);

    fseek(fp, 0, SEEK_END);
    if (ftell(fp) == 0)
        fprintf(fp, "Группа,Имя Функции,Время выполнения,Мин. длина, Макс. длина, Дисперсия, Load Factor, Хи-квадрат\n");

    fprintf(fp, "%s,%s,%lf,%lf,%zu,%zu,%lf,%lf,%lf\n", 
                Group, FileStart + 1,
                SpeedStats.mean, SpeedStats.std,
                HmapStats.MinSize, HmapStats.MaxSize, HmapStats.Dispersion,
                HmapStats.LoadFactor, HmapStats.HiSquared);
            
    fclose(fp);
    return OK_HMAP;
}

HmapError_t DumpHmapToHtml(Hashmap_t *Hmap, size_t NumTestingWords,
                           size_t NumReadWords, size_t HashFuncIdx) {
    sassert(Hmap, ERR_PTR_NULL_HMAP);

    FILE *fp = fopen(DUMP_FILE_NAME, "w");
    RET_ASSERT(fp, ERR_PTR_NULL_HMAP, 
               "не удалось открыть файл: %s", DUMP_FILE_NAME);

    PrintHTMLHeaders(fp);
    PrintListSizeToFile(Hmap, AllHashFuncs[HashFuncIdx].SizeDumpStr);

    HmapStats_t HmapStats = {};
    GetHmapStats(&HmapStats, HashFuncIdx);

    SpeedStats_t SpeedStats = {};
    GetSpeedStats(&SpeedStats);

    DumpToAllMeasurementsCsv(HashFuncIdx, HmapStats, SpeedStats);
    fprintf(fp, "<div class=\"stats\">\n");
    fprintf(fp, "<h2>Дамп функции %s</h2>\n", AllHashFuncs[HashFuncIdx].Str);
    fprintf(fp, "<div class=\"tag-container\">\n");
    fprintf(fp, "<h3>Размер таблицы:        <br>  %zu</h3>\n",           Hmap->size);
    fprintf(fp, "<h3>Всего элементов:       <br>  %zu</h3>\n",           HmapStats.SumListSizes);
    fprintf(fp, "<h3>Пустых Списков:        <br>  %zu (%.3f %%)</h3>\n", HmapStats.EmptyLists, (double) HmapStats.EmptyLists / Hmap->size * 100.f);
    fprintf(fp, "<h3>Load Factor(среднее):  <br>  %.3f</h3>\n",          HmapStats.LoadFactor);
    fprintf(fp, "<h3>Минимальное значение:  <br>  %zu</h3>\n",           HmapStats.MinSize);
    fprintf(fp, "<h3>Максимальное значение: <br>  %zu</h3>\n",           HmapStats.MaxSize);
    fprintf(fp, "<h3>Дисперсия:             <br>  %.3f</h3>\n",          HmapStats.Dispersion);
    fprintf(fp, "<h3>Хи-Квадрат:  <br>  %.3f</h3>\n",                    HmapStats.HiSquared);
    fprintf(fp, "</div></div>\n\n");

    fprintf(fp, "<div class=\"stats-speed\">\n");
    fprintf(fp, "<h2>Дамп Скорости %s</h2>\n", AllHashFuncs[HashFuncIdx].Str);
    fprintf(fp, "<div class=\"tag-container-speed\">\n");
    fprintf(fp, "<h3>Среднее время выполнения:          <br>    %.3lf ± %.3lf</h3>\n", SpeedStats.mean, SpeedStats.std);
    fprintf(fp, "<h3>мединное время выполнения:         <br>    %.3lf</h3>\n", SpeedStats.median);
    fprintf(fp, "<h3>Количество итераций:               <br>    20</h3>\n");
    fprintf(fp, "</div></div>\n\n");

    PrintGnuplotHistOfHmap(Hmap, HashFuncIdx, HmapStats.MaxIdx);
    fprintf(fp, "<img src=\"../../%s\" alt=\"гистограмма распределения размеров списков\">\n", AllHashFuncs[HashFuncIdx].ImgDumpStr);

    size_t ZeroListsCount = 0;
    for (size_t i = 0; i < Hmap->size; i++) {
        size_t ListSize = Hmap->Table[i]->size;
        if (ListSize == 0) {
            ZeroListsCount++;
            continue;
        }
        else if (ZeroListsCount != 0) {
            fprintf(fp, "<p class=\"tiny\">%zu пустых</p>\n", ZeroListsCount);
            ZeroListsCount = 0;
        }

        fprintf(fp, "<details class=\"det\">\n");
        fprintf(fp, "<summary>[%zu] : %zu элементов</summary>", i, ListSize);
        
        fprintf(fp, "<div class=\"ListExt\">\n");
        bucket_t *Bucket = Hmap->Table[i];
        for (size_t i = 0; i < Bucket->size; i++) {
            char *val = Bucket->Str[i];
            
            if (val == POISON)
                fprintf(fp, "<span class=\"node\" style=\"color:red\">POISON</span>\n");
            else
                fprintf(fp, "<span class=\"node\">%s</span>\n", val);
            
            if (i != Bucket->size - 1)
                fprintf(fp, "<span class=\"arrow\">⇒</span>\n");
        }
        
        fprintf(fp, "</div>\n"
                    "</details>\n");
    }

    if (ZeroListsCount != 0)
        fprintf(fp, "<p class=\"tiny\">далее %zu пустых</p>\n", ZeroListsCount);

    fprintf(fp, "</body>\n"
                "</html>\n");
    fclose(fp);
    
    return OK_HMAP;
}
