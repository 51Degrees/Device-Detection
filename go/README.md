![51Degrees](https://51degrees.com/DesktopModules/FiftyOne/Distributor/Logo.ashx?utm_source=github&utm_medium=repository&utm_content=readme_main&utm_campaign=Go-open-source "THE Fastest and Most Accurate Device Detection") **Device Detection in C** Go wrapper

[Supported Databases](https://51degrees.com/compare-data-options?utm_source=github&utm_medium=repository&utm_content=readme_main&utm_campaign=Go-open-source "Different device databases which can be used with 51Degrees device detection") | [Developer Documention](https://51degrees.com/support/documentation?utm_source=github&utm_medium=repository&utm_content=readme_main&utm_campaign=Go-open-source "Full getting started guide and advanced developer documentation") | [Available Properties](https://51degrees.com/resources/property-dictionary?utm_source=github&utm_medium=repository&utm_content=readme_main&utm_campaign=Go-open-source "View all available properties and values")

<sup>Need [C](https://github.com/51Degrees/Device-Detection "THE Fastest and most Accurate device detection for C") | [Java](https://github.com/51Degrees/Java-Device-Detection "THE Fastest and most Accurate device detection for Java") | [PHP](https://github.com/51Degrees/Device-Detection) | [Python](https://github.com/51Degrees/Device-Detection "THE Fastest and most Accurate device detection for Python") | [Perl](https://github.com/51Degrees/Device-Detection "THE Fastest and most Accurate device detection for Perl") | [Node.js](https://github.com/51Degrees/Device-Detection "THE Fastest and most Accurate device detection for Node.js")?</sup>

## Introduction

In Go, use like...
```golang
// Declare Resources.
var provider FiftyOneDegreesPatternV3.Provider
// Initialise provider.
provider = FiftyOneDegreesPatternV3.NewProvider([string]PATH TO DATA FILE)
// Perform detection.
match := provider.GetMatch([string]DEVICE USER AGENT)
//Release the Match Object back to the pool.
FiftyOneDegreesPatternV3.DeleteMatch(match)
```
... to turn User-Agent HTTP headers into useful information about a device such as physical screen size, device price and type of device.

Use this project to detect device properties using HTTP browser User-Agents as input. It can be used to process server web log files, or for real time device detection to support web optimisation.

**Pattern:**  Searches for device signatures in a useragent returning metrics about the validity of the results. Does NOT use regular expressions.

**Hash Trie:** A binary file populated with hashes of User-Agent signatures. Very fast. 

All methods use an external data file which can be easily updated.

## Dependencies
#### Linux/MacOS

- Golang
- make
- SWIG - if regenerating the wrapper.

For Ubuntu based distributions, dependencies can be found on apt. Use:
```
$ sudo apt-get install gcc g++ make swig golang
```

#### Windows

- Golang
- MinGW-w64 - GCC 64bit environment for windows.
- SwigWin - Precompiled SWIG binarys - needed if regenerating the wrapper.

Make sure to add the SwigWin and MinGW to your Path environment variable.

Golang is available in some package managers, if it is not available or if you are using Windows or MacOS you can still install Golang by following the guide on [golang.org](https://golang.org/doc/install). For MacOS, the other dependencies can be obtained using Homebrew.

## Install
Clone the Device-Detection repository:
```
$ git clone https://github.com/51Degrees/Device-Detection.git
```

Move to the go directory:
```
$ cd Device-Detection/go
```
### Linux/MacOS

To set up the environment with the Pattern algorithm, do:

```
$ make pattern
```
For the Hash Trie algorithm, do:
```
$ make hash
```

If you wish to regenerate the swig wrapper as well then do:
`$ make swig-pat` or `$ make swig-hash`

### Windows

To set up the environment with both the Pattern and Hash Trie algorithms, do:

```
> PreBuild.bat
```
For the Hash Trie algorithm, do:
```
> PreBuild.bat HASH
```
If you wish to regenerate the swig wrapper as well then do:
`> PreBuild.bat SWIG` or `> PreBuild.bat HASH SWIG`


## Examples
The go folder contains various examples which show how to use 51Degrees in various use cases, a full explanation of these can be found within the files or at [Go Tutorials](https://51degrees.com/Developers/Documentation/APIs/Go/Tutorials).

The Go api also includes the following examples:

| Filename                      | Description                                                       |
| :---------------------------- | :---------------------------------------------------------------- |
| FindProfiles.go               | Get Profiles with a specific Property:Value                       |
| GettingStarted.go             | Match some User-Agents                                            |
| GettingStartedHashTrie.go     | Match some User-Agents using Hash Trie                            |
| MatchForDeviceId.go           | Match and retieve data from a Device Id                           |
| OfflineProcessing.go          | Process a list of User-Agents                                     |
| OfflineProcessingHashTrie.go  | Process a list of User-Agents using Hash Trie                     |
| PerfHashTrie.go               | Benchmark for Hash Trie algorithm                                 |
| server.go                     | Http server example                                               |
| StronglyTyped.go              | Strongly typed example of matching a User-Agent                   |
| StronglyTypedHashTrie.go      | Strongly typed example of matching a User-Agent using Hash Trie   |

Build an exmaple to running: `go build <filename>.go`. Or run an example by doing: `go run <filename>.go`.

## Common Problems
#### Cannot find package
##### Problem:
`cannot find package "./src/trie" ...`

OR

`cannot find package "./src/pattern" ...`

##### Resolution:

The 51Degrees Go API has not yet been initialized. Run make command if on Linux/MacOS or PreBuild.bat script if on Windows.

##### Problem:

`build constraints exclude all Go files`

##### Resolution:

Set the environment variable CGO_ENABLED to 1.