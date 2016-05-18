
![51Degrees](https://51degrees.com/DesktopModules/FiftyOne/Distributor/Logo.ashx?utm_source=github&utm_medium=repository&utm_content=src_home&utm_campaign=c-open-source "THE Fastest and Most Accurate Device Detection") **Device Detection in C / C++** includes PHP, Python, Perl, .NET and Node.js

[Supported Databases](https://51degrees.com/compare-data-options?utm_source=github&utm_medium=repository&utm_content=src_compare-data-options&utm_campaign=c-open-source "Different device databases which can be used with 51Degrees device detection") | [Developer Documention](https://51degrees.com/support/documentation?utm_source=github&utm_medium=repository&utm_content=src_documentation&utm_campaign=c-open-source "Full getting started guide and advanced developer documentation") | [Available Properties](https://51degrees.com/resources/property-dictionary?utm_source=github&utm_medium=repository&utm_content=src_property_dictionary&utm_campaign=c-open-source "View all available properties and values")

<sup>Need [.NET](https://github.com/51Degrees/.NET-Device-Detection "THE Fastest and most Accurate device detection for .NET") | [Java](https://github.com/51Degrees/Java-Device-Detection "THE Fastest and most Accurate device detection for Java") | [PHP Script](https://github.com/51Degrees/51Degrees-PHP)?</sup>

This folder contains all the core source code used by associated C APIs as well as some basic C and C++ examples. The project contain three significant file types:

* Pattern - source code related to the Pattern device detection algorithm.
* Trie - source code related to the Trie device detection algorithm.
* Shared - functions that are provided by 3rd parties or are shared across the two algorithms.

[Understand Trie & Pattern Algorithms](https://51degrees.com/support/documentation/how-device-detection-works?utm_source=github&utm_medium=repository&utm_content=src_algorithms&utm_campaign=c-open-source "Two different methods can be used for Device Detection. We recommend Pattern is used for all except very high performance detection requirements")

# Included Files
| File Name | Description |
| --------- | ----------- |
Shared | |
threading.h | contains synchronisation macros for MSVC and GCC compilers. 
threading.c | provide more complex implementations for GCC compilers. 
cityhash\city.c | a C implementation of the cityhash algorithm used by the Pattern algorithms User-Agent cache.
cityhash\city.h | header file for cityhash implementation.
cityhash\LICENSE | license file for cityhash C implementation.
console\Console.c | the example console for Pattern algorithm in C.
console\Console.cpp | the example console for Pattern algorithm in C++.
snprintf\snprintf.c | a C implementation of the snprintf method. No longer used.
snprintf\snprintf.h | header file for snprintf implementation.
snprintf\LICENSE.txt | license file for the snprintf implementation.
Pattern | |
pattern\51Degrees.c | the core Pattern algorithm implementation in C.
pattern\51Degrees.h | the header file for the core Pattern algorithm implementation in C.
pattern\51Degrees.i | SWIG interface file for Pattern algorithm. Used for Perl, Python, and C#.
pattern\51Degrees_csharp.cxx | output C++ interface file from SWIG for use with C#. See Visual Studio / FiftyOne.Mobile.Detection.Provider.Pattern project for usage.
pattern\Match.cpp | C++ class encapsulating the results of Pattern device detection. Used to retrieve property values and match metrics.
pattern\Match.hpp | header file for Match class.
pattern\PerfPat.c | example C console application to measure the performance of the Pattern algorithm.
pattern\ProcPat.c | example C application to write input data and read results for Pattern algorithm stdin and stdout.
pattern\Provider.cpp | C++ class encapsulating the C Pattern algorithm, data set, cache and workset pool. Once initialised used to return Match instances.
pattern\Provider.hpp | header file for Pattern Provider class.
pattern\Profiles.cpp | C++ class encapsulating the Profiles retrieval functionality.
pattern\Profiles.hpp | header file for the Pattern Profiles class.
Trie | |
trie\51Degrees.c | the core Trie algorithm implementation in C.
trie\51Degrees.h | the header file for the core Trie algorithm implementation in C.
trie\51Degrees.i | SWIG interface file for Trie algorithm. Used for Perl, Python, and C#.
trie\51Degrees_csharp.cxx | output C++ interface file from SWIG for use with C#. See Visual Studio / FiftyOne.Mobile.Detection.Provider.Trie project for usage.
trie\Match.cpp | C++ class encapsulating the results of Trie device detection. Used to retrieve property values and match metrics.
trie\Match.hpp | header file for Match class.
trie\PerfTrie.c | example C console application to measure the performance of the Trie algorithm.
trie\ProcTrie.c | example C application to write input data and read results for Trie algorithm stdin and stdout.
trie\Provider.cpp | C++ class encapsulating the C Trie algorithm, data set, cache and workset pool. Once initialised used to return Match instances.
trie\Provider.hpp | header file for Trie Provider class.
