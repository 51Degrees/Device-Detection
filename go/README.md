![51Degrees](https://51degrees.com/DesktopModules/FiftyOne/Distributor/Logo.ashx?utm_source=github&utm_medium=repository&utm_content=readme_main&utm_campaign=Go-open-source "THE Fastest and Most Accurate Device Detection") **Device Detection in C** Go wrapper

[Supported Databases](https://51degrees.com/compare-data-options?utm_source=github&utm_medium=repository&utm_content=readme_main&utm_campaign=Go-open-source "Different device databases which can be used with 51Degrees device detection") | [Developer Documention](https://51degrees.com/support/documentation?utm_source=github&utm_medium=repository&utm_content=readme_main&utm_campaign=Go-open-source "Full getting started guide and advanced developer documentation") | [Available Properties](https://51degrees.com/resources/property-dictionary?utm_source=github&utm_medium=repository&utm_content=readme_main&utm_campaign=Go-open-source "View all available properties and values")

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

**Pattern:**  Searches for device signatures in a useragent returning metrics about the validity of the results. Does NOT use regular expressions.

**Hash Trie:** A binary file populated with hashes of User-Agent signatures. Very fast. 

All methods use an external data file which can be easily updated.

## Dependencies
#### Essential

- golang
- make
- SWIG

For Ubuntu based distributions, dependencies can be found on apt. Use:
```
$ sudo apt-get install gcc g++ make swig golang
```
Golang is available in some package managers, if it is not available or if you are using Windows or MacOS you can still install Golang by following the guide on [golang.org](https://golang.org/doc/install)

## Install
<installation>
### Linux

Move to the go directory:
```
$ cd ../go
```
To set up the environment with the Pattern algorithm, do:

```
$ make
```
For the Hash Trie algorithm, do:
```
$ make TRIE=1
```
</installation>


#### Examples
The go folder contains various examples which show how to use 51Degrees in various use cases, including:
- HTTP Server
- Offline processing
- Finding profiles
- Performance Testing

A full explanation of these can be found within the files or at [Go Tutorials](https://51degrees.com/Developers/Documentation/APIs/Go/Tutorials).

The Go api also include the following examples:

- FindProfiles.go
- GettingStarted.go
- GettingStartedHashTrie.go
- MatchForDeviceId.go
- OfflineProcessing.go
- OfflineProcessingHashTrie.go
- PerfHashTrie.go
- server.go
- StronglyTyped.go
- StronglyTypedHashTrie.go