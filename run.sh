#!/bin/bash
SANITIZER_FLAGS="-fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr"
LINUX_FLAGS="-O3 -DNDEBUG -msse4.2 -mavx2"
LIBS_L_PATH="-L/mnt/c/Users/Azerty/my_project/my_libs -Wl,-rpath=/mnt/c/Users/Azerty/my_project/my_libs/"
INCLUDES_PATH="-I/mnt/c/Users/Azerty/my_project/my_libs/ -Iincludes/"
rm bin/hash
rm gmon.out
rm *.o

nasm -f elf64 src/MyStrLen.asm -o MyStrLen.o
g++ $LIBS_L_PATH $INCLUDES_PATH                                                         \
$LINUX_FLAGS -DINIT_FUNC_IDX=CRC32_Hash_E -DUSE_$1                                      \
    -c  src/list.cpp                src/hash.cpp            src/list_dump.cpp           \
        src/hashfuncs.cpp           src/hashdump.cpp        src/HashAssemblyNO.cpp      \
        ../my_libs/error_manage.cpp

g++ $LIBS_L_PATH $INCLUDES_PATH                                                         \
    $LINUX_FLAGS -DINIT_FUNC_IDX=CRC32_Hash_E -DUSE_$1                                  \
    *.o -o bin/hash

valgrind --tool=callgrind --callgrind-out-file=log.txt \
         ./bin/hash texts/usa.txt 
rm *.o