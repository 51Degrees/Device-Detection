![51Degrees](https://51degrees.com/DesktopModules/FiftyOne/Distributor/Logo.ashx?utm_source=github&utm_medium=repository&utm_content=readme_main&utm_campaign=python-open-source "THE Fastest and Most Accurate Device Detection") **Device Detection in C** Go wrapper

[Supported Databases](https://51degrees.com/compare-data-options?utm_source=github&utm_medium=repository&utm_content=compare-data-options&utm_campaign=python-open-source "Different device databases which can be used with 51Degrees device detection") | [Developer Documention](https://51degrees.com/support/documentation?utm_source=github&utm_medium=repository&utm_content=documentation&utm_campaign=python-open-source "Full getting started guide and advanced developer documentation") | [Available Properties](https://51degrees.com/resources/property-dictionary?utm_source=github&utm_medium=repository&utm_content=property_dictionary&utm_campaign=python-open-source "View all available properties and values")

<sup>Need [C](https://github.com/51Degrees/Device-Detection "THE Fastest and most Accurate device detection for C") | [Java](https://github.com/51Degrees/Java-Device-Detection "THE Fastest and most Accurate device detection for Java") | [PHP](https://github.com/51Degrees/Device-Detection) | [Python](https://github.com/51Degrees/Device-Detection "THE Fastest and most Accurate device detection for Python") | [Perl](https://github.com/51Degrees/Device-Detection "THE Fastest and most Accurate device detection for Perl") | [Node.js](https://github.com/51Degrees/Device-Detection "THE Fastest and most Accurate device detection for Node.js")?</sup>

## Introduction

In Go, use like...
```golang
// Declare Resources.
var provider FiftyOneDegreesPatternV3.Provider
// Initialise provider.
provider = FiftyOneDegreesPatternV3.NewProvider([PATH TO DATA FILE])
// Perform detection.
match := provider.GetMatch([DEVICE USER AGENT])
//Release the Match Object back to the pool.
FiftyOneDegreesPatternV3.DeleteMatch(match)
```
... to turn User-Agent HTTP headers into useful information about physical screen size, device price and type of device.

Use this project to detect device properties using HTTP browser User-Agents as input. It can be used to process server web log files, or for real time device detection to support web optimisation.

Currently only one method is supported.

**Pattern:**  Searches for device signatures in a useragent returning metrics about the validity of the results. Does NOT use regular expressions.

All methods use an external data file which can easilly be updated.

## Dependencies
#### Essential

- golang
- gcc 5.x
- g++ 5.x
- make
- SWIG

To install Golang, follow the guide on [golang.org](https://golang.org/doc/install)

For Ubuntu based distributions the other dependencies can be found on apt, use
```
$ sudo apt-get install gcc g++ make swig
```

## Install
<installation>
### Linux

Move to the go directory 
```
$ cd ../go
```
Install with

```
$ make
```
</installation>


#### Examples
In the examples folder, you can find examples of various functionalities that the 51Degrees detector has such as:
- HTTP Server
- Offline processing
- Finding profiles

A full explanation of these can be found within the files or at [Go Tutorials](https://51Degrees/support/documentation/Go/tutorials).


Go package also include the following examples

- FindProfiles.go
- GettingStarted.go

