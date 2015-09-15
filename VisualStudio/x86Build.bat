@echo off

if exist "%ProgramFiles(x86)%\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" goto :msvc86

goto :eof

:msvc86

call "%ProgramFiles(x86)%\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" x86

del Demo\bin\*.* /q
msbuild "Pattern\FiftyOne.Mobile.Detection.Provider.Pattern.vcxproj" /p:Configuration=Release /p:Platform=Win32
msbuild "Trie\FiftyOne.Mobile.Detection.Provider.Trie.vcxproj" /p:Configuration=Release /p:Platform=Win32
msbuild "Interop\FiftyOne.Mobile.Detection.Provider.Interop.csproj" /p:Configuration=Release /p:Platform=Win32

goto :eof

:eof