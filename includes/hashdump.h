#ifndef HASHDUMP_H
#define HASHDUMP_H

#include <stddef.h>
#include "hash.h"

typedef struct {
    size_t HmapSize;
    size_t SumListSizes;
    size_t EmptyLists;
    size_t MinSize;
    size_t MaxSize;
    size_t MaxIdx;
    double Dispersion;
    double LoadFactor;
    double HiSquared;
} HmapStats_t; 

typedef struct {
    double ReadSpeed;
    double FindSpeed;
    double AddSpeed;
    double StdReadSpeed;
    double StdFindSpeed;
    double StdAddSpeed;
} SpeedStats_t; 

#define CSV_DUMP_PRINT_HEADER()
#define CSV_DUMP_PRINT_HMAP_PARAMS()

HmapError_t PrintListSizeToFile(Hashmap_t *Hmap, const char * FileName);
HmapError_t DumpHeaderToCsv();
double GetStdVal(double Array[NUM_OF_MEASURE_REPS], double MeanVal);
double GetMeanVal(double Array[NUM_OF_MEASURE_REPS]);
HmapError_t DumpHmapParamsToCsv();
HmapError_t DumpSpeedToCsv(size_t HashFuncIdx);
HmapError_t DumpHmapToHtml(Hashmap_t *Hmap, const char *FileName,
                           size_t NumTestingWords, size_t NumReadWords, size_t HashFuncIdx);


#endif // HASHDUMP_H