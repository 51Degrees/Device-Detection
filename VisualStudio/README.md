![51Degrees](https://51degrees.com/DesktopModules/FiftyOne/Distributor/Logo.ashx?utm_source=github&utm_medium=repository&utm_content=home&utm_campaign=c-visual-studio-open-source "THE Fasstest and Most Accurate Device Detection") **Device Detection in C & C++** for Visual Studio

[Recent Changes](#recent-changes "Review recent major changes") | [Supported Databases](https://51degrees.com/compare-data-options?utm_source=github&utm_medium=repository&utm_content=home-menu&utm_campaign=c-visual-studio-open-source "Different device databases which can be used with 51Degrees device detection") | [Developer Documention](https://51degrees.com/support/documentation?utm_source=github&utm_medium=repository&utm_content=home-menu&utm_campaign=c-visual-studio-open-source "Full getting started guide and advanced developer documentation") | [Available Properties](https://51degrees.com/resources/property-dictionary?utm_source=github&utm_medium=repository&utm_content=home-menu&utm_campaign=c-visual-studio-open-source "View all available properties and values")

<sup>Need [.NET](https://github.com/51Degrees/.NET-Device-Detection "THE Fastest and most Accurate device detection for .NET") | [Java](https://github.com/51Degrees/Java-Device-Detection "THE Fastest and most Accurate device detection for Java") | [PHP Script](https://github.com/51Degrees/51Degrees-PHP)?</sup>

This Visual Studio solution contains example and test projects for the C and C++ device detection code. The following list of projects are available.

* Managed Code:
  * Console Interop - a console application to output basic device detection results.
  * FiftyOne.Mobile.Detection.Provider.Interop - exposes unmanaged C / C++ DLLs to managed .NET code via providers (wrappers).
  * FiftyOne.Mobile.Detection.Provider.Pattern - SWIG generated code to expose C / C++ classes for Pattern algorithm to .NET.
  * FiftyOne.Mobile.Detection.Provider.Trie - SWIG generated code to expose C / C++ classes for Trie algorithm to .NET.
  * FiftyOne.Reconcile - compares the results of device detection between C# and C / C++ implementations reporting any exceptions (see Dependencies).
  * FiftyOne.UnitTests - standard tests for the C / C++ code.
  * FiftyOne.WebSite - an example implementation of the Interop project used in a web site.
* Profile:
  * FiftyOne.Mobile.Detection.Performance - C++ CPU tester for performance profiling
  * FiftyOne.Mobile.Detection.Memory - C# tester for memory profiling
* Standard Demos - MSVC implementations of basic C examples:
  * Console - simple example console application.
  * PerfPat - performance tester for Pattern algorithm.
  * PerfTrie - performance tester for Trie algorithm.
  * ProcPat - stdin/stdout interface for Pattern algorithm.
  * ProcTrie - stdin/stdout interface for Trie algorithm.

## Dependencies

The .NET-Device-Detection repository should be cloned into the same folder as the Device-Detection repository. The .NET managed code device detection project is used by the reconcilation tests to compare detection results from both the C / C++ source code and the .NET source code. If these tests do not need to be executed the warning about a missing project can be ignored when the solution is opened.

## Tests

The Visual Studio project contains the main unit and system tests for the C and C++ code. The full set of tests require Premium and Enterprise data files to be placed into the Device-Detection / data folder alongside the Lite files included in the repository.

When run in Visual Studio ensure the **Test -> Test Settings -> Default Processor Architecture** is set to **x64**. If the default value of x86 is used no tests will appear in the Test Explorer after the solution has been rebuilt.