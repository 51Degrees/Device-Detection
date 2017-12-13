GCCVERSION := $(shell expr `gcc -dumpversion | cut -f1 -d.` \< 5)

TRIE?=0

FILES = ../src/threading.c          \
        ../src/threading.h          \
        ../src/cityhash/city.h      \
        ../src/cityhash/city.c      \
        ../src/pattern/51Degrees.c  \
        ../src/pattern/51Degrees.h  \
        ../src/pattern/51Degrees.i  \
        ../src/pattern/Match.cpp    \
        ../src/pattern/Match.hpp    \
        ../src/pattern/Profiles.cpp \
        ../src/pattern/Profiles.hpp \
        ../src/pattern/Provider.cpp \
        ../src/pattern/Provider.hpp \
        ../src/flatdir/flatdir.h

TRIEFILES = ../src/threading.c       \
            ../src/threading.h       \
            ../src/cache.h           \
            ../src/cache.c           \
            ../src/trie/51Degrees.c  \
            ../src/trie/51Degrees.h  \
            ../src/trie/51Degrees.i  \
            ../src/trie/Match.cpp    \
            ../src/trie/Match.hpp    \
            ../src/trie/Provider.cpp \
            ../src/trie/Provider.hpp \
            ../src/flatdir/flatdir.h

ifeq "$(GCCVERSION)" "1"
        $(error gcc version 5.0.0 or higher is required ***)
else
ifeq "$(TRIE)" "0"
copyfiles:
	mkdir -p ./src/pattern
	cp $(FILES) ./src/pattern/
	swig -go -cgo -c++ -intgosize 64 src/pattern/51Degrees.i
else
copyfiles:
	mkdir -p ./src/trie
	cp $(TRIEFILES) ./src/trie/
	swig -go -cgo -c++ -intgosize 64 src/trie/51Degrees.i
endif
endif
