@echo off

gcc -static -Ofast -DHAVE_SNPRINTF -DFIFTYONEDEGREES_NO_THREADING -march=corei7 -Wno-trigraphs ../src/console/Console.c ../src/pattern/51Degrees.c ../src/cityhash/city.c -o Console.exe
gcc -static -Ofast -DHAVE_SNPRINTF -DFIFTYONEDEGREES_NO_THREADING -march=corei7 -Wno-trigraphs ../src/pattern/ProcPat.c ../src/pattern/51Degrees.c ../src/cityhash/city.c -o ProcPat.exe
gcc -static -Ofast -DHAVE_SNPRINTF -DFIFTYONEDEGREES_NO_THREADING -march=corei7 -Wno-trigraphs ../src/pattern/PerfPat.c ../src/pattern/51Degrees.c ../src/cityhash/city.c -o PerfPat.exe
gcc -static -Ofast -DHAVE_SNPRINTF -D_FILE_OFFSET_BITS=64 -march=corei7 ../src/trie/ProcTrie.c ../src/trie/51Degrees.c -o ProcTrie.exe
gcc -static -Ofast -DHAVE_SNPRINTF -DFIFTYONEDEGREES_NO_THREADING -D_FILE_OFFSET_BITS=64 -march=corei7 ../src/trie/PerfTrie.c ../src/trie/51Degrees.c -o PerfTrie.exe

:eof
