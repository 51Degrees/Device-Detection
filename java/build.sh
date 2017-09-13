#!/bin/sh
mkdir -p trie/src/main/java/FiftyOneDegreesTrieV3/
mkdir -p trie/src/main/resources/FiftyOneDegreesTrieV3/
cp ../src/trie/*.java trie/src/main/java/FiftyOneDegreesTrieV3/

mkdir obj
gcc -c -fPIC ../src/trie/51Degrees.c -o obj/51Degrees.o
gcc -c -fPIC ../src/threading.c -o obj/threading.o
g++ -c -fPIC ../src/trie/Provider.cpp -o obj/Provider.o
g++ -c -fPIC ../src/trie/Match.cpp -o obj/Match.o
g++ -c -fPIC ../src/trie/51Degrees_java.cpp -o obj/51Degrees_Java.o -I $JAVA_HOME/include -I $JAVA_HOME/include/linux
g++ -shared obj/*.o -o trie/src/main/resources/FiftyOneDegreesTrieV3.so
