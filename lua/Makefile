LUAINCLUDE := /usr/include/lua5.2
LUAMODULEDIR := /usr/local/lib/lua/5.2/

build-pattern:
	if [ ! -d "pattern/obj" ]; then mkdir -p pattern/obj; fi
	if [ ! -f "../src/pattern/51Degrees_lua.cpp" ]; then \
		swig -c++ -lua -nomoduleglobal -o ../src/pattern/51Degrees_lua.cpp ../src/pattern/51Degrees.i; \
	fi
	gcc -c -fPIC ../src/pattern/51Degrees.c -o pattern/obj/51Degrees.o -lm
	gcc -c -fPIC ../src/threading.c -o pattern/obj/threading.o
	gcc -c -fPIC ../src/cityhash/city.c -o pattern/obj/city.o
	g++ -c -fPIC ../src/pattern/51Degrees_lua.cpp -o pattern/obj/51Degrees_lua.o -I $(LUAINCLUDE)
	g++ -c -fPIC ../src/pattern/Match.cpp -o pattern/obj/Match.o -DSWIGLUA
	g++ -c -fPIC ../src/pattern/Provider.cpp -o pattern/obj/Provider.o
	g++ -c -fPIC ../src/pattern/Profiles.cpp -o pattern/obj/Profiles.o

	g++ -shared -I /usr/include/lua5.2 pattern/obj/* -o pattern/FiftyOneDegreesPatternV3.so

build-trie:
	if [ ! -d "trie/obj" ]; then mkdir -p trie/obj; fi
	if [ ! -f "../src/pattern/51Degrees_lua.cpp" ]; then \
		swig -c++ -lua -nomoduleglobal -o ../src/trie/51Degrees_lua.cpp ../src/trie/51Degrees.i; \
	fi
	gcc -c -fPIC ../src/trie/51Degrees.c -o trie/obj/51Degrees.o -lm
	gcc -c -fPIC ../src/threading.c -o pattern/obj/threading.o
	g++ -c -fPIC ../src/trie/51Degrees_lua.cpp -o trie/obj/51Degrees_lua.o -I $(LUAINCLUDE)
	g++ -c -fPIC ../src/trie/Match.cpp -o trie/obj/Match.o -DSWIGLUA
	g++ -c -fPIC ../src/trie/Provider.cpp -o trie/obj/Provider.o

	g++ -shared -I /usr/include/lua5.2 trie/obj/* -o trie/FiftyOneDegreesTrieV3.so


install: build-pattern build-trie
    mkdir --parents $(LUAMODULEDIR)
	sudo cp pattern/FiftyOneDegreesPatternV3.so $(LUAMODULEDIR)
	sudo cp trie/FiftyOneDegreesTrieV3.so $(LUAMODULEDIR)
