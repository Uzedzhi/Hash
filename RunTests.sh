#!/bin/bash
# AsciiSum_Hash_E Rollleft_Hash_E Rollright_Hash_E SDBM_Hash_E CRC32_Hash_E FNV1A_Hash_E FirstAsciiChar_Hash_E Strlen_Hash_E AlwaysZero_Hash_E

PROGRAM_RUN_CMD="./bin/hash texts/usa.txt"
INCLUDES_PATH="-Iincludes/ -I../my_libs/ -Imnt/c/users/azerty/my_project/my_libs/"
ALL_HASH_FUNCS="AsciiSum_Hash_E"
ALL_CPP_FILES="src/hashdump.cpp src/hashfuncs.cpp src/list.cpp src/list_dump.cpp src/HashAssembly.cpp src/hash.cpp"
TEST_DIR_NAME="PROFILER_TEST"
FLAGS="-O3 -mavx2 -msse4.2 -g -DDUMP_ENABLE -DNDEBUG"

run_profiler() {
    mkdir -p "callgrind/${TEST_DIR_NAME}"
    valgrind --tool=callgrind\
             --callgrind-out-file=callgrind/${TEST_DIR_NAME}/callgrind.out.$1 \
             $PROGRAM_RUN_CMD
}

run_speedtest() {
    mkdir -p "listdump/${TEST_DIR_NAME}"
    mkdir -p "hyperfine/${TEST_DIR_NAME}"
    hyperfine "$PROGRAM_RUN_CMD" --warmup 5 --runs 20 --export-csv hyperfine/${TEST_DIR_NAME}/$1.txt
}

run_dump() {
    mkdir -p "listdump/${TEST_DIR_NAME}"
    $PROGRAM_RUN_CMD
}

compile_nasm() {
    nasm -f elf64 src/FuncImpl/$1.asm -o $1.o
}

compile_file() {
    g++ $FLAGS src/FuncImpl/$1.cpp -c $1.o
}

rm_trash() {
    rm *.o
    rm bin/hash
}

make_obj() {
    rm_trash

    compile_nasm "MyCRC32_ASM"
    compile_nasm "MyFNV1A_ASM"
    compile_nasm "MySDBM_ASM"

    if [ $1 == "ASM" ]; then
        compile_nasm "MyStrlen_ASM"
        compile_nasm "MyStrcmp_ASM"
    elif [ $1 == "ASMINLINE" ]; then
        compile_file "MyStrcmp_ASMINLINE"
        compile_file "MyStrlen_ASMINLINE"
    fi

    g++ $LIBS_L_PATH $INCLUDES_PATH                     \
        $FLAGS -DUSE_$1                                 \
        -c $ALL_CPP_FILES
}

compile_program() {
    g++ $LIBS_L_PATH $INCLUDES_PATH                     \
        $FLAGS -DUSE_$2 -DINIT_FUNC_IDX=$1              \
        -c src/HashAssembly.cpp
    g++ $LIBS_L_PATH $INCLUDES_PATH                     \
        $FLAGS -DUSE_$2                                 \
        *.o -o bin/hash
}

make_obj "$1"
for FuncIdx in $ALL_HASH_FUNCS
do
    compile_program "$FuncIdx" "$1"
    echo -e                                                 \
    "\e[0;35m                                               \
    \n=================================================     \
            \n\tnow testing func $FuncIdx                   \
    \n=================================================     \
    \n\e[0m"

    run_dump        "$FuncIdx"
    # run_speedtest   "$FuncIdx"
    # run_profiler    "$FuncIdx"
done