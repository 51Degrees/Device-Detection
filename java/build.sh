#!/bin/sh
mkdir -p trie/src/main/java/FiftyOneDegreesTrieV3/
mkdir -p trie/src/main/resources/FiftyOneDegreesTrieV3/
cp ../src/trie/*.java trie/src/main/java/FiftyOneDegreesTrieV3/

mkdir obj
gcc -c -fPIC -O3 ../src/trie/51Degrees.c -o obj/51Degrees.o
gcc -c -fPIC -O3 ../src/threading.c -o obj/threading.o
g++ -c -fPIC -O3 ../src/trie/Provider.cpp -o obj/Provider.o
g++ -c -fPIC -O3 ../src/trie/Match.cpp -o obj/Match.o
g++ -c -fPIC -O3 ../src/trie/51Degrees_java.cpp -o obj/51Degrees_Java.o -I $JAVA_HOME/include -I $JAVA_HOME/include/linux -I $JAVA_HOME/include/darwin
g++ -shared obj/*.o -O3 -o trie/src/main/resources/FiftyOneDegreesTrieV3.so
