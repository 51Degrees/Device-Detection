all:
	gcc -pthread -O3 -D HAVE_SNPRINTF -Wno-trigraphs src/pattern/PerfPat.c src/pattern/51Degrees.c src/cityhash/city.c -o PerfPat
	gcc -pthread -O3 -D HAVE_SNPRINTF -Wno-trigraphs src/pattern/ProcPat.c src/pattern/51Degrees.c src/cityhash/city.c -o ProcPat
	gcc -pthread -O3 -D HAVE_SNPRINTF -Wno-trigraphs src/console/Console.c src/pattern/51Degrees.c src/cityhash/city.c -o Console
	gcc -pthread -O3 -D HAVE_SNPRINTF src/trie/PerfTrie.c src/trie/51Degrees.c -lpthread src/cityhash/city.c -o PerfTrie
	gcc -pthread -O3 -D HAVE_SNPRINTF src/trie/ProcTrie.c src/trie/51Degrees.c src/cityhash/city.c -o ProcTrie
