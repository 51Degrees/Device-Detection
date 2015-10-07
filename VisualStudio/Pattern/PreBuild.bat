@for %%X in (swig.exe) do (set SWIG_EXE=%%~$PATH:X)
@if defined SWIG_EXE (
@echo SWIG auto generated code being rebuilt.
swig -c++ -csharp -namespace FiftyOne.Mobile.Detection.Provider.Interop.Pattern -dllimport FiftyOne.Mobile.Detection.Provider.Pattern.dll -outdir ../Interop/Pattern -o ../../src/pattern/51Degrees_csharp.cxx ../../src/pattern/51Degrees.i
) else (
@echo SWIG not found. SWIG auto generated code will not be rebuilt.
)