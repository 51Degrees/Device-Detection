# 51Degrees Go API
# ----------------
# Makefile copies required sources to go/src directory and generates the swig 
# wrapper. This is to facilitate the building of Swig generated wrappers and 
# underlying C/C++ sources. The root /src/ directory contains other C/C++ 
# sources not pertaining to the Go implementation. 
# If building against the src directory in the repository root is attempted, 
# `go build` will fail as it will attempt to compile everything in the
# directory, including other wrappers files.

CMNFILES =  ../src/threading.c          \
            ../src/threading.h

CITYHASH =  ../src/cityhash/city.h      \
            ../src/cityhash/city.c

CACHE =     ../src/cache.h              \
            ../src/cache.c

PATFILES =  ../src/pattern/51Degrees.c  \
            ../src/pattern/51Degrees.h  \
            ../src/pattern/51Degrees.i  \
            ../src/pattern/51Degrees_go.cxx \
            ../src/pattern/FiftyOneDegreesPatternV3.go \
            ../src/pattern/Match.cpp    \
            ../src/pattern/Match.hpp    \
            ../src/pattern/Profiles.cpp \
            ../src/pattern/Profiles.hpp \
            ../src/pattern/Provider.cpp \
            ../src/pattern/Provider.hpp

TRIEFILES = ../src/trie/51Degrees.c     \
            ../src/trie/51Degrees.h     \
            ../src/trie/51Degrees.i     \
            ../src/trie/51Degrees_go.cxx \
            ../src/trie/FiftyOneDegreesTrieV3.go \
            ../src/trie/Match.cpp       \
            ../src/trie/Match.hpp       \
            ../src/trie/Provider.cpp    \
            ../src/trie/Provider.hpp
            
all: swig-pat swig-trie
.PHONY: all            
            
.PHONY: pattern
pattern:
	mkdir -p ./src/pattern
	mkdir -p ./src/cityhash
	cp $(PATFILES) ./src/pattern/
	cp $(CITYHASH) ./src/cityhash/
	cp $(CMNFILES) ./src/
	
swig-pat: pattern
	swig -go -cgo -c++ -intgosize 64 -o src/pattern/51Degrees_go.cxx src/pattern/51Degrees.i

.PHONY: trie
trie: 
	mkdir -p ./src/trie
	cp $(TRIEFILES) ./src/trie/
	cp $(CACHE) ./src/
	cp $(CMNFILES) ./src/
	
swig-trie: trie
	swig -go -cgo -c++ -intgosize 64 -o src/trie/51Degrees_go.cxx src/trie/51Degrees.i
	
hash: trie
swig-hash: swig-trie
