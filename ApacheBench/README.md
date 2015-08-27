![51Degrees](https://51degrees.com/DesktopModules/FiftyOne/Distributor/Logo.ashx?utm_source=github&utm_medium=repository&utm_content=home&utm_campaign=c-open-source "THE Fasstest and Most Accurate Device Detection") **Apache Bench** modified for User-Agent testing

[Recent Changes](#recent-changes "Review recent major changes") | [Supported Databases](https://51degrees.com/compare-data-options?utm_source=github&utm_medium=repository&utm_content=home-menu&utm_campaign=c-open-source "Different device databases which can be used with 51Degrees device detection") | [Developer Documention](https://51degrees.com/support/documentation?utm_source=github&utm_medium=repository&utm_content=home-menu&utm_campaign=c-open-source "Full getting started guide and advanced developer documentation") | [Available Properties](https://51degrees.com/resources/property-dictionary?utm_source=github&utm_medium=repository&utm_content=home-menu&utm_campaign=c-open-source "View all available properties and values")

<sup>Need [.NET](https://github.com/51Degrees/.NET-Device-Detection "THE Fastest and most Accurate device detection for .NET") | [Java](https://github.com/51Degrees/Java-Device-Detection "THE Fastest and most Accurate device detection for Java") | [PHP Script](https://github.com/51Degrees/51Degrees-PHP)?</sup>

A version of the Apache Benchmarking tool modified to randomly generate User-Agent headers. For more information on the original project including building this version see the [Apache HTTP server benchmarking tool](http://httpd.apache.org/docs/2.2/programs/ab.html) documentation.

The tool is used to stress device detection solutions by providing unique unlikely User-Agents during benchmarking. The original implementation was poorly suited to device detection testing as it repeated a single set of HTTP headers for every test.

## New Argument

This version can be provided with a text file containing User-Agents where each User-Agent is a single line in the file. Each request will select a User-Agent at random and further modify up to 10 random characters.

Use the U argument to specify a source of User-Agents. The following example uses the standard User Agents CSV file included in this repository.

```
./ab -c 10 -n 10000 -U "../data/20000 User Agents.csv" http://127.0.0.1:8081/
```
