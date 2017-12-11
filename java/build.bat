set JAVA_INCLUDE=%JAVA_HOME%\include
set JAVA_WIN32=%JAVA_INCLUDE%\win32

mkdir obj
mkdir trie\src\main\java\FiftyOneDegreesTrieV3
mkdir trie\src\main\resources\FiftyOneDegreesTrieV3\
copy ..\src\trie\*.java trie\src\main\java\FiftyOneDegreesTrieV3\

cl /Fo:obj/ /Fe:trie/src/main/resources/FiftyOneDegreesTrieV3 /O2 /LD /EHsc ..\src\trie\51Degrees.c ..\src\threading.c ..\src\trie\Provider.cpp ..\src\trie\Match.cpp ..\src\trie\51Degrees_java.cpp /I "%JAVA_INCLUDE%" /I "%JAVA_WIN32%"