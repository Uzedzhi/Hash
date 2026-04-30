#ifndef HASHFUNCS_H
#define HASHFUNCS_H

typedef const char * const string;
extern "C" unsigned int MyStrlen_Hash(const char *str)  noexcept;
extern "C" unsigned int MyCRC32_Hash(const char *str)   noexcept;
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

#define IMG_PATH_NAME       "img"
#define LISTSIZES_PATH_NAME "listsizes"
#define LISTDUMP_PATH_NAME  "listdump"

#define X(n)                            \
    n(AlwaysZero_Hash       , 0)        \
    n(FirstAsciiChar_Hash   , 1)        \
    n(Strlen_Hash           , 2)        \
    n(AsciiSum_Hash         , 3)        \
    n(Rollleft_Hash         , 4)        \
    n(Rollright_Hash        , 5)        \
    n(SDBM_Hash             , 6)        \
    n(CRC32_Hash            , 7)        \
    n(FNV1A_Hash            , 8)        \
    n(MyStrlen_Hash         , 9)        \
    n(MyCRC32_Hash          , 10)    
#define INIT_POINTERS(FuncName, Ind) FuncName ,
#define INIT_STR(FuncName, Ind)     #FuncName ,
#define INIT_IMG_PATHS(FuncName, Ind)           IMG_PATH_NAME       "/" #FuncName ".png"  ,
#define INIT_LISTSIZES_PATHS(FuncName, Ind)     LISTSIZES_PATH_NAME "/" #FuncName ".txt"  ,
#define INIT_LISTDUMP_PATHS(FuncName, Ind)      LISTDUMP_PATH_NAME  "/" #FuncName ".html" ,
#define INIT_IND(FuncName, Ind) FuncName##_E = Ind,

const char * const ListSizePathStr[] = {X(INIT_LISTSIZES_PATHS)};
const char * const ListDumpPathStr[] = {X(INIT_LISTDUMP_PATHS)};
const char * const ImgPathStr[]      = {X(INIT_IMG_PATHS)};
enum AllHashFuncInd {
    X(INIT_IND)
};

unsigned int (*const AllHashFuncs[])(const char *) = {X(INIT_POINTERS)};
const char * const AllHashFuncsStr[] = {X(INIT_STR)};
const size_t NUM_OF_HASH_FUNCS = sizeof(AllHashFuncs) / sizeof(AllHashFuncs[0]);

string CSV_DUMP_FILE_NAME   = "tests/AllMeasurements.csv";
#ifndef SPEEDTESTS_FILE_NAME
#define SPEEDTESTS_FILE_NAME "tests/SpeedTest.csv"
#endif

#ifndef DUMP_FILE_NAME
#define DUMP_FILE_NAME "listdump/dump.html"
#endif // DUMP_FILE_NAME

#undef X
#undef INIT_POINTERS
#undef INIT_STR
#undef INIT_IMG_PATHS
#undef INIT_LISTSIZES_PATHS
#undef INIT_LISTDUMP_PATHS
#undef IMG_PATH_NAME      
#undef LISTSIZES_PATH_NAME
#undef LISTDUMP_PATH_NAME 

#endif // HASHFUNCS_H