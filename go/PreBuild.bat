@ECHO OFF

REM Reset vars.
SET PATTERN=false
SET TRIE=false
SET PATWRAPER=false
SET TRIEWRAPER=false

REM File listings.
SET COMMONFILES=(..\src\threading.c ..\src\threading.h)
SET CITYHASH=(..\src\cityhash\city.h ..\src\cityhash\city.c)
SET CACHE=(..\src\cache.h ..\src\cache.c)
SET PATFILES=(..\src\pattern\51Degrees.c ..\src\pattern\51Degrees.h ..\src\pattern\51Degrees.i ..\src\pattern\51Degrees_go.cxx ..\src\pattern\FiftyOneDegreesPatternV3.go ..\src\pattern\Match.cpp ..\src\pattern\Match.hpp ..\src\pattern\Profiles.cpp ..\src\pattern\Profiles.hpp ..\src\pattern\Provider.cpp ..\src\pattern\Provider.hpp)
SET TRIEFILES=(..\src\trie\51Degrees.c ..\src\trie\51Degrees.h ..\src\trie\51Degrees.i ..\src\trie\51Degrees_go.cxx ..\src\trie\FiftyOneDegreesTrieV3.go ..\src\trie\Match.cpp ..\src\trie\Match.hpp ..\src\trie\Provider.cpp ..\src\trie\Provider.hpp)


REM Set up pattern algoritm by default.
IF "%~1" == "" SET PATTERN=true
IF "%~1" == "PATTERN" SET PATTERN=true

IF "%~1" == "TRIE" SET TRIE=true
IF "%~1" == "HASH" SET TRIE=true

IF "%~1" == "SWIG" (
    SET PATTERN=true
    SET PATWRAPER=true
)

REM Pattern library.
IF "%PATTERN%" == "true" (
    MKDIR src\pattern >nul 2>nul
    MKDIR src\cityhash >nul 2>nul
    ECHO Copying Pattern files.
    FOR %%f IN %PATFILES% DO (
        copy /Y %%f "src\pattern\" >nul 2>nul
    )
    FOR %%f IN %CITYHASH% DO (
        copy /Y %%f "src\cityhash\" >nul 2>nul
    )
    FOR %%f IN %COMMONFILES% DO (
        copy /Y %%f "src\" >nul 2>nul
    )
    IF "%~2" == "SWIG" (
        SET PATWRAPER=true
        GOTO :swigpat
    )
)

:swigpat
REM Build the pattern wrapper.
IF "%PATWRAPER%" == "true" (
	WHERE swig -go -cgo -c++ -intgosize 64 -o src\pattern\51Degrees_go.cxx src\pattern\51Degrees.i >nul 2>nul ( 
        ECHO Building Pattern wrapper. 
    ) || ( 
        ECHO Error: SWIG not found.
    )
)

REM Hash Trie library.
IF "%TRIE%" == "true" (
    MKDIR src\trie >nul 2>nul
    ECHO Copying Trie files.
    FOR %%f IN %TRIEFILES% DO (
        copy /Y %%f "src\trie\" >nul 2>nul
    )
    FOR %%f IN %CACHE% DO (
        copy /Y %%f "src\" >nul 2>nul
    )
    FOR %%f IN %COMMONFILES% DO (
        copy /Y %%f "src\" >nul 2>nul
    )
    IF "%~2" == "SWIG" (
        SET TRIEWRAPER=true
        GOTO :swigtrie
    )
)

:swigtrie
REM Build the Hash Trie wrapper.
IF "%TRIEWRAPER%" == "true" (
    swig -go -cgo -c++ -intgosize 64 -o src\trie\51Degrees_go.cxx src\trie\51Degrees.i >nul 2>nul ( 
        ECHO Building Hash Trie wrapper. 
    ) || ( 
        ECHO Error: SWIG not found 
    )
)
