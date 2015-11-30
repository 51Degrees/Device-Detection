![51Degrees](https://51degrees.com/DesktopModules/FiftyOne/Distributor/Logo.ashx?utm_source=github&utm_medium=repository&utm_content=home&utm_campaign=c-open-source "THE Fastest and Most Accurate Device Detection") **Device Detection in C / C++** includes PHP, Python, Perl, .NET and Node.js

[Recent Changes](#recent-changes "Review recent major changes") | [Supported Databases](https://51degrees.com/compare-data-options?utm_source=github&utm_medium=repository&utm_content=home-menu&utm_campaign=c-open-source "Different device databases which can be used with 51Degrees device detection") | [Developer Documention](https://51degrees.com/support/documentation?utm_source=github&utm_medium=repository&utm_content=home-menu&utm_campaign=c-open-source "Full getting started guide and advanced developer documentation") | [Available Properties](https://51degrees.com/resources/property-dictionary?utm_source=github&utm_medium=repository&utm_content=home-menu&utm_campaign=c-open-source "View all available properties and values")

<sup>Need [.NET](https://github.com/51Degrees/.NET-Device-Detection "THE Fastest and most Accurate device detection for .NET") | [Java](https://github.com/51Degrees/Java-Device-Detection "THE Fastest and most Accurate device detection for Java") | [PHP Script](https://github.com/51Degrees/51Degrees-PHP)?</sup>

Use C code like...

```c
fiftyoneDegreesDataSet dataSet;
fiftyoneDegreesWorkset *workSet;
fiftyoneDegreesInitWithPropertyString("[DATA FILE LOCATION]" , &dataSet,
    "IsMobile,DeviceType,ScreenInchesDiagonal,PriceBand");
workSet = fiftyoneDegreesCreateWorkset(dataSet);
fiftyoneDegreesMatch(workset, "[YOUR USERAGENT]");
```

... to turn User-Agent HTTP headers into useful information about physical screen size, device price and type of device.

Use C++ code like ...

```cpp
Provider provider = new Provider("[DATA FILE LOCATION]",
	"IsMobile,DeviceType,ScreenInchesDiagonal,PriceBand");
Match match = Provider.getMatch("[YOUR USERAGENT]");
match.Dispose();
provider.Dispose();
```

... to turn User-Agent HTTP headers into useful information about physical screen size, device price and type of device.

Extensions available for PHP, Python, Perl and Node.

**[Review All Properties](https://51degrees.com/resources/property-dictionary?utm_source=github&utm_medium=repository&utm_content=home-cta&utm_campaign=c-open-source" View all available properties and values")**

## Introduction

Use this project to detect device properties using HTTP browser user agents as input. It can be used to process server log files, or for real time device detection to support web optimisation.

Two detection methods are supported.

Pattern:  Searches for device signatures in a User-Agent. Unlike previous versions, this does not use regular expression and uses an external data file for easy updates.

Trie: A large binary Trie (pronounced Try) populated with device Patterns. Uses a separate data file. Very fast.

This package includes the following examples:

1. Command line process which takes a user agent via stdin and return a device
  id via stdout. Can easily be modified to return other properties.
  (ProcPat & ProcTrie projects)

2. Command line performance evaluation programme which takes a file of user
  agents and returns a performance score measured in detections per second
  per CPU core.
  (PerfPat & PerfTrie projects)

3. Visual Studio solution with example web site, command line projects and C++
  projects to demonstrate how to access from managed and unmanaged code.

4. Getting started, takes some common User-Agents and returns the value of
  the IsMobile property.

5. Match Metrics, takes some common User-Agents and returns various metrics
  relating to the match carried out on them.

6. Offline Processing, takes an input file with a list of User-Agent,
  processes them and writes results to an output file.

7. Strongly Typed, takes some common User-Agents and returns the value of
  the IsMobile property as a boolean.

8. Match for Device Id, takes some common device ids and returns the value of
  the IsMobile property.

9. PHP extension.

Examples 3-8 are available in C, and in C# using the C++ Provider.
They are also available in Python, Perl and PHP within their subdirectories.

Use the following instructions to compile different versions for different
target platforms.

### Included Files

makefile - Builds a command line executable under Linux.
LICENSE - The licence terms for all source code and Lite data.

CodeBlocks - project files for CodeBlocks IDE for 5 generic project types.

  PerfPat.* - Project files using standard C source.
  PerfTrie.* - Project files using standard C source.
  ProcPat.* - Project files using standard C source.
  ProcTrie.* - Project files using standard C source.
  Console.* - Project files using standard C source.

data - Open source device data files.
  51Degrees-LiteV3.2.dat - uncompressed data file for use with Pattern.
  51Degrees-LiteV3.2.trie - uncompressed data file for use with Trie.
  20000 User Agents.csv - file with 20,000 popular User-Agents.

**[Compare Device Databases](https://51degrees.com/compare-data-options?utm_source=github&utm_medium=repository&utm_content=home-cta&utm_campaign=c-open-source "Compare different data file options for 51Degrees device detection")**

php/ - Files for PHP extension. See php/README.md for details.

src/ - all generic C files used across multiple platforms.

  threading.h - Macros used for threading including locking, signalling and running multi threaded operations for GCC and MSVC compilers.  

src/Pattern - all source files related to Pattern matching detection.

  51Degrees.h - The header file for the core detection library.
  51Degrees.c - All the code and necessary data. Quite large.
  ProcPat.c - The command line interface wrapper.
  PerfPat.c - The command line performance test executable.
  Provider.cpp - Uses 51Degrees.c and Match.cpp to serve a Provider object.
  Provider.hpp - The header file for Provider.cpp.
  Match.cpp - Uses 51Degrees.c to serve a match object.
  Match.cpp - The header file for Match.cpp.

src/Trie - all source files related to Trie matching detection.

  51Degrees.h - The header file for the core detection library.
  51Degrees.c - Source code needed to interrogate the data.
  ProcTrie.c - The command line interface wrapper.
  PerfTrie.c - The command line performance test executable.
  Provider.cpp - Uses 51Degrees.c and Match.cpp to serve a Provider object.
  Provider.hpp - The header file for Provider.cpp.
  Match.cpp - Uses 51Degrees.c to serve a match object.
  Match.cpp - The header file for Match.cpp.

src/cityhash - a port of Google's cityhash algorithm to C by Alexander Nusov.

  city.h - The cityhash header file with CRC methods removed.
  city.c - The cityhash implementation in C with CRC methods removed.
  LICENSE.txt - Licence under which cityhash is distributed.

src/snprintf - source files to provide safe, OS independent snprintf functions if not supported by the compiler. These resources are no longer used but retained in case integrators need them.

  snprintf.h - The snprintf header file.
  snprintf.c - The snprintf implementation.
  LICENSE.txt - Licence under which snprintf is distributed.

VisualStudio/ - all files related exclusively to Windows and Visual Studio 2013.

  Win32build.bat - Builds command line executable in 32 bit Windows.
  Win64build.bat - Builds command line executable in 64 bit Windows.
  WinGCCbuild.bat - Builds command line executable using GCC for Windows.

  Interop/FiftyOne.Mobile.Detection.Provider.Interop.csproj - C# project.
  Interop/PatternWrapper.cs - C# class used to wrap and expose Pattern matching library to .NET callers.
  Interop/TrieWrapper.cs - C# class used to wrap and expose Trie matching library to .NET callers.
  Interop/Utils.cs - Utility class for shared internal methods.
  Interop/Properties/AssemblyInfo.cs - DLL header information.

  Trie/FiftyOne.Mobile.Detection.Provider.Trie.vcxproj - C library project.
  Trie/FuncsTrieDll.c - Dll methods accessible to any Windows library.

  Pattern/FiftyOne.Mobile.Detection.Provider.Pattern.vcxproj - C lib project.
  Pattern/FuncsPatternDll.c - Dll methods accessible to any Windows library.

  Console Interop/Console Interop.csproj - Project file for a console application which demonstrates access to the Interop wrapper.
  Console Interop/Program.cs - Main method with simple code to prove the Interop wrappers are working.
  Console Interop/Properties/AssemblyInfo.cs - Project header information.

  Examples/Getting Started/Getting Started.* - Project files for a getting started example.
  Examples/Match For Device Id/Match For Device Id.* - Project files for a match for device id example.
  Exampes/Match Metrics/Match Metrics.* - Project files for a match metrics example.
  Examples/Offline Processing/Offline Processing.* - Project files for an offline processing example.
  Exampels/Strongly Typed/Strongly Typed.* - Project files for a strongly typed example.

  CS Examples/Getting Started/* - Project files for a getting started example/
  CS Examples/Match For Device Id/* - Project files for a match for device id example.
  CS Examples/Match Metrics/* - Project files for a match metrics example.
  CS Examples/Offline Processing/* - Project files for an offline processing example.
  CS Examples/Strongly Typed/* - Project files for a strongly typed example.
  CS Examples/Examples Tests/* - Project files for tests which test each example.

  Demo/Web Site.csproj - Demo web site using the Interop assembly to request properties associated with the requesting device.
  Demo/Default.aspx.* - Demo web page used to display properties. See these files for examples of how to create providers and reTrieve properties.
  Demo/Properties/AssemblyInfo.cs - Project header information.

  Performance/FiftyOne.Mobile.Detection.Performance.vcxproj - Project file for the multi threaded Windows command line executable.
  Performance/Performance.cpp - Main class for the executable.
  Performance/*.cpp & *.h - Related files to the C++ programme.

  PerfPat/PerfPat.vcxproj - Project file using standard C source.
  PerfTrie/PerfTrie.vcxproj - Project file using standard C source.
  ProcPat/ProcPat.vcxproj - Project file using standard C source.
  ProcTrie/ProcTrie.vcxproj - Project file using standard C source.
  Console/Console.vcxproj - Project file using standard C source.

  x64Build.bat - Uses the Visual Studio compiler to build a x64 library.
  x86Build.bat - Uses the Visual Studio compiler to build a x86 library.
  VisualStudio.sln - Visual Studio 2013 solution including demonstration web site, interop console, performance and general C projects.