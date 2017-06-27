@echo off

if exist "%ProgramFiles(x86)%\Microsoft Visual Studio 10.0\VC\vcvarsall.bat" goto :msvc64

goto :eof

:msvc64

call "%ProgramFiles(x86)%\Microsoft Visual Studio 10.0\VC\vcvarsall.bat" amd64

del Demo\bin\*.* /q
msbuild "Pattern\FiftyOne.Mobile.Detection.Provider.Pattern.vcxproj" /p:Configuration=Release /p:Platform=x64
msbuild "Trie\FiftyOne.Mobile.Detection.Provider.Trie.vcxproj" /p:Configuration=Release /p:Platform=x64
msbuild "Interop\FiftyOne.Mobile.Detection.Provider.Interop.csproj" /p:Configuration=Release /p:Platform=x64

goto :eof

:eof