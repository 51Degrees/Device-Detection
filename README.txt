
﻿Introduction
------------

Use this project to detect device properties using HTTP browser user agents as
input. It can be used to process server log files, or for real time device
detection to support web optimisation.

Two detection methods are supported.

Pattern:  Searches for device signatures in a useragent. Unlike previous
  versions, this does not use regular expression and uses an external data file
  for easy updates.

Trie: A large binary Trie (pronounced Try) populated with device Patterns. Uses
  a separate data file. Very fast.

This package includes the follow examples:

1.  Command line process which takes a user agent via stdin and return a device
  id via stdout. Can easily be modified to return other properties.

2.  Command line performance evaluation programme which takes a file of user 
  agents and returns a performance score measured in detections per second
  per CPU core.

3.  A GCC only multi-threaded Trie matching method performance evaluation
  program.

4.  A Windows example web site which uses the 2 detection methods and compares
  the results side by side for the requesting browser.

Use the following instructions to compile different versions for different 
target platforms.

Included Files
--------------

Win32build.bat - Builds command line executable in 32 bit Windows.
Win64build.bat - Builds command line executable in 64 bit Windows.
WinGCCbuild.bat - Builds command line executable using GCC for Windows.
makefile - Builds a command line executable under Linux.
Licence.txt - The licence terms for the files in this zip file.

data - A folder for external data files.
  51Degrees-Lite.dat - uncompressed data file for use with Pattern.
  51Degrees-Lite.Trie - uncompressed data file for use with Trie.

src/ - all generic C files used across multiple platforms.

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

src/snprintf - source files to provide safe, OS independent snprintf functions.

  snprintf.h - The snprintf header file.
  snprintf.c - The snprintf implementation.

windows/ - all files related exclusively to Windows.

  Interop/FiftyOne.Mobile.Detection.Provider.Interop.csproj - C# project.
  Interop/PatternWrapper.cs - C# class used to wrap and expose Pattern
    matching library to .NET callers.
  Interop/TrieWrapper.cs - C# class used to wrap and expose Trie matching
    library to .NET callers.
  Interop/Utils.cs - Utility class for shared internal methods.
  Interop/Properties/AssemblyInfo.cs - DLL header information.

  Trie/FiftyOne.Mobile.Detection.Provider.Trie.vcxproj - C library project.
  Trie/FuncsTrieDll.c - Dll methods accessible to any Windows library.

  Pattern/FiftyOne.Mobile.Detection.Provider.Pattern.vcxproj - C lib project.
  Pattern/FuncsPatternDll.c - Dll methods accessible to any Windows library.
  Pattern/PreBuild.bat - Used to create platform specific char tables for
    regexes.

  Demo/FiftyOne.Mobile.Detection.Demo.csproj - Demo web site using the 
    Interop assembly to request properties associated with the requesting
    device.
  Demo/Default.aspx.* - Demo web page used to display properties. See these 
    files for examples of how to create providers and reTrieve properties.
  Demo/Properties/AssemblyInfo.cs - Project header information.

  Performance/FiftyOne.Mobile.Detection.Performance.vcxproj - Project file
    for the multi threaded Windows command line executable.
  Performance/Performance.cpp - Main class for the executable.
  Performance/*.cpp & *.h - Related files to the C++ programme.

  x64Build.bat - Uses the Visual Studio compiler to build a x64 library.
  x86Build.bat - Uses the Visual Studio compiler to build a x86 library.
  FiftyOne.Mobile.Detection.Provider.sln - Visual Studio 2010 solution 
    including demonstration web site and single threaded command line
    executables.
  FiftyOne.Mobile.Detection.Performance.sln - Visual Studio 2010 solution 
    for the multi threaded performance test application.

Changes
-------

Version 3.1.5.2

1. Names of external functions and structs have been changed to have the
   prefix 'fiftyoneDegrees'. This is to prevent name clashes when
   integrating to existing code.
   The following Pattern detector names have been changed:

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
   int fiftyoneDegreesGetRequiredPropertiesCount()
   Returns how many properties have been loaded in the dataset.

   char** fiftyoneDegreesGetRequiredPropertiesNames()
   Returns the names of the properties loaded in the dataset.

Version 3.1.4.2
---------------

1. Changed the PerfTrie and PerfPat class to use wall time rather than CPU
   ticks to avoid problems associated with multi threading on some platforms.
   Number of passes increased to offset impact of lower resolution timer.
2. Updated the makefile to use O3 optimisations and to work on Windows.

Version 3.1.3.2
---------------

1. The PerfTrie.c file has been changed to use the new init function enum.

2. The JSON method has been changed to return the correct string lenth when no
   properties are provided.

Version 3.1.2.2
---------------

1. The init functions now return an enum instead of a int which can be used to
   to see if the initialisation was successful or to the exact error if a
   failure did occur. These allowed values are:
	 SUCCESS - Initialisation finished with no issues.
	 INSUFFICIENT_MEMORY - The system does not have enough available memory
	 to create the dataset.
	 CORRUPT_DATA - The data file had unexpected data. Make sure it is uncompressed
	 and that you're using the correct file. Try downloading the file again if
	 you get persistent problems.
	 INCORRECT_VERSION - The file is valid, but cannot be read by your api. Make
	 sure you have the latest file and api.
	 FILE_NOT_FOUND - The given file could not be found. Check it exists and
	 that the process has sufficient permission.
2.	Added ProcessJSON function. This function returns device data as a JSON string.
	This function is used heavily in the Perl and Python APIs.

Version 3.0.X to 3.1.1.3
------------------

1. Pattern data file changed to include pre-sorted lists for Nearest and Closest
   matching improving performance. Version 3.0 data files will not work with this 
   verison.
2. Various changes to wrapper programmes to improve diagnostics information and 
   better show how the matching rountines can be used.
3. Fixed segment fault defect with Lite data file and files with fewer root nodes
   than user agent characters.

Trie Data - Download
--------------------

Lite Trie data is included in the data folder. Lite data updates are on the
51Degrees Sourceforge page (https://sourceforge.net/projects/fiftyone-c/files/).

For information Premium and Enterprise Trie data and their benefits, see our
website: https://51degrees.com/Products/DeviceDetection

The Trie data file is not required if only the Pattern method is used.

Trie Data - File Size
-----------------------

The Trie data file is very large as it contains 10s of millions of index
strings. If the data file is larger than 2,147,483,647 bytes it cannot be loaded
into memory on a 32 bit architecture. This is not a problem for the Lite format
data file. However if using the Premium or Enterprise data file this limit may
be exceeded. Where possible, only use the 64 bit versions of the following
examples.

Windows .NET - Wrapper & Demo Web Site
--------------------------------------

All Windows specific files are located in the Windows folder. They rely on 
Visual Studio 2010 or greater to build.

Execute the x64Build.bat or x86Build.bat files to compiler a 32 or 64 bit 
version of the DLLs. 

3 DLLs are compiled and placed in the following directories.

1. Windows/Demo/bin/FiftyOne.Mobile.Detection.Provider.Pattern.dll
2. Windows/Demo/bin/FiftyOne.Mobile.Detection.Provider.Trie.dll
3. Windows/Demo/bin/FiftyOne.Mobile.Detection.Provider.Interop.dll

1.	The "Pattern" DLL is a self-contained detection provider using the Pattern
  matching method. The FuncsPatternDll.c file contains the external
  interfaces.

2.	The "Trie" DLL is a detection provider using the Trie matching method. A 
  data file located in the data folder needs to be used with this DLL. The
  FuncsTrieDll.c file contain the external interfaces.

3.	The "Interop" DLL is a Common Language Runtime inter operation interface to
  the previous two DLLs. It should be used with .NET code.

Alternatively the FiftyOne.Mobile.Detection.Provider.sln can be opened in 
Visual Studio 2010 or greater. This also includes an example web site with a
single page which displays the values returned for the requesting device from
both the Pattern and Trie providers.

Important: Before starting the example web site check the Default.aspx.cs file 
to ensure the path to the Trie data file is set correctly. Alternatively 
comment out the Trie provider.

Windows Command Line
--------------------

Execute the Win32Build.bat or Win64Build.bat files in the root folder to build
a command line executable using the Visual Studio compiler.
This will compile ProcPat.exe and PerfPat.exe.

Execute the WinGCCBuild.bat file to compile the same executable using the GCC 
compiler through CygWin or MinGW if installed. The relevant PATH file should be
configured before use.
This will compile ProcPat.exe, PerfPat.exe, ProcTrie.exe and PerfTrie.exe. The
Trie text programs rely on features only available in the GCC compiler.

4 or 5 executable are placed in the root folder, depending on compiler.
See Instructions further down this document for more details.

Console.exe   Detection test.
ProcPat.exe   Stdin and Stdout process.
ProcTrie.exe

PerfPat.exe   Performance test executable.
PerfTrie.exe  (GCC only)

Windows Multi Threaded Performance Measurement
----------------------------------------------

A slightly modified version of PerfTrie.exe is created from the
FiftyOne.Mobile.Detection.Performance.sln Visual Studio project. A command line
executable is built which takes a Trie data file and file of user agents as 
input. 4 threads are started to read and process the user agents input file in
parallel. Once complete it returns the average number of detections performed
per second.

Performance.exe TrieDataFile UserAgentsFile

[Data file path]  A path to a Trie data file

[UserAgentsFile] A path to a list of user agents to be used in the performance
  test.

  http://51Degrees.com/Million.zip contains a test file of one million user agents.

Example:  Performance.exe 51Degrees-Lite.Trie million.csv


Linux and Mac OSX Command Line
------------------------------

The makefile provided in the root folder should be used to build the following
4 executables. See Instructions further down this document for more details
concerning how to use these executables.

Console   Detection test.

ProcPat   Stdin and Stdout process.
ProcTrie

PerfPat   Performance test executable.
PerfTrie

Use the terminal to navigate to the root folder of the project and enter the
command "make" to build the executables.



Instructions
------------

Console
-------

Prints out properties and detection information from some precompiled
useragent strings.

Console.exe [Data file path]

[Data file path]  A path to a Pattern data file

Example:  Console.exe data/51Degrees-Lite.dat

ProcPat
-------

Starts a process which reads user agents from stdIn and returns CSV format
properties and values for the matching device via stdOut. Uses the Pattern 
matching detection routine.

When running type a user agent, or any string for testing, followed by enter to
view the matched device properties. Provide an empty string to terminate the
process.

ProcPat [Data file path] [Properties]

[Data file path]  A path to a Pattern data file

[Properties]  A comma separated list of properties to be returned. If not 
  provided Id will be used.

  For example: Id,IsMobile will return the Id of the matched device and True or
  False to indicate if the device is a mobile.

  See http://51Degrees/Products/DeviceData/PropertyDictionary for a list of
  valid properties.

Example:  ProcPat data/51Degrees-Lite.dat Id,IsMobile

  Enter user agent strings to view the returned Id and IsMobile values, or an
  empty string to quit.

PerfPat
-------

Runs a performance test using a single process and thread using a data file
of user agent strings as input. Uses the Pattern matching detection routine
which is very memory efficient.

PerfPat [Data file path] [Useragents file] [Properties]

[Data file path]  A path to a Pattern data file

[Useragents file] A path to a list of user agents to be used in the performance
  test.

  http://51Degrees/Million.zip contains a test file of one million user agents
  for testing purposes.

[Properties]  A comma separated list of properties to be returned. If not 
  provided Id will be used.

  For example: Id,IsMobile will return the Id of the matched device and True or
  False to indicate if the device is a mobile.

  See http://51Degrees/Products/DeviceData/PropertyDictionary for a list of
  valid properties.

Example: PerfPat data/51Degrees-Lite.dat million.csv Id,IsMobile

  This program requires no input. It will test with the given data and output
  detection times at the end. This will take several minutes.

ProcTrie
--------

Starts a process which reads user agents from stdIn and returns CSV format
properties and values for the matching device via stdOut. Uses the Pattern 
matching detection routine.

When running type a user agent, or any string for testing, followed by enter to
view the matched device properties. Provide an empty string to terminate the
process.

ProcTrie DataFile [Properties]

[TrieDataFile]  A source data file in Trie format. A data file current at the
  time the zip file was created is included in data folder.

[Properties]  A comma separated list of properties to be returned. If not 
  provided Id will be used.

  For example: Id,IsMobile will return the Id of the matched device and True or
  False to indicate if the device is a mobile.

  See http://51Degrees/Products/DeviceData/PropertyDictionary for a list of
  valid properties.

Example:  ProcTrie data/51Degrees-Lite.Trie IsMobile,Id

  Enter user agent strings to view the returned Id and IsMobile values, or an
  empty string to quit. 

PerfTrie
--------

Runs a performance test using multiple threads and a data file of useragent
strings as input. Uses the Trie matching detection routine which is very fast,
but uses a lot of memory.

This program is only built when using the GCC compiler.

PerfTrie [Trie data file] [Useragents file] [Properties]

[Trie data file]  A source data file in Trie format. A data file current at the
  time the zip file was created is included in data folder.

[Useragents file] A path to a list of user agents to be used in the performance
  test.

  http://51Degrees/Million.zip contains a test file of one million useragents.

[Properties]  A comma separated list of properties to be returned. If not 
  provided Id will be used.

  For example: Id,IsMobile will return the Id of the matched device and True or
  False to indicate if the device is a mobile.

  See http://51Degrees/Products/DeviceData/PropertyDictionary for a list of
  valid properties.

Example: PerfTrie data/51Degrees-Lite.Trie million.csv Id,IsMobile

  This program requires no input. It will test with the given data and output
  detection times at the end. This will take several minutes.
