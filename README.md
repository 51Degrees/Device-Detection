![51Degrees](https://51degrees.com/DesktopModules/FiftyOne/Distributor/Logo.ashx?utm_source=github&utm_medium=repository&utm_content=home&utm_campaign=c-open-source "THE Fastest and Most Accurate Device Detection") **Device Detection in C / C++** includes PHP, Python, Perl, .NET and Node.js

[Recent Changes](#recent-changes "Review recent major changes") | [Supported Databases](https://51degrees.com/compare-data-options?utm_source=github&utm_medium=repository&utm_content=home-menu&utm_campaign=c-open-source "Different device databases which can be used with 51Degrees device detection") | [Developer Documention](https://51degrees.com/support/documentation?utm_source=github&utm_medium=repository&utm_content=home-menu&utm_campaign=c-open-source "Full getting started guide and advanced developer documentation") | [Available Properties](https://51degrees.com/resources/property-dictionary?utm_source=github&utm_medium=repository&utm_content=home-menu&utm_campaign=c-open-source "View all available properties and values")

<sup>Need [.NET](https://github.com/51Degrees/.NET-Device-Detection "THE Fastest and most Accurate device detection for .NET") | [Java](https://github.com/51Degrees/Java-Device-Detection "THE Fastest and most Accurate device detection for Java") | [PHP Script](https://github.com/51Degrees/51Degrees-PHP)?</sup>

[Important 3.2.6 Trie breakages](#recent-changes "Review recent major changes")

Use C code like...

```c
// Declare Resources.
fiftyoneDegreesProvider provider;
fiftyoneDegreesWorkset *ws = NULL;
char *properties = "IsMobile,DeviceType,ScreenInchesDiagonal,PriceBand";
int cacheSize = 1000;
int poolSize = 5;
// Initialise provider.
fiftyoneDegreesDataSetInitStatus status = fiftyoneDegreesInitProviderWithPropertyString(
"[DATA FILE LOCATION]", &provider, properties, poolSize, cacheSize);
// Get workset from the pool.
ws = fiftyoneDegreesProviderWorksetGet(provider);
// Perform detection.
fiftyoneDegreesMatch(ws, "[DEVICE USER AGENT]");
// Return Workset to the pool.
fiftyoneDegreesWorksetRelease(ws);
// Free provider when program complete.
fiftyoneDegreesProviderFree(&provider);
```

... to turn User-Agent HTTP header into useful information about device type, screen diagonal in inches, price band of the device and whether or not device is mobile.

Use C++ code like ...

```cpp
Provider provider = new Provider("[DATA FILE LOCATION]",
	"IsMobile,DeviceType,ScreenInchesDiagonal,PriceBand");
Match match = Provider.getMatch("[YOUR USERAGENT]");
match.Dispose();
provider.Dispose();
```

... to turn User-Agent HTTP headers into useful information about device type, screen diagonal in inches, price band of the device and whether or not device is mobile.

Extensions available for PHP, Python, Perl and Node.

**[Review All Properties](https://51degrees.com/resources/property-dictionary?utm_source=github&utm_medium=repository&utm_content=home-cta&utm_campaign=c-open-source" View all available properties and values")**

## Introduction

Use this project to detect device properties using HTTP browser User-Agent headers as input. It can be used to process server log files, or for real time device detection to support web optimisation.

Two detection methods are supported.

Pattern:  Searches for device signatures in a User-Agent. Unlike previous versions, this does not use regular expression and uses an external data file for easy updates.

Trie: A large binary Trie (pronounced Try) populated with device Patterns. Uses a separate data file. Very fast.

PHP, Python, Perl and C# APIs are built using SWIG, so all use the same C++ Provider and Match classes. This keeps all the APIs consistent and introduces any new features across all APIs simultaneously.

This package includes the following examples:

1. Command line process which takes a user agent via stdin and return a device
  id via stdout. Can easily be modified to return other properties.
  (ProcPat & ProcTrie projects)

2. Command line performance evaluation program which takes a file of user
  agents and returns a performance score measured in detections per second
  per CPU core.
  (PerfPat & PerfTrie projects)

3. Visual Studio solution with example web site, command line projects and C++
  projects to demonstrate how to access from managed and unmanaged code.

4. Getting Started (Pattern and Trie), takes some common User-Agents and returns the value of
  the IsMobile property.

5. Match Metrics, takes some common User-Agents and returns various metrics
  relating to the match carried out on them.

6. Offline Processing, takes an input file with a list of User-Agent,
  processes them and writes results to an output file.

7. Strongly Typed, takes some common User-Agents and returns the value of
  the IsMobile property as a boolean.

8. Match for Device Id, takes some common device ids and returns the value of
  the IsMobile property.

9. Reload from File (Pattern and Trie), illustrates how to reload the data file from the data file on disk without restarting the application. Contains example of both the single threaded and multithreaded reload.

10. Reload from Memory (Pattern and Trie), illustrates how to reload the data file from a continuous memory space that the data file was read into without restarting the application. Contains example of both the single threaded and multithreaded reload.

11. Find Profiles, illustrates how to retrieve profiles based on some user-defined value of one of the properties.

12. PHP, Perl and Python folders containing makefiles for building the respective extensions.

Examples 3-8 are available in C, and in C# using the C++ Provider.
They are also available in Python, Perl and PHP using the same
C++ Provider within their subdirectories.

Use the following instructions to compile different versions for different
target platforms.

**Note: when compiling the examples with GCC or Clang there is a dependancy on dmalloc, and dmallocth for the multithreaded reload functions.**

### Recent Changes
#### Version 3.2.6 Changes
The Trie data file is now initialised into a non static dataset structure, so a few things have changed. 
1. A new provider structure has now been inroduced, giving the Trie API the same reload capabilities as Pattern. It is initialised in the same way, with ``fiftyoneDegreesInitProviderWithPropertyString`` or ``fiftyoneDegreesInitProviderWithPropertyArray`` and freed with ``fiftyoneDegreesProviderFree``.
2. When using the provider, offsets should be created with ``fiftyoneDegreesProviderCreateDeviceOffsets`` and freed with ``fiftyoneDegreesProviderFreeDevideOffsets``.
3. fiftyoneDegreesInitWithPropertyString and fiftyoneDegreesInitWithPropertyArray should now be passed a pointer to a dataset structure which can be allocated with ``malloc(sizeof(fiftyoneDegreesDataSet))``.
4. fiftyoneDegreesDestroy function has been superceded by the fiftyoneDegreesDataSetFree function which takes the dataset to free as an argument.
5. Many funtions now take a fiftyoneDegreesDataSet pointer as an argument as the dataset is no longer contained staticly. See src/trie/51Degrees.h for more details.
6. Any project using the Trie API should either be compiled with ``FIFTYONEDEGREES_NO_THREADING`` defined, or threading enabled by compiling it with src/threading.h.
7. Two new return codes ``DATA_SET_INIT_STATUS_NULL_POINTER`` and ``DATA_SET_INIT_STATUS_POINTER_OUT_OF_BOUNDS`` can be provided when initialising a Trie data set which indicate where a memory allocation problem could have occurred. When used with C++ runtime_error exceptions are thrown for these cases.

Note: this does not affect any of the C based API's as that is taken care of in the wrapper.


### Included Files

makefile - Builds a command line executable under Linux. 
CMakeLists - Builds the example projects under Windows, Linu and OS X.
LICENSE - The licence terms for all source code and Lite data.  

ApacheBench/ - A version of the Apache Benchmarking tool modified to randomly generate User-Agent headers. See ApacheBench/README.md for more details.

CodeBlocks/ - project files for CodeBlocks IDE for 5 generic project types.  

&nbsp;&nbsp;&nbsp;&nbsp;PerfPat - Project files using standard C source.  
&nbsp;&nbsp;&nbsp;&nbsp;PerfTrie - Project files using standard C source.  
&nbsp;&nbsp;&nbsp;&nbsp;ProcPat - Project files using standard C source.  
&nbsp;&nbsp;&nbsp;&nbsp;ProcTrie - Project files using standard C source.  
&nbsp;&nbsp;&nbsp;&nbsp;Console - Project files using standard C source.  
&nbsp;&nbsp;&nbsp;&nbsp;FindProfiles - Project files using standard C source.  
&nbsp;&nbsp;&nbsp;&nbsp;GettingStarted - Project files using standard C source.  
&nbsp;&nbsp;&nbsp;&nbsp;MatchForDeviceId - Project files using standard C source.  
&nbsp;&nbsp;&nbsp;&nbsp;MatchMetrics - Project files using standard C source.  
&nbsp;&nbsp;&nbsp;&nbsp;OfflineProcessing - Project files using standard C source.  
&nbsp;&nbsp;&nbsp;&nbsp;ReloadFromFile - Project files using standard C source.  
&nbsp;&nbsp;&nbsp;&nbsp;ReloadFromMemory - Project files using standard C source.  
&nbsp;&nbsp;&nbsp;&nbsp;StronglyTyped - Project files using standard C source.  

data/ - Open source device data files.  

&nbsp;&nbsp;&nbsp;&nbsp;51Degrees-LiteV3.2.dat - uncompressed data file for use with Pattern.   
&nbsp;&nbsp;&nbsp;&nbsp;51Degrees-LiteV3.2.trie - uncompressed data file for use with Trie.  
&nbsp;&nbsp;&nbsp;&nbsp;20000 User Agents.csv - file with 20,000 popular User-Agents.  
**[Compare Device Databases](https://51degrees.com/compare-data-options?utm_source=github&utm_medium=repository&utm_content=home-cta&utm_campaign=c-open-source "Compare different data file options for 51Degrees device detection")**

examples/ - Contain C source files of example programs. Corresponding project files are available for Visual Studio and Code Blocks.

haproxy/ - References files necessary to build the API with HA Proxy.

node.js/ - Files for Node.js extension. See node.js/README.md for details.

perl/ - Files for Perl extension. See perl/README.md for details.

php/ - Files for PHP extension. See php/README.md for details.

python/ - Files for Python extension. See python/README.md for details.

src/ - all generic C files used across multiple platforms.

&nbsp;&nbsp;&nbsp;&nbsp;threading.h - Macros used for threading including locking, signalling and running multi threaded operations for GCC and MSVC compilers.    
&nbsp;&nbsp;&nbsp;&nbsp;threading.c - provide more complex implementations for GCC compilers.   

src/Pattern - all source files related to Pattern matching detection.

&nbsp;&nbsp;&nbsp;&nbsp;51Degrees.h - The header file for the core detection library.  
&nbsp;&nbsp;&nbsp;&nbsp;51Degrees.c - All the code and necessary data. Quite large.  
&nbsp;&nbsp;&nbsp;&nbsp;ProcPat.c - The command line interface wrapper.  
&nbsp;&nbsp;&nbsp;&nbsp;PerfPat.c - The command line performance test executable.  
&nbsp;&nbsp;&nbsp;&nbsp;Provider.cpp - Uses 51Degrees.c and Match.cpp to serve a Provider object.  
&nbsp;&nbsp;&nbsp;&nbsp;Provider.hpp - The header file for Provider.cpp.  
&nbsp;&nbsp;&nbsp;&nbsp;Match.cpp - Uses 51Degrees.c to serve a match object.  
&nbsp;&nbsp;&nbsp;&nbsp;Match.hpp - The header file for Match.cpp.  
&nbsp;&nbsp;&nbsp;&nbsp;Profiles.cpp - Used by the provider for the find profiles functionality.  
&nbsp;&nbsp;&nbsp;&nbsp;Profiles.hpp - The header file for Profiles.cpp  
&nbsp;&nbsp;&nbsp;&nbsp;51Degrees.i - Used by SWIG to generate Pattern-based extensions.  
&nbsp;&nbsp;&nbsp;&nbsp;The CXX files - Wrappers generated by SWIG.  
&nbsp;&nbsp;&nbsp;&nbsp;ProcPat - Example console project that performs detection based on user input.  
&nbsp;&nbsp;&nbsp;&nbsp;PerfPat - Example console project that measures Pattern performance.  

src/Trie - all source files related to Trie matching detection.

&nbsp;&nbsp;&nbsp;&nbsp;51Degrees.h - The header file for the core detection library.  
&nbsp;&nbsp;&nbsp;&nbsp;51Degrees.c - Source code needed to interrogate the data.  
&nbsp;&nbsp;&nbsp;&nbsp;ProcTrie.c - The command line interface wrapper.  
&nbsp;&nbsp;&nbsp;&nbsp;PerfTrie.c - The command line performance test executable.  
&nbsp;&nbsp;&nbsp;&nbsp;Provider.cpp - Uses 51Degrees.c and Match.cpp to serve a Provider object.  
&nbsp;&nbsp;&nbsp;&nbsp;Provider.hpp - The header file for Provider.cpp.  
&nbsp;&nbsp;&nbsp;&nbsp;Match.cpp - Uses 51Degrees.c to serve a match object.  
&nbsp;&nbsp;&nbsp;&nbsp;Match.cpp - The header file for Match.cpp.  

src/cityhash - a port of Google's cityhash algorithm to C by Alexander Nusov.

&nbsp;&nbsp;&nbsp;&nbsp;city.h - The cityhash header file with CRC methods removed.  
&nbsp;&nbsp;&nbsp;&nbsp;city.c - The cityhash implementation in C with CRC methods removed.  
&nbsp;&nbsp;&nbsp;&nbsp;LICENSE.txt - Licence under which cityhash is distributed.  

src/snprintf - source files to provide safe, OS independent snprintf functions if not supported by the compiler. These resources are no longer used but retained in case integrators need them.

&nbsp;&nbsp;&nbsp;&nbsp;snprintf.h - The snprintf header file.  
&nbsp;&nbsp;&nbsp;&nbsp;snprintf.c - The snprintf implementation.  
&nbsp;&nbsp;&nbsp;&nbsp;LICENSE.txt - Licence under which snprintf is distributed.  

VisualStudio/ - all files related exclusively to Windows and Visual Studio 2013.

&nbsp;&nbsp;&nbsp;&nbsp;Win32build.bat - Builds command line executable in 32 bit Windows.  
&nbsp;&nbsp;&nbsp;&nbsp;Win64build.bat - Builds command line executable in 64 bit Windows.  
&nbsp;&nbsp;&nbsp;&nbsp;WinGCCbuild.bat - Builds command line executable using GCC for Windows.  

&nbsp;&nbsp;&nbsp;&nbsp;Interop/FiftyOne.Mobile.Detection.Provider.Interop.csproj - C# project.  
&nbsp;&nbsp;&nbsp;&nbsp;Interop/PatternWrapper.cs - C# class used to wrap and expose Pattern matching library to .NET callers.  
&nbsp;&nbsp;&nbsp;&nbsp;Interop/TrieWrapper.cs - C# class used to wrap and expose Trie matching library to .NET callers.  
&nbsp;&nbsp;&nbsp;&nbsp;Interop/Utils.cs - Utility class for shared internal methods.  
&nbsp;&nbsp;&nbsp;&nbsp;Interop/Properties/AssemblyInfo.cs - DLL header information.  

&nbsp;&nbsp;&nbsp;&nbsp;Trie/FiftyOne.Mobile.Detection.Provider.Trie.vcxproj - C library project.  
&nbsp;&nbsp;&nbsp;&nbsp;Trie/FuncsTrieDll.c - Dll methods accessible to any Windows library.  

&nbsp;&nbsp;&nbsp;&nbsp;Pattern/FiftyOne.Mobile.Detection.Provider.Pattern.vcxproj - C lib project.  
&nbsp;&nbsp;&nbsp;&nbsp;Pattern/FuncsPatternDll.c - Dll methods accessible to any Windows library.  

&nbsp;&nbsp;&nbsp;&nbsp;Console Interop/Console Interop.csproj - Project file for a console application which demonstrates access to the Interop wrapper.  
&nbsp;&nbsp;&nbsp;&nbsp;Console Interop/Program.cs - Main method with simple code to prove the Interop wrappers are working.  
&nbsp;&nbsp;&nbsp;&nbsp;Console Interop/Properties/AssemblyInfo.cs - Project header information.  

&nbsp;&nbsp;&nbsp;&nbsp;Examples/Getting Started/Getting Started.* - Project files for a getting started example.  
&nbsp;&nbsp;&nbsp;&nbsp;Examples/Getting Started Trie/Getting Started Trie.* - Project files for a getting started example.  
&nbsp;&nbsp;&nbsp;&nbsp;Examples/Match For Device Id/Match For Device Id.* - Project files for a match for device id example.  
&nbsp;&nbsp;&nbsp;&nbsp;Exampes/Match Metrics/Match Metrics.* - Project files for a match metrics example.  
&nbsp;&nbsp;&nbsp;&nbsp;Examples/Offline Processing/Offline Processing.* - Project files for an offline processing example.  
&nbsp;&nbsp;&nbsp;&nbsp;Exampels/Strongly Typed/Strongly Typed.* - Project files for a strongly typed example.  
&nbsp;&nbsp;&nbsp;&nbsp;Examples/Reload From File/Reload From File.* - Project files for a reload example.
&nbsp;&nbsp;&nbsp;&nbsp;Examples/Reload From File/Reload From Memory.* - Project files for a reload example.
&nbsp;&nbsp;&nbsp;&nbsp;Examples/Reload From File/Reload From File Trie.* - Project files for a reload example.
&nbsp;&nbsp;&nbsp;&nbsp;Examples/Reload From File/Reload From Memory Trie.* - Project files for a reload example.


&nbsp;&nbsp;&nbsp;&nbsp;CS Examples/Getting Started/* - Project files for a getting started example.  
&nbsp;&nbsp;&nbsp;&nbsp;CS Examples/Getting Started Trie/* - Project files for a getting started example.  
&nbsp;&nbsp;&nbsp;&nbsp;CS Examples/Match For Device Id/* - Project files for a match for device id example.  
&nbsp;&nbsp;&nbsp;&nbsp;CS Examples/Match Metrics/* - Project files for a match metrics example.  
&nbsp;&nbsp;&nbsp;&nbsp;CS Examples/Offline Processing/* - Project files for an offline processing example.  
&nbsp;&nbsp;&nbsp;&nbsp;CS Examples/Strongly Typed/* - Project files for a strongly typed example.  
&nbsp;&nbsp;&nbsp;&nbsp;CS Examples/Reload Data File/* - Project files for a strongly typed example.  
&nbsp;&nbsp;&nbsp;&nbsp;CS Examples/Examples Tests/* - Project files for tests which test each example.  

&nbsp;&nbsp;&nbsp;&nbsp;Demo/Web Site.csproj - Demo web site using the Interop assembly to request properties associated with the requesting device.  
&nbsp;&nbsp;&nbsp;&nbsp;Demo/Default.aspx.* - Demo web page used to display properties. See these files for examples of how to create providers and reTrieve properties.  
&nbsp;&nbsp;&nbsp;&nbsp;Demo/Properties/AssemblyInfo.cs - Project header information.  

&nbsp;&nbsp;&nbsp;&nbsp;Performance/FiftyOne.Mobile.Detection.Performance.vcxproj - Project file for the multi threaded Windows command line executable.  
&nbsp;&nbsp;&nbsp;&nbsp;Performance/Performance.cpp - Main class for the executable.  
&nbsp;&nbsp;&nbsp;&nbsp;Performance/*.cpp & *.h - Related files to the C++ program.  

&nbsp;&nbsp;&nbsp;&nbsp;PerfPat/PerfPat.vcxproj - Project file using standard C source.  
&nbsp;&nbsp;&nbsp;&nbsp;PerfTrie/PerfTrie.vcxproj - Project file using standard C source.  
&nbsp;&nbsp;&nbsp;&nbsp;ProcPat/ProcPat.vcxproj - Project file using standard C source.  
&nbsp;&nbsp;&nbsp;&nbsp;ProcTrie/ProcTrie.vcxproj - Project file using standard C source.  
&nbsp;&nbsp;&nbsp;&nbsp;Console/Console.vcxproj - Project file using standard C source.  

&nbsp;&nbsp;&nbsp;&nbsp;x64Build.bat - Uses the Visual Studio compiler to build a x64 library.  
&nbsp;&nbsp;&nbsp;&nbsp;x86Build.bat - Uses the Visual Studio compiler to build a x86 library.  
&nbsp;&nbsp;&nbsp;&nbsp;VisualStudio.sln - Visual Studio 2013 solution including demonstration web site, interop console, performance and general C projects.  
