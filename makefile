CC = gcc

all:
	$(CC) -D HAVE_SNPRINTF -Wno-trigraphs src/pattern/PerfPat.c src/pattern/51Degrees.c src/snprintf/snprintf.c -o PerfPat
	$(CC) -D HAVE_SNPRINTF -Wno-trigraphs src/pattern/ProcPat.c src/pattern/51Degrees.c src/snprintf/snprintf.c -o ProcPat
	$(CC) -D HAVE_SNPRINTF -Wno-trigraphs src/console/Console.c src/pattern/51Degrees.c src/snprintf/snprintf.c -o Console
	$(CC) -D HAVE_SNPRINTF src/trie/PerfTrie.c src/trie/51Degrees.c src/snprintf/snprintf.c -lpthread -o PerfTrie
	$(CC) -D HAVE_SNPRINTF src/trie/ProcTrie.c src/trie/51Degrees.c src/snprintf/snprintf.c -o ProcTrie