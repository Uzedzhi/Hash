#!/bin/bash
SANITIZER_FLAGS="-fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr"
PROFILER_FLAGS="-O2 -DNDEBUG -msse4.2 -mavx2"
SPEED_FLAGS="-O3 -DNDEBUG -msse4.2 -mavx2"
LIBS_L_PATH="-L/mnt/c/Users/Azerty/my_project/my_libs -Wl,-rpath=/mnt/c/Users/Azerty/my_project/my_libs/"
INCLUDES_PATH="-I/mnt/c/Users/Azerty/my_project/my_libs/ -Iincludes/"

ALL_CPP_FILES=" src/list.cpp                src/hash.cpp            src/list_dump.cpp       \
                src/hashfuncs.cpp           src/hashdump.cpp        src/HashAssembly.cpp  \
                ../my_libs/error_manage.cpp"

ALL_HASH_FUNCS="AsciiSum_Hash_E Rollleft_Hash_E Rollright_Hash_E SDBM_Hash_E CRC32_Hash_E FNV1A_Hash_E AlwaysZero_Hash_E FirstAsciiChar_Hash_E Strlen_Hash_E"
MODDED_HASH_FUNCS="StrlenIntrinsic_Hash_E CRC32Intrinsic_Hash_E"
PROGRAM_RUN_COMMAND="./bin/hash texts/usa.txt"

# ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
# ++++++++++++++++++Repeat with 3 types of program++++++++++++++++++
# ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
for TYPE in NOP STRCMPOP ALLOP
do
    if [ "$TYPE" = "ALLOP" ]; then
    CUR_HASH_FUNCS=$MODDED_HASH_FUNCS
    else
    CUR_HASH_FUNCS=$ALL_HASH_FUNCS
    fi
    # ==================================================================
    # ==============1st uses valgrind to profile the program============
    # ================2nd uses hyperfine to test its speed==============
    # =======3rd runs through the program but dumps every hash func=====
    # ==================================================================
    for FuncIdx in $CUR_HASH_FUNCS
    do
        rm bin/hash
        nasm -f elf64 src/MyStrLen.asm -o MyStrLen.o
        
        g++ $LIBS_L_PATH $INCLUDES_PATH                                                         \
            $PROFILER_FLAGS -DINIT_FUNC_IDX=$FuncIdx -DUSE_$1                                   \
            -c $ALL_CPP_FILES
        
        g++ $LIBS_L_PATH $INCLUDES_PATH                                                         \
            $PROFILER_FLAGS -DINIT_FUNC_IDX=$FuncIdx -DUSE_$1                                   \
            *.o -o bin/hash
        
        valgrind --tool=callgrind --callgrind-out-file=callgrind/${TYPE}/callgrind.out.${TYPE}_${FuncIdx} $PROGRAM_RUN_COMMAND
        hyperfine "$PROGRAM_RUN_COMMAND" --warmup 5 --runs 20 --export-markdown results_${TYPE}_${FuncIdx}.md
        $PROGRAM_RUN_COMMAND -DDUMP_HASH
        rm *.o
    done
done