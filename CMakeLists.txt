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

target_link_libraries(ReloadFromFile dmallocth pthread m)
target_compile_options(ReloadFromFile PRIVATE -D_DEBUG)

add_executable(ReloadFromFileTrie
        examples/ReloadFromFileTrie.c
        ${TRIE}
        ${THREADING})

target_link_libraries(ReloadFromFileTrie dmallocth pthread)
target_compile_options(ReloadFromFileTrie PRIVATE -D_DEBUG)

add_executable(ReloadFromMemory
        examples/ReloadFromMemory.c
        ${PATTERN}
        ${THREADING})

target_link_libraries(ReloadFromMemory dmallocth pthread m)
target_compile_options(ReloadFromMemory PRIVATE -D_DEBUG)

add_executable(ReloadFromMemoryTrie
        examples/ReloadFromMemoryTrie.c
        ${TRIE}
        ${THREADING})

target_link_libraries(ReloadFromMemoryTrie dmallocth pthread)
target_compile_options(ReloadFromMemoryTrie PRIVATE -D_DEBUG)

add_executable(GettingStarted
        examples/GettingStarted.c
        ${PATTERN}
        ${THREADING})

target_link_libraries(GettingStarted dmalloc m)
target_compile_options(GettingStarted PRIVATE -D_DEBUG)

add_executable(GettingStartedTrie
        examples/GettingStartedTrie.c
        ${TRIE}
        ${THREADING})

target_link_libraries(GettingStartedTrie dmalloc)
target_compile_options(GettingStartedTrie PRIVATE -D_DEBUG)

add_executable(MatchForDeviceId
        examples/MatchForDeviceId.c
        ${PATTERN}
        ${THREADING})

target_link_libraries(MatchForDeviceId dmalloc m)
target_compile_options(MatchForDeviceId PRIVATE -D_DEBUG)

add_executable(MatchMetrics
        examples/MatchMetrics.c
        ${PATTERN}
        ${THREADING})

target_link_libraries(MatchMetrics dmalloc m)
target_compile_options(MatchMetrics PRIVATE -D_DEBUG)

add_executable(OfflineProcessing
        examples/OfflineProcessing.c
        ${PATTERN}
        ${THREADING})

target_link_libraries(OfflineProcessing dmalloc m)
target_compile_options(OfflineProcessing PRIVATE -D_DEBUG)

add_executable(FindProfiles
        examples/FindProfiles.c
        ${PATTERN}
        ${THREADING})

target_link_libraries(FindProfiles dmalloc m)
target_compile_options(FindProfiles PRIVATE -D_DEBUG)

add_executable(StronglyTyped
        examples/StronglyTyped.c
        ${PATTERN}
        ${THREADING})

target_link_libraries(StronglyTyped dmalloc m)
target_compile_options(StronglyTyped PRIVATE -D_DEBUG)

add_executable(PerfPat
        src/pattern/PerfPat.c
        ${THREADING}
        ${PATTERN}
        )
target_link_libraries(PerfPat m pthread)

add_executable(ProcPat
        src/pattern/ProcPat.c
        ${THREADING}
        ${PATTERN}
        )
target_link_libraries(ProcPat m)

add_executable(PerfTrie
        src/trie/PerfTrie.c
        ${THREADING}
        ${TRIE}
        )
target_link_libraries(PerfTrie pthread)

add_executable(ProcTrie
        src/trie/ProcTrie.c
        ${THREADING}
        ${TRIE}
        )
