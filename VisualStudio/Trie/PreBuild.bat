for %%X in (swig.exe) do (set SWIG_EXE=%%~$PATH:X)
if defined SWIG_EXE (
echo SWIG auto generated code being rebuilt.
swig -c++ -csharp -namespace FiftyOne.Mobile.Detection.Provider.Interop.Trie -dllimport FiftyOne.Mobile.Detection.Provider.Trie.dll -outdir ../Interop/Trie -o ../../src/trie/51Degrees_csharp.cxx ../../src/trie/51Degrees.i
) else (
echo SWIG not found. SWIG auto generated code will not be rebuilt.
)