![51Degrees](https://51degrees.com/DesktopModules/FiftyOne/Distributor/Logo.ashx?utm_source=github&utm_medium=repository&utm_content=readme_main&utm_campaign=node-open-source "THE Fastest and Most Accurate Device Detection") **Device Detection in C** Node.js wrapper

[Supported Databases](https://51degrees.com/compare-data-options?utm_source=github&utm_medium=repository&utm_content=compare-data-options&utm_campaign=node-open-source "Different device databases which can be used with 51Degrees device detection") | [Developer Documention](https://51degrees.com/support/documentation?utm_source=github&utm_medium=repository&utm_content=documentation&utm_campaign=node-open-source "Full getting started guide and advanced developer documentation") | [Available Properties](https://51degrees.com/resources/property-dictionary?utm_source=github&utm_medium=repository&utm_content=property_dictionary&utm_campaign=node-open-source "View all available properties and values")

<sup>Need [.NET](https://github.com/51Degrees/.NET-Device-Detection "THE Fastest and most Accurate device detection for .NET") | [Java](https://github.com/51Degrees/Java-Device-Detection "THE Fastest and most Accurate device detection for Java") | [PHP Script](https://github.com/51Degrees/51Degrees-PHP)?</sup>

## Introduction

In Node, use like...
```js
var FiftyOneDegrees = require('FiftyOneDegreesV3');
var provider = new FiftyOneDegrees.provider('/path/to/config/file');
var match = provider.getMatch(userAgent;
console.log(match['IsMobile']);
console.log(match['DeviceType']);
match.close();
```
... to turn User-Agent HTTP headers into useful information about physical screen size, device price and type of device.

Use this project to detect device properties using HTTP browser User-Agents as input. It can be used to process server web log files, or for real time device detection to support web optimisation.

Two detection methods are supported.

**Pattern:**  Searches for device signatures in a useragent returning metrics about the validity of the results. Does NOT use regular expressions.

**Trie:** A large binary Trie (pronounced Try) populated with User-Agent signatures. Very fast.

All methods use an external data file which can easilly be updated.

## Dependencies
#### Essential

- node.js
- node-gyp
- g++

For Ubuntu based distributions these can be found on apt, use
```
$ sudo apt-get install node.js g++
$ npm install -g node-gyp
```

#### Optional
To use the express example you will need
- express

This can be found on npm, use
```
$ npm install -g express
```

To run the tests you will need
- mocha

This can be found on npm, use
```
$ npm install -g mocha
```

## Install
<installation>
### Linux
#### From git
However, if you want to build the package yourself and use the examples too.
First clone 51Degrees/Device-Detection repository with
```
$ git clone https://github.com/51Degrees/Device-Detection.git
```
Move to the node.js directory with
```
$ cd Device-Detection/node.js
```
and install with
```
$ npm install
```
</installation>
When you're done installing, check it's set up correctly by recreating the output in the Usage section below. Or if you've cloned this repository, try the examples.
## Configure
<configuration>
Before you start matching user agents, you may wish to configure the solution to use a different datadase for example. The config file used when initialising the provider is where all settings can be changed. It is also possible to initialise the provider with a json object in the same format as the config file
### Settings
#### General Settings
 - ``dataFile``. Location on the Pattern or Trie database file.
 
 - ``properties`` (defaults to all available). List of case-sensitive property names to be fetched on every device detection. Leave empty to fetch all available properties.

 - ``LicenceKey``. Your 51Degrees license key. This is required if you want to set up the automatic package updates.

#### Pattern Detector Settings
 - ``cacheSize`` (defaults to ``10000``). Sets the size of the workset cache.

 - ``poolSize`` (defaults to ``20``). Sets the size of the workset pool.

#### Usage Sharer Settings
 - ``UsageSharingEnabled`` (defaults to ``true``). Indicates if usage data should be shared with 51Degrees.com. We recommended leaving this value unchanged to ensure we're improving the performance and accuracy of the solution.
</configuration>
#### Automatic updates
If you want to set up automatic updates, add your license key to your settings and the provider will automatically update the data file.

## Usage
To check everything is set up , try fetching a match by opening a node console, load the module and start matching with
```js
> var FiftyOneDegrees = require('FiftyOneDegreesV3');
> var config = {'dataFile' : 'data/51Degrees-LiteV3.2.dat',
                'properties' : 'BrowserName,ScreenPixelsWidth'};
> var provider = FiftyOneDegrees.provider(config);
> var device = provider.getMatch("Mozilla/5.0 (iPad; CPU OS 5_1 like Mac OS X) AppleWebKit/534.46 (KHTML, like Gecko) Mobile/9B176")

> device.Id
'15767-18117-17776-18092'

> device.BrowserName
'MobileSafari'

> device.ScreenPixelsWidth
'1024'
```

#### Examples
In the examples folder, you can find examples of various functionalities that the 51Degrees detector has such as:
- Matching with a User-Agent
- Matching with a device id
- Evaluating match metrics
- Offline processing
- Strongly typed variables
- Finding profiles

There are also the following server examples

**server.js** which runs a server at localhost:8080 by default and returns matches from the web browsers User-Agent and all relevant HTTP headers seperately. Is run with
```
$ node server.js
```
or
```
$ node serverTrie.js
```

#### Http and Express
There are also simple HTTP and express server examples. These can be run with
```
$ node client.js 
```
and 
```
$ node expressClient.js
```