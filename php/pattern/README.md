![51Degrees](https://51degrees.com/DesktopModules/FiftyOne/Distributor/Logo.ashx?utm_source=Github&utm_medium=repository&utm_content=readme_pattern&utm_campaign=php-open-source "THE Fastest and Most Accurate Device Detection") **Device Detection in C** PHP C extension

[Supported Databases](https://51degrees.com/compare-data-options?utm_source=Github&utm_medium=repository&utm_content=readme_pattern&utm_campaign=php-open-source "Different device databases which can be used with 51Degrees device detection") | [Developer Documention](https://51degrees.com/support/documentation?utm_source=Github&utm_medium=repository&utm_content=readme_pattern&utm_campaign=php-open-source "Full getting started guide and advanced developer documentation") | [Available Properties](https://51degrees.com/resources/property-dictionary?utm_source=Github&utm_medium=repository&utm_content=readme_pattern&utm_campaign=php-open-source "View all available properties and values")

<sup>Need [.NET](https://github.com/51Degrees/.NET-Device-Detection "THE Fastest and most Accurate device detection for .NET") | [Java](https://github.com/51Degrees/Java-Device-Detection "THE Fastest and most Accurate device detection for Java") | [PHP Script](https://github.com/51Degrees/51Degrees-PHP)?</sup>

# Pattern

Searches for device signatures in a useragent returning metrics about the validity of the results. Does NOT use regular expressions.

# Included files
File Name | Description
------------ | -------------
README.md | this file.
config.m4 | checks the environment and creates the makefile to be used for building.
src/fiftyone_degrees_v3_extension.c | contains the implementation of the 51Degrees PHP C extension for Pattern.
src/php_fiftyone_degrees_v3_extension.h | the header file that contains definitions and function prototypes.
