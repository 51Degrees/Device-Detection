# A skeleton CMake Build file
# Builds some basic C examples
cmake_minimum_required(VERSION 3.4)
project(Device_Detection)

#dmalloc installs here by default
link_directories(/usr/local/lib)
include_directories(/usr/local/include)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SNPRINTF src/snprintf/snprintf.c
        src/snprintf/snprintf.h)

set(TRIE src/trie/51Degrees.c
        src/trie/51Degrees.h)

set(CITYHASH
        src/cityhash/city.c
        src/cityhash/city.h)

set(PATTERN src/pattern/51Degrees.c
        src/pattern/51Degrees.h
        ${CITYHASH})

set(THREADING
        src/threading.h
        src/threading.c)

add_executable(ReloadFromFile
        examples/ReloadFromFile.c
        ${THREADING}
        ${PATTERN})

target_link_libraries(ReloadFromFile dmalloc)
target_compile_options(ReloadFromFile PRIVATE -D_DEBUG)

add_executable(ReloadFromFileTrie
        examples/ReloadFromFileTrie.c
        ${TRIE}
        ${THREADING})

target_link_libraries(ReloadFromFileTrie dmalloc)
target_compile_options(ReloadFromFileTrie PRIVATE -D_DEBUG)

add_executable(PerfPat
        src/pattern/PerfPat.c
        ${THREADING}
        ${PATTERN}
        )
add_executable(ProcPat
        src/pattern/ProcPat.c
        ${THREADING}
        ${PATTERN}
        )
add_executable(PerfTrie
        src/trie/PerfTrie.c
        ${THREADING}
        ${TRIE}
        )
add_executable(ProcTrie
        src/trie/ProcTrie.c
        ${THREADING}
        ${TRIE}
        )
