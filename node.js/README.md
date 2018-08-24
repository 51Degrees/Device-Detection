![51Degrees](https://51degrees.com/DesktopModules/FiftyOne/Distributor/Logo.ashx?utm_source=github&utm_medium=repository&utm_content=readme_main&utm_campaign=node-open-source "THE Fastest and Most Accurate Device Detection") **Device Detection in C** Node.js wrapper

[Supported Databases](https://51degrees.com/compare-data-options?utm_source=github&utm_medium=repository&utm_content=compare-data-options&utm_campaign=node-open-source "Different device databases which can be used with 51Degrees device detection") | [Developer Documention](https://51degrees.com/support/documentation?utm_source=github&utm_medium=repository&utm_content=documentation&utm_campaign=node-open-source "Full getting started guide and advanced developer documentation") | [Available Properties](https://51degrees.com/resources/property-dictionary?utm_source=github&utm_medium=repository&utm_content=property_dictionary&utm_campaign=node-open-source "View all available properties and values")

<sup>Need [.NET](https://github.com/51Degrees/.NET-Device-Detection "THE Fastest and most Accurate device detection for .NET") | [Java](https://github.com/51Degrees/Java-Device-Detection "THE Fastest and most Accurate device detection for Java") | [PHP Script](https://github.com/51Degrees/51Degrees-PHP)?</sup>

## Introduction
Use this project with core node.js, HTTP module or Express to detect device properties using HTTP browser User-Agents as input. It can be used to process server web log files, or for real time device detection to support web optimisation. The module takes a User-Agent string, HTTP header array or HTTP request object and returns a Match object ontaining properties relating to the device's hardware, software and browser.

In Node, use like...
```js
var fiftyonedegrees = require('fiftyonedegreescore');
var provider = new fiftyonedegrees.provider(config);
var match = provider.getMatch(userAgent);
console.log(match['IsMobile']);
console.log(match['DeviceType']);
match.close();
```
... to turn User-Agent HTTP headers into useful information about physical screen size, device price and type of device.

Two detection methods are supported.

**Pattern:**  Searches for device signatures in a useragent returning metrics about the validity of the results. Does NOT use regular expressions.

**Trie:** A large binary Trie (pronounced Try) populated with User-Agent signatures. Very fast.

All methods use an external data file which can easilly be updated.

## Dependencies

#### Essential
- node.js (https://nodejs.org/en/download/package-manager/)
- node-gyp (https://github.com/nodejs/node-gyp)
- gcc/g++ >=4.7 *or* Visual C++ Build tools

## Install
Install the core 51Degrees module with:
```
$ npm install fiftyonedegreescore
```
This will also install Pattern and Trie Lite data files as dependencies.

### Windows
Node has know issues installing modules in Windows. Three things must be in place before installing.
1. Install Visual C++ Build Tools
2. Install Python 2.7 and run ``npm config set python python2.7``
3. Open a terminal and run ``npm config set msvs_version 2015``

Alternatively, Microsoft have offered an npm package to install all this and can be run (in an elevated Power Shell) with ``npm install -g windows-build-tools``.

Full explanation of windows issues can be found at https://github.com/Microsoft/nodejs-guidelines/blob/master/windows-environment.md

When you're done installing, check it's set up correctly by recreating the output in the Usage section below. Or if you've cloned this repository, try the examples.

### GitHub
If installing from the GitHub repository, the source should be coppied before installing.

Move to the node.js directory with
```
$ cd Device-Detection/node.js
```
copy the source and data files to the fiftyonedegreescore directory with
```
$ node copySource.js
```
install the local packages in order with
```
$ npm install ./fiftyonedegreeslitepattern
$ npm install ./fiftyonedegreeslitetrie
$ npm install ./fiftyonedegreescore
```

## Configure
The config json object is where all the settings can be changed, it is also possible to initialise the provider a path to a .json file in the same format.

The most basic example using just a data file  can be seen below:
```js
var pattern = require('fiftyonedegreeslitepattern');
var trie = require('fiftyonedegreeslitetrie');

var config = {"dataFile" : pattern};

var provider = new fiftyonedegreescore(config);
```
### Settings

#### General Settings
 - String ``dataFile``. (defaults to the ``fiftyonedegreeslitepattern`` package). Path to a Pattern or Trie database file.
 
 - String ``properties`` (defaults to all available). Comma separated list of case-sensitive property names to be fetched on every device detection. Leave empty to fetch all available properties.

 - String ``License``. Your 51Degrees license key. This is required if you want to set up the automatic package updates.
 
#### Pattern Specific Settings
 - Integer ``cacheSize`` (defaults to ``10000``). Sets the size of the workset cache.

 - Integer ``poolSize`` (defaults to ``20``). Sets the size of the workset pool.

#### Usage Sharer Settings
 - Boolean ``UsageSharingEnabled`` (defaults to ``true``). Indicates if usage data should be shared with 51Degrees.com. We recommended leaving this value unchanged to ensure we're improving the performance and accuracy of the solution. If this is enabled, you may need to allow HTTPS traffic to the 51Degrees address.
 
#### Interface Settings
 - Boolean ``setGetters`` (defaults to ``true``). Indicates whether getters are set for the match object so that ``match.IsMobile`` can be used in place of ``match.getValue('IsMobile')``. This makes things simpler when using properties that can return an array of values.
 
 - Boolean ``stronglyTyped`` (defaults to ``true``). Indicates whether the match getters return boolean values as a string (``True``/``False``) or a boolean type (``true``/``false``).
 
#### Automatic updates
If you want to set up automatic updates, add your license key to your settings and the provider will automatically update the data file whenvever a new one is available. This will replace the file in the location it has been loaded from when the provider was initialised.

For Lite users, the data file can be updated by updating the Lite data package (either ``fiftyonedegreeslitepattern`` or ``fiftyonedegreeslitetrie``).

For more information on data options, see https://51degrees.com/compare-data-options

## Usage
To check everything is set up , try fetching a match, load the module and start matching with
```js
var fiftyonedegrees = require('fiftyonedegreescore');

var config = {'dataFile' : require('fiftyonedegreeslitepattern'),
              'properties' : 'BrowserName,ScreenPixelsWidth'};

var provider = fiftyonedegrees.provider(config);

var device = provider.getMatch("Mozilla/5.0 (iPad; CPU OS 5_1 like Mac OS X) AppleWebKit/534.46 (KHTML, like Gecko) Mobile/9B176");

console.log(device.Id);
console.log(device.BrowserName);
console.log(device.ScreenPixelsWidth);

device.close();
```
which will output:
```
15767-18117-17776-18092
MobileSafari
1024
```

#### Provider
The provider is initialised with config object or file path as detailed in the configuration section above. This is done with:
```js
var fiftyonedegreescore = require('fiftyonedegreescore');
var config = {'dataFile' : require('fiftyonedegreeslitepattern')};
var provider = new fiftyonedegreescore.provider(config);
```

#### Match
Using the provider's ``getMatch`` function with a User-Agent string returns a match object from which the properties of the device can be extracted:
```js
var userAgent = '[User-Agent String]';
var match = provider.getMatch(userAgent);
var isMobile = match.IsMobile;

match.close();
```
The ``close`` method must be called when the Match object is no longer needed to release it back to the provider's pool for reuse.

##### Muliple HTTP Headers
A match can also be carried out on multiple HTTP headers in the same way, where instead of passing in a User-Agent string, a JSON object containing the headers is used like
```js
var headers = {'user-agent': userAgent,
               etc...
               };
var match = provider.getMatch(headers);
match.close();
```

##### HTTP Request
When used in an HTTP server the request object can be used. Using this method means that the match object is closed when the request ends so there is no need to implement ``match.close()``. The match object is also attached to the request as ``fiftyoneDevice`` when ``getMatch()`` is called so can be accessed in one of two ways:

1. The same as with a match outside of an HTTP server
```js
var match = provider.getMatch(request);
var isMobile = match.IsMobile;
```
2. Through the request object
```js
provider.getMatch(request);
var isMobile = request.fiftyoneDevice.IsMobile;
```

#### Examples
In the examples folder, you can find examples of various functionalities that the 51Degrees detector has such as:
- Matching with a User-Agent
- Matching with a device id
- Evaluating match metrics
- Offline processing
- Strongly typed variables
- Finding profiles

Full example documentation can be found at https://51Degrees.com/Support/Documentation/APIs/Nodejs-V32/Tutorials

#### Example Server
The example **server.js** runs a server at localhost:8080 by default and returns matches from the web browsers User-Agent and all relevant HTTP headers seperately. Is run with
```
$ node examples/server.js pattern
```
or
```
$ node examples/server.js trie
```

#### Http and Express
There are also simple HTTP and express server examples. These can be run with
```
$ node examples/client.js 
```
and 
```
$ node examples/expressClient.js
```

##### expressClient.js dependencies
- express (https://expressjs.com/en/starter/installing.html)


### Logging
Log events are emitted via ``fiftyonedegreescore.log`` with the possible events being **error**, **info** and **debug**. A function can be attached to any of these events in the expected way like
```js
fiftyonedegreescore.log.on('error', function(err) {
    // Oh no, an error!
    console.log(err);
})
```
to print the an error.

**NOTE: if the ``error`` event is not listened for, node will treat it as a full blown error and exit the process. So be sure to add the above function as a minimum.**

### Tests
API and performance tests can be run from the module directory with either
```
$ mocha test.js --pattern
```
or
```
$ mocha test.js --trie
```

##### Test dependencies
- mocha (https://mochajs.org/#installation)
