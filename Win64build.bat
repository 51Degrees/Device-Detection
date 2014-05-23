@echo off

if exist "%ProgramFiles(x86)%\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" goto :msvc64-12
if exist "%ProgramFiles%\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" goto :msvc86-12

if exist "%ProgramFiles(x86)%\Microsoft Visual Studio 11.0\VC\vcvarsall.bat" goto :msvc64-11
if exist "%ProgramFiles%\Microsoft Visual Studio 11.0\VC\vcvarsall.bat" goto :msvc86-11

if exist "%ProgramFiles(x86)%\Microsoft Visual Studio 10.0\VC\vcvarsall.bat" goto :msvc64-10
if exist "%ProgramFiles%\Microsoft Visual Studio 10.0\VC\vcvarsall.bat" goto :msvc86-10

goto :eof

:msvc64-10

call "%ProgramFiles(x86)%\Microsoft Visual Studio 10.0\VC\vcvarsall.bat" x64

goto :msvc

:msvc86-10

call "%ProgramFiles%\Microsoft Visual Studio 10.0\VC\vcvarsall.bat" x64

goto :msvc

:msvc64-11

call "%ProgramFiles(x86)%\Microsoft Visual Studio 11.0\VC\vcvarsall.bat" x64

goto :msvc

:msvc86-11

call "%ProgramFiles%\Microsoft Visual Studio 11.0\VC\vcvarsall.bat" x64

goto :msvc

:msvc64-12

call "%ProgramFiles(x86)%\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" x64

goto :msvc

:msvc86-12

call "%ProgramFiles%\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" x64

goto :msvc

:msvc

cl /DHAVE_SNPRINTF /D_CRT_SECURE_NO_WARNINGS /Ox /Ot /GL /MP src/console/Console.c src/pattern/51Degrees.c src/snprintf/snprintf.c
cl /DHAVE_SNPRINTF /D_CRT_SECURE_NO_WARNINGS /Ox /Ot /GL /MP src/pattern/ProcPat.c src/pattern/51Degrees.c src/snprintf/snprintf.c
cl /DHAVE_SNPRINTF /D_CRT_SECURE_NO_WARNINGS /Ox /Ot /GL /MP src/pattern/PerfPat.c src/pattern/51Degrees.c src/snprintf/snprintf.c
cl /DHAVE_SNPRINTF /D_CRT_SECURE_NO_WARNINGS /Ox /Ot /GL /MP src/trie/ProcTrie.c src/trie/51Degrees.c src/snprintf/snprintf.c

del *.obj

goto :eof

:eof
