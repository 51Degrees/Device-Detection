![51Degrees](https://51degrees.com/Portals/0/Logo.png "THE Fasstest and Most Accurate Device Detection")**Device Detection for C** includes PHP, Python, Perl and Node

Looking for [.NET](https://github.com/51Degrees/.NET-Device-Detection "THE Fastest and most Accurate device detection for .NET") | [Java](https://github.com/51Degrees/Java-Device-Detection "THE Fastest and most Accurate device detection for Java") | [PHP Script](https://github.com/51Degrees/51Degrees-PHP)?

[Recent Changes](#recent-changes "Review recent major changes") | [Supported Databases](https://51degrees.com/compare-data-options?utm_source=github&utm_medium=repository&utm_content=home-menu&utm_campaign=c-open-source "Different device databases which can be used with 51Degrees device detection") | [Developer Documention](https://51degrees.com/support/documentation?utm_source=github&utm_medium=repository&utm_content=home-menu&utm_campaign=c-open-source "Full getting started guide and advanced developer documentation") | [Available Properties](https://51degrees.com/resources/property-dictionary?utm_source=github&utm_medium=repository&utm_content=home-menu&utm_campaign=c-open-source "View all available properties and values")

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

Extensions available for PHP, Python, Perl and Node.

**[Review All Properties](https://51degrees.com/resources/property-dictionary?utm_source=github&utm_medium=repository&utm_content=home-cta&utm_campaign=c-open-source" View all available properties and values")**

## Introduction

Use this project to detect device properties using HTTP browser user agents as input. It can be used to process server log files, or for real time device detection to support web optimisation.

Two detection methods are supported.

Pattern:  Searches for device signatures in a useragent. Unlike previous versions, this does not use regular expression and uses an external data file for easy updates.

Trie: A large binary Trie (pronounced Try) populated with device Patterns. Uses a separate data file. Very fast.

This package includes the follow examples:

1. Command line process which takes a user agent via stdin and return a device
  id via stdout. Can easily be modified to return other properties.
  (ProcPat & ProcTrie projects)

2. Command line performance evaluation programme which takes a file of user 
  agents and returns a performance score measured in detections per second
  per CPU core.
  (PerfPat & PerfTrie projects)

3. Visual Studio solution with example web site, command line projects and C++
  projects to demonstrate how to access from managed and unmanaged code.
  
4. PHP extension.

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
  51Degrees-Lite.dat - uncompressed data file for use with Pattern.
  51Degrees-Lite.Trie - uncompressed data file for use with Trie.
  20000 User Agents.csv - file with 20,000 popular useragents.

**[Compare Device Databases](https://51degrees.com/compare-data-options?utm_source=github&utm_medium=repository&utm_content=home-cta&utm_campaign=c-open-source "Compare different data file options for 51Degrees device detection")**

php/ - Files for PHP extension. See php/README.md for details.
 
src/ - all generic C files used across multiple platforms.

  threading.h - Macros used for threading including locking, signalling and running multi threaded operations for GCC and MSVC compilers.  

src/Pattern - all source files related to Pattern matching detection.

  51Degrees.h - The header file for the core detection library.
  51Degrees.c - All the code and necessary data. Quite large.
  ProcPat.c - The command line interface wrapper.
  PerfPat.c - The command line performance test executable.

src/Trie - all source files related to Trie matching detection.

  51Degrees.h - The header file for the core detection library.
  51Degrees.c - Source code needed to interrogate the data.
  ProcTrie.c - The command line interface wrapper.
  PerfTrie.c - The command line performance test executable.

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

## Recent Changes

#### Version 3.1.6.X

1. Pattern now supports multi threading for both GCC and MSVC compilers. Multi threading can be turned off with the use of **FIFTYONEDEGREES___NO___THREADING** 
   directive.
2. Pattern includes options for a cache of previous detections.
3. Pattern supports a workset pool for use in a multi threaded environment.
4. All external method names have been standardised in the form fiftyoneDegrees[TYPE][ACTION]
5. snprintf is no longer required.
6. New methods have been added to return the length of a CSV and JSON buffer and
   also allocate memory for these processing operations.

Version 3.1.5.2

1. Names of external functions and structs have been changed to have the prefix 'fiftyoneDegrees'. This is to prevent name clashes when integrating to existing code. The following Pattern detector names have been changed:

  Structs and enums:
  fod_bool -> fiftyoneDegreesBool
  matchMethod -> fiftyoneDegreesMatchMethod;
  DataSetInitStatus -> fiftyoneDegreesDataSetInitStatus
  RANGE -> fiftyoneDegreesRANGE
  AsciiString -> fiftyoneDegreesAsciiString
  Component -> fiftyoneDegreesComponent
  Map -> fiftyoneDegreesMap
  NodeNumericIndex -> fiftyoneDegreesNodeNumericIndex
  NodeIndexValue -> fiftyoneDegreesNodeIndexValue
  NodeIndex -> fiftyoneDegreesNodeIndex
  String -> fiftyoneDegreesString
  Strings -> fiftyoneDegreesStrings
  Node -> fiftyoneDegreesNode
  ProfileOffset -> fiftyoneDegreesProfileOffset
  Property -> fiftyoneDegreesProperty
  Value -> fiftyoneDegreesValue
  Profile -> fiftyoneDegreesProfile
  Date -> fiftyoneDegreesDate
  EntityHeader -> fiftyoneDegreesEntityHeader
  NumericNodeState -> fiftyoneDegreesNumericNodeState
  DataSetHeader -> fiftyoneDegreesDataSetHeader;
  DataSet -> fiftyoneDegreesDataSet;
  LinkedSignatureListItem -> fiftyoneDegreesLinkedSignatureListItem;
  LinkedSignatureList -> fiftyoneDegreesLinkedSignatureList;
  WorkSet -> fiftyoneDegreesWorkset;

  Functions:
  initWithPropertyArray -> fiftyoneDegreesInitWithPropertyArray
  initWithPropertyString -> fiftyoneDegreesInitWithPropertyString
  destroy -> fiftyoneDegreesDestroy
  createWorkset -> fiftyoneDegreesCreateWorkset
  freeWorkset -> fiftyoneDegreesFreeWorkset
  match -> fiftyoneDegreesMatch
  setValues -> fiftyoneDegreesSetValues
  getString -> fiftyoneDegreesGetString
  getValueName -> fiftyoneDegreesGetValueName
  getPropertyName -> fiftyoneDegreesGetPropertyName
  processDeviceCSV -> fiftyoneDegreesProcessDeviceCSV
  processDeviceJSON -> fiftyoneDegreesProcessDeviceJSON             

  The following Trie detector names have been changes:

  Structs:
  DataSetInitStatus -> fiftyoneDegreesDataSetInitStatus;

  Functions:
  init -> fiftyoneDegreesInit
  getDeviceOffset -> fiftyoneDegreesGetDeviceOffset
  getPropertyIndex -> fiftyoneDegreesGetPropertyIndex
  getValue -> fiftyoneDegreesGetValue
  destroy -> fiftyoneDegreesDestroy
  processDeviceCSV -> fiftyoneDegreesProcessDeviceCSV
  processDeviceJSON -> fiftyoneDegreesProcessDeviceJSON

2. Added two new functions to Trie:

  int fiftyoneDegreesGetRequiredPropertiesCount() - Returns how many properties have been loaded in the dataset.
  char** fiftyoneDegreesGetRequiredPropertiesNames() - Returns the names of the properties loaded in the dataset.

### Trie Data - Download

Lite Trie data is included in the data folder. Lite data updates are on the 51Degrees Sourceforge page (https://sourceforge.net/projects/fiftyone-c/files/).

For information Premium and Enterprise Trie data and their benefits, see our website.

**[Compare Device Databases](https://51degrees.com/compare-data-options?utm_source=github&utm_medium=repository&utm_content=home-cta&utm_campaign=c-open-source "Compare different data file options for 51Degrees device detection")**

The Trie data file is not required if only the Pattern method is used.

### Trie Data - File Size

The Trie data file is very large as it contains 10s of millions of index strings. If the data file is larger than 2,147,483,647 bytes it cannot be loaded into memory on a 32 bit architecture. This is not a problem for the Lite format data file. However if using the Premium or Enterprise data file this limit may be exceeded. Where possible, only use the 64 bit versions of the following examples.

### Windows .NET - Wrapper & Demo Web Site

All Windows specific files are located in the Windows folder. They rely on Visual Studio 2010 or greater to build.

Execute the x64Build.bat or x86Build.bat files to compiler a 32 or 64 bit version of the DLLs. 

3 DLLs are compiled and placed in the following directories.

1. Windows/Demo/bin/FiftyOne.Mobile.Detection.Provider.Pattern.dll
2. Windows/Demo/bin/FiftyOne.Mobile.Detection.Provider.Trie.dll
3. Windows/Demo/bin/FiftyOne.Mobile.Detection.Provider.Interop.dll

1. The "Pattern" DLL is a self-contained detection provider using the Pattern matching method. The FuncsPatternDll.c file contains the external interfaces.

2. The "Trie" DLL is a detection provider using the Trie matching method. A data file located in the data folder needs to be used with this DLL. The FuncsTrieDll.c file contain the external interfaces.

3. The "Interop" DLL is a Common Language Runtime inter operation interface to the previous two DLLs. It should be used with .NET code.

Alternatively the FiftyOne.Mobile.Detection.Provider.sln can be opened in Visual Studio 2013 or greater. This also includes an example web site with a single page which displays the values returned for the requesting device from both the Pattern and Trie providers.

Important: Before starting the example web site check the Default.aspx.cs file to ensure the path to the Trie data file is set correctly. Alternatively comment out the Trie provider.

### Windows Command Line

Execute the Win32Build.bat or Win64Build.bat files in the root folder to build a command line executable using the Visual Studio compiler. This will compile ProcPat.exe and PerfPat.exe.

Execute the WinGCCBuild.bat file to compile the same executable using the GCC compiler through CygWin or MinGW if installed. The relevant PATH file should be configured before use.

This will compile ProcPat.exe, PerfPat.exe, ProcTrie.exe and PerfTrie.exe. The Trie text programs rely on features only available in the GCC compiler.

4 or 5 executable are placed in the root folder, depending on compiler.

See Instructions further down this document for more details.

Basic test: [Console.exe](#console)
Stdin and Stdout process: [ProcPat.exe](#procpat) | [ProcTrie.exe](#proctrie)
Performance test executables: [PerfPat.exe](#perfpat) | [PerfTrie.exe](#perftrie)

### Windows Multi Threaded Performance Measurement

A slightly modified version of PerfTrie.exe is created from the FiftyOne.Mobile.Detection.Performance.sln Visual Studio project. A command line executable is built which takes a Trie data file and file of user agents as input. 4 threads are started to read and process the user agents input file in parallel. Once complete it returns the average number of detections performed per second.

Performance.exe TrieDataFile UserAgentsFile

**[Data file path]**  A path to a Trie data file

**[UserAgentsFile]** A path to a list of user agents to be used in the performance test.

https://51degrees.com/million.zip contains a test file of one million user agents.

Example:  

```
Performance.exe 51Degrees-Lite.Trie million.csv
```

### Linux and Mac OSX Command Line

The makefile provided in the root folder should be used to build the following 4 executables. See Instructions further down this document for more details concerning how to use these executables.

Console   Detection test.

ProcPat   Stdin and Stdout process.
ProcTrie

PerfPat   Performance test executable.
PerfTrie

Use the terminal to navigate to the root folder of the project and enter the command "make" to build the executables.

### Instructions

#### Console

Prints out properties and detection information from some precompiled useragent strings.

```
Console.exe [Data file path]
```

**[Data file path]**  A path to a Pattern data file

Example:  

```
Console.exe data/51Degrees-Lite.dat
```

#### ProcPat

Starts a process which reads user agents from stdIn and returns CSV format properties and values for the matching device via stdOut. Uses the Pattern matching detection routine.

When running type a user agent, or any string for testing, followed by enter to view the matched device properties. Provide an empty string to terminate the process.

```
ProcPat [Data file path] [Properties]
```

**[Data file path]** A path to a Pattern data file

**[Properties]** A comma separated list of properties to be returned. If not provided Id will be used.

For example: Id,IsMobile will return the Id of the matched device and True or False to indicate if the device is a mobile.

See httpa://51Degrees.com/Products/DeviceData/PropertyDictionary for a list of valid properties.

Example:  

```
ProcPat data/51Degrees-Lite.dat Id,IsMobile
```

Enter user agent strings to view the returned Id and IsMobile values, or an empty string to quit.

#### PerfPat

Runs a performance test using a single process and thread using a data file of user agent strings as input. Uses the Pattern matching detection routine which is very memory efficient.

```
PerfPat [Data file path] [Useragents file] [Properties]
```

**[Data file path]** A path to a Pattern data file

**[Useragents file]** A path to a list of user agents to be used in the performance test.

http://51Degrees/Million.zip contains a test file of one million user agents for testing purposes.

**[Properties]** A comma separated list of properties to be returned. If not provided Id will be used.

For example: Id,IsMobile will return the Id of the matched device and True or False to indicate if the device is a mobile.

See http://51Degrees/Products/DeviceData/PropertyDictionary for a list of valid properties.

Example: PerfPat data/51Degrees-Lite.dat million.csv Id,IsMobile

This program requires no input. It will test with the given data and output detection times at the end. This will take several minutes.

#### ProcTrie

Starts a process which reads user agents from stdIn and returns CSV format properties and values for the matching device via stdOut. Uses the Pattern matching detection routine.

When running type a user agent, or any string for testing, followed by enter to view the matched device properties. Provide an empty string to terminate the process.

```
ProcTrie DataFile [Properties]
```

**[TrieDataFile]** A source data file in Trie format. A data file current at the time the zip file was created is included in data folder.

**[Properties]** A comma separated list of properties to be returned. If not provided Id will be used.

For example: Id,IsMobile will return the Id of the matched device and True or False to indicate if the device is a mobile.

See http://51Degrees/Products/DeviceData/PropertyDictionary for a list of valid properties.

Example:  

```
ProcTrie data/51Degrees-Lite.Trie IsMobile,Id
```

Enter user agent strings to view the returned Id and IsMobile values, or an empty string to quit. 

#### PerfTrie

Runs a performance test using multiple threads and a data file of useragent strings as input. Uses the Trie matching detection routine which is very fast, but uses a lot of memory.

This program is only built when using the GCC compiler.

```
PerfTrie [Trie data file] [Useragents file] [Properties]
```

**[Trie data file]** A source data file in Trie format. A data file current at the time the zip file was created is included in data folder.

**[Useragents file]** A path to a list of user agents to be used in the performance test.

http://51Degrees/Million.zip contains a test file of one million useragents.

**[Properties]** A comma separated list of properties to be returned. If not provided Id will be used.

For example: Id,IsMobile will return the Id of the matched device and True or False to indicate if the device is a mobile.

See http://51Degrees/Products/DeviceData/PropertyDictionary for a list of valid properties.

Example: 

```
PerfTrie data/51Degrees-Lite.Trie million.csv Id,IsMobile
```

This program requires no input. It will test with the given data and output detection times at the end. This will take several minutes.