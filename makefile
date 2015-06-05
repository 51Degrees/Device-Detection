all:
	gcc -O3 -D HAVE_SNPRINTF -Wno-trigraphs src/pattern/PerfPat.c src/pattern/51Degrees.c src/snprintf/snprintf.c src/cityhash/city.c -o PerfPat
    gcc -O3 -D HAVE_SNPRINTF -Wno-trigraphs src/pattern/ProcPat.c src/pattern/51Degrees.c src/snprintf/snprintf.c src/cityhash/city.c -o ProcPat
    gcc -O3 -D HAVE_SNPRINTF -Wno-trigraphs src/console/Console.c src/pattern/51Degrees.c src/snprintf/snprintf.c src/cityhash/city.c -o Console
    gcc -O3 -D HAVE_SNPRINTF src/trie/PerfTrie.c src/trie/51Degrees.c src/snprintf/snprintf.c src/cityhash/city.c -lpthread -o PerfTrie
    gcc -O3 -D HAVE_SNPRINTF src/trie/ProcTrie.c src/trie/51Degrees.c src/snprintf/snprintf.c src/cityhash/city.c -o ProcTrie