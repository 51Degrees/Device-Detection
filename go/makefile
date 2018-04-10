TRIE?=0

COMMONFILES =   ../src/threading.c          \
                ../src/threading.h          \

CITYHASH =  ../src/cityhash/city.h      \
            ../src/cityhash/city.c      \

CACHE = ../src/cache.h           \
        ../src/cache.c           \

PATFILES =  ../src/pattern/51Degrees.c  \
            ../src/pattern/51Degrees.h  \
            ../src/pattern/51Degrees.i  \
            ../src/pattern/Match.cpp    \
            ../src/pattern/Match.hpp    \
            ../src/pattern/Profiles.cpp \
            ../src/pattern/Profiles.hpp \
            ../src/pattern/Provider.cpp \
            ../src/pattern/Provider.hpp

TRIEFILES = ../src/trie/51Degrees.c  \
            ../src/trie/51Degrees.h  \
            ../src/trie/51Degrees.i  \
            ../src/trie/Match.cpp    \
            ../src/trie/Match.hpp    \
            ../src/trie/Provider.cpp \
            ../src/trie/Provider.hpp

ifeq "$(TRIE)" "0"
copyfiles:
	mkdir -p ./src/pattern
	mkdir -p ./src/cityhash
	cp $(PATFILES) ./src/pattern/
	cp $(CITYHASH) ./src/cityhash/
	cp $(COMMONFILES) ./src/
	swig -go -cgo -c++ -intgosize 64 src/pattern/51Degrees.i
else
copyfiles:
	mkdir -p ./src/trie
	cp $(TRIEFILES) ./src/trie/
	cp $(CACHE) ./src/
	cp $(COMMONFILES) ./src/
	swig -go -cgo -c++ -intgosize 64 src/trie/51Degrees.i
endif

