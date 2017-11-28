![51Degrees](https://51degrees.com/DesktopModules/FiftyOne/Distributor/Logo.ashx?utm_source=github&utm_medium=repository&utm_content=readme_main&utm_campaign=lua-open-source "THE Fastest and Most Accurate Device Detection") **Device Detection in C** Lua wrapper

[Supported Databases](https://51degrees.com/compare-data-options?utm_source=github&utm_medium=repository&utm_content=compare-data-options&utm_campaign=lua-open-source "Different device databases which can be used with 51Degrees device detection") | [Developer Documention](https://51degrees.com/support/documentation?utm_source=github&utm_medium=repository&utm_content=documentation&utm_campaign=lua-open-source "Full getting started guide and advanced developer documentation") | [Available Properties](https://51degrees.com/resources/property-dictionary?utm_source=github&utm_medium=repository&utm_content=property_dictionary&utm_campaign=lua-open-source "View all available properties and values")

<sup>Need [.NET](https://github.com/51Degrees/.NET-Device-Detection "THE Fastest and most Accurate device detection for .NET") | [Java](https://github.com/51Degrees/Java-Device-Detection "THE Fastest and most Accurate device detection for Java") | [PHP Script](https://github.com/51Degrees/51Degrees-PHP)?</sup>

## Introduction

**Note: Lua module should be considered beta. Any issues can be raised over GitHub.**

Use this project with Lua to detect device properties using HTTP browser User-Agents as input. It can be used to process server web log files, or for real time device detection to support web optimisation. The module takes a User-Agent string or HTTP header array and returns a Match object containing properties relating to the device's hardware, software and browser.

In Node, use like...
```lua
provider=loadfile("../FiftyOneDegrees.lua")(config)
match=provider:getMatch(userAgent)
print(match:getValue("IsMobile"))
```
... to turn User-Agent HTTP headers into useful information about physical screen size, device price and type of device.

Two detection methods are supported.

**Pattern:**  Searches for device signatures in a useragent returning metrics about the validity of the results. Does NOT use regular expressions.

**Trie:** A large binary Trie (pronounced Try) populated with User-Agent signatures. Very fast.

All methods use an external data file which can easilly be updated.

## Dependencies
#### Essential

- Lua
- gcc/g++ >=4.7 *or* Visual C++ Build tools

On Ubuntu do:
```console
sudo apt-get update
sudo apt-get install build-essential lua5.2 liblua5.2-dev
```

## Install
Install the 51Degrees module with:
```
$ make install
```
This will also install Pattern and Trie Lite data files as dependencies.

## Configure
The config json object is where all the settings can be changed.

The most basic example using just a data file  can be seen below:
```lua
config = {dataFile = "path/to/data/file"};
provider = loadfile("FiftyOneDegrees.lua")(config)
```
### Settings
#### General Settings
 - String ``dataFile``. Path to a Pattern or Trie database file.
 
 - String ``properties`` (defaults to all available). Comma separated list of case-sensitive property names to be fetched on every device detection. Leave empty to fetch all available properties.

## Usage
To check everything is set up , try fetching a match, load the module and start matching with
```lua
config={
    dataFile="../data/51Degrees-LiteV3.2.dat",
    properties="BrowserName,ScreenPixelsWidth"}
provider=loadfile("FiftyOneDegrees.lua")(config)

match=provider:getMatch("Mozilla/5.0 (iPad; CPU OS 5_1 like Mac OS X) AppleWebKit/534.46 (KHTML, like Gecko) Mobile/9B176")
print(match:getDeviceId())
print(match:getValue("BrowserName"))
print(match:getValue("BrowserName))
```
which will output:
```
15767-18117-17776-18092
MobileSafari
1024
```

#### Provider
The provider is initialised with config object or file path as detailed in the configuration section above. This is done with:
```lua
config = {
    dataFile = "../data/51Degrees-LiteV3.2.dat",
    properties = "BrowserName,ScreenPixelsWidth"}
provider = loadfile("FiftyOneDegrees.lua")(config)
```
#### Match
Using the provider's ``getMatch`` function with a User-Agent string returns a match object from which the properties of the device can be extracted:
```lua
userAgent = '[User-Agent String]';
match = provider:getMatch(userAgent);
isMobile = match:getValue("IsMobile");
```

##### Muliple HTTP Headers
A match can also be carried out on multiple HTTP headers in the same way, where instead of passing in a User-Agent string, a JSON object containing the headers is used like
```lua
headers = {"User-Agent" =  userAgent,
               etc...
               };
match = provider:getMatch(headers);
```

#### Examples
In the examples folder, you can find examples of various functionalities that the 51Degrees detector has such as:
- Matching with a User-Agent
- Matching with a device id
- Evaluating match metrics
- Offline processing
- Strongly typed variables
- Finding profiles