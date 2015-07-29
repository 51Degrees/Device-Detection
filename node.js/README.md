![51Degrees](https://51degrees.com/DesktopModules/FiftyOne/Distributor/Logo.ashx?utm_source=github&utm_medium=repository&utm_content=home&utm_campaign=c-open-source "THE Fasstest and Most Accurate Device Detection") **Device Detection for Node.js**

[Recent Changes](#recent-changes "Review recent major changes") | [Supported Databases](https://51degrees.com/compare-data-options?utm_source=github&utm_medium=repository&utm_content=home-menu&utm_campaign=c-open-source "Different device databases which can be used with 51Degrees device detection") | [Developer Documention](https://51degrees.com/support/documentation/nodejs?utm_source=github&utm_medium=repository&utm_content=home-menu&utm_campaign=c-open-source "Full getting started guide and advanced developer documentation") | [Available Properties](https://51degrees.com/resources/property-dictionary?utm_source=github&utm_medium=repository&utm_content=home-menu&utm_campaign=c-open-source "View all available properties and values")

<sup>Need [.NET](https://github.com/51Degrees/.NET-Device-Detection "THE Fastest and most Accurate device detection for .NET") | [Java](https://github.com/51Degrees/Java-Device-Detection "THE Fastest and most Accurate device detection for Java") | [PHP Script](https://github.com/51Degrees/51Degrees-PHP)?</sup>

[![Gitter](https://badges.gitter.im/Join Chat.svg)](https://gitter.im/51Degreesmobi/51degrees.node?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

51degrees c-sdk native bindings for nodejs, help you detect details like device type, screen size and input methods from `userAgent`.

[![NPM](https://nodei.co/npm/51degrees.png?stars&downloads)](https://nodei.co/npm/51degrees/)
[![NPM](https://nodei.co/npm-dl/51degrees.png)](https://nodei.co/npm/51degrees/)

Windows        | Mac/Linux   
-------------- | ------------
[![Build status](https://ci.appveyor.com/api/projects/status/m1nwwmospqiipyeu?svg=true)](https://ci.appveyor.com/project/yorkie/51degrees-node) | [![Build Status](https://travis-ci.org/51Degreesmobi/51degrees.node.svg?branch=master)](https://travis-ci.org/51Degreesmobi/51degrees.node)

### API

##### `.Parse(filename[, properties, options])`

* `filename` {String} your 51degrees data (lite, premium or enterprise)

* `properties` {Array} optional, required properties. returns all possible properties if not provided

* `options` {Object}

* `options.autoUpdate` {Boolean} enable/disable `autoUpdate` feature

* `options.key` {String} auto-update requires a licesen key from 51Degrees

* `options.interval` {Integer}, default value 30 * 60 * 1000 (half hours)

* `options.onupdated` {Function} if auto updated, you will get call at this function

for more information, you could move to [51Degrees Node.js documentation](https://51degrees.com/support/documentation/nodejs)

##### `parser.parse(userAgent)`

* `userAgent` {String}

parse the `userAgent` given by you, and return the property values for the device, operating system and browser.

### Usage

```js
var Parser = require('51degrees').Parser;
var psr = new Parser('data/51Degrees-LiteV3.2.dat');
var userAgent = '...'; // your userAgent in any clients(browser/ios/android)
var ret = psr.parse(userAgent);
console.log(ret);
```

provides all the available information about the device including data about the accuracy of the results eliminating false positives.

##### Create an auto-updated pattern parser

```js
var psr = new Parser('51Degrees-LiteV3.2.dat', [], {
  autoUpdate: true,
  key: 'your license key'
});
```

**Note**: this feature works with **pattern** data only at this time.

**[Compare Device Databases](https://51degrees.com/compare-data-options?utm_source=github&utm_medium=repository&utm_content=home-cta&utm_campaign=c-open-source "Compare different data file options for 51Degrees device detection")**

##### `.availableProperties()`

Fetch all properties available from the data set and parser with `availableProperties`.

```js
var psr = new Parser('data/51Degrees-LiteV3.2.trie');
console.log(psr.availableProperties());
```

Then will print all the properties. It's super useful to provide overview of this module and 51Degrees product.

**[Review All Properties](https://51degrees.com/resources/property-dictionary?utm_source=github&utm_medium=repository&utm_content=home-cta&utm_campaign=c-open-source" View all available properties and values")**

### Installation

```
$ npm install 51degrees --save
```

### License

[MPL](License.txt)