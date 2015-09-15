<<<<<<< HEAD

51degrees.node
==============
[![Gitter](https://badges.gitter.im/Join Chat.svg)](https://gitter.im/51Degreesmobi/51degrees.node?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

51degrees c-sdk native bindings for nodejs, it helps you detect devices from `userAgent` in high performance.
=======
![51Degrees](https://51degrees.com/DesktopModules/FiftyOne/Distributor/Logo.ashx?utm_source=github&utm_medium=repository&utm_content=home&utm_campaign=c-open-source "THE Fasstest and Most Accurate Device Detection") **Device Detection for Node.js**

[Recent Changes](#recent-changes "Review recent major changes") | [Supported Databases](https://51degrees.com/compare-data-options?utm_source=github&utm_medium=repository&utm_content=home-menu&utm_campaign=c-open-source "Different device databases which can be used with 51Degrees device detection") | [Developer Documention](https://51degrees.com/support/documentation/nodejs?utm_source=github&utm_medium=repository&utm_content=home-menu&utm_campaign=c-open-source "Full getting started guide and advanced developer documentation") | [Available Properties](https://51degrees.com/resources/property-dictionary?utm_source=github&utm_medium=repository&utm_content=home-menu&utm_campaign=c-open-source "View all available properties and values")

<sup>Need [.NET](https://github.com/51Degrees/.NET-Device-Detection "THE Fastest and most Accurate device detection for .NET") | [Java](https://github.com/51Degrees/Java-Device-Detection "THE Fastest and most Accurate device detection for Java") | [PHP Script](https://github.com/51Degrees/51Degrees-PHP)?</sup>

[![Gitter](https://badges.gitter.im/Join Chat.svg)](https://gitter.im/51Degreesmobi/51degrees.node?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

51degrees c-sdk native bindings for nodejs, help you detect details like device type, screen size and input methods from `userAgent`.
>>>>>>> V32

[![NPM](https://nodei.co/npm/51degrees.png?stars&downloads)](https://nodei.co/npm/51degrees/)
[![NPM](https://nodei.co/npm-dl/51degrees.png)](https://nodei.co/npm/51degrees/)

Windows        | Mac/Linux   
-------------- | ------------
[![Build status](https://ci.appveyor.com/api/projects/status/m1nwwmospqiipyeu?svg=true)](https://ci.appveyor.com/project/yorkie/51degrees-node) | [![Build Status](https://travis-ci.org/51Degreesmobi/51degrees.node.svg?branch=master)](https://travis-ci.org/51Degreesmobi/51degrees.node)

<<<<<<< HEAD

=======
>>>>>>> V32
### API

##### `.Parse(filename[, properties, options])`

<<<<<<< HEAD
* `filename` {String} your 51degrees data, lite or premium.

* `properties` {Array} optional, required properties
=======
* `filename` {String} your 51degrees data (lite, premium or enterprise)

* `properties` {Array} optional, required properties. returns all possible properties if not provided
>>>>>>> V32

* `options` {Object}

* `options.autoUpdate` {Boolean} enable/disable `autoUpdate` feature

* `options.key` {String} auto-update requires a licesen key from 51Degrees

* `options.interval` {Integer}, default value 30 * 60 * 1000 (half hours)

* `options.onupdated` {Function} if auto updated, you will get call at this function

<<<<<<< HEAD
for more information, you could move to [51degrees documentation](https://51degrees.com/Support/Documentation)
=======
for more information, you could move to [51Degrees Node.js documentation](https://51degrees.com/support/documentation/nodejs?utm_source=github&utm_medium=repository&utm_content=home-cta&utm_campaign=c-open-source)
>>>>>>> V32

##### `parser.parse(userAgent)`

* `userAgent` {String}

<<<<<<< HEAD
parse the `userAgent` given by you, and return result of that.
=======
parse the `userAgent` given by you, and return the property values for the device, operating system and browser.
>>>>>>> V32

### Usage

```js
var Parser = require('51degrees').Parser;
<<<<<<< HEAD
var psr = new Parser('51Degrees-Lite.dat');
=======
var psr = new Parser('data/51Degrees-LiteV3.2.dat');
>>>>>>> V32
var userAgent = '...'; // your userAgent in any clients(browser/ios/android)
var ret = psr.parse(userAgent);
console.log(ret);
```

<<<<<<< HEAD
##### Create an auto-updated pattern parser

```js
var psr = new Parser('51Degrees-Lite.dat', [], {
=======
provides all the available information about the device including data about the accuracy of the results eliminating false positives.

##### Create an auto-updated pattern parser

```js
var psr = new Parser('51Degrees-LiteV3.2.dat', [], {
>>>>>>> V32
  autoUpdate: true,
  key: 'your license key'
});
```

<<<<<<< HEAD
**Note**: this feature only is used at **pattern** data.

After the above program, you will get:

```js
{ Id: '17595-21721-21635-18092',
  Canvas: true,
  CssTransforms: true,
  CssTransitions: true,
  History: true,
  Html5: true,
  IndexedDB: true,
  IsMobile: false,
  Json: true,
  PostMessage: true,
  Svg: true,
  TouchEvents: true,
  WebWorkers: true,
  method: 'trie',
  data: {} }
```

##### `.ALL_PROPERTIES`

`51Degrees.node` exposure `ALL_PROPERTIES` to shortly fetch all propertites from `userAgent`, just using like this:

```js
var properties = require('51degrees').ALL_PROPERTIES;
var userAgent = '...' // your userAgent in any clients(browser/ios/android)
var psr = new Parser('51Degrees-Lite.trie', properties);
var ret = psr.parse(userAgent);
console.log(ret);
```

Then will print the whole properties, it's super useful for overview of this module and 51Degrees product.

##### Properties

The following list shows all properties that 51degrees would export in json:

* `AnimationTiming`
* `BlobBuilder`
* `Canvas`
* `CssBackground`
* `CssBorderImage`
* `CssCanvas`
* `CssColor`
* `CssColumn`
* `CssFlexbox`
* `CssFont`
* `CssImages`
* `CssMediaQueries`
* `CssMinMax`
* `CssOverflow`
* `CssPosition`
* `CssText`
* `CssTransforms`
* `CssTransitions`
* `CssUI`
* `DataSet`
* `DataUrl`
* `DeviceOrientation`
* `FileReader`
* `FileSaver`
* `FileWriter`
* `FormData`
* `Fullscreen`
* `GeoLocation`
* `History`
* `Html5`
* `Html-Media-Capture`
* `Id`
* `Iframe`
* `IndexedDB`
* `IsMobile`
* `Json`
* `LayoutEngine`
* `Masking`
* `PostMessage`
* `Progress`
* `Prompts`
* `ScreenPixelsHeight`
* `ScreenPixelsWidth`
* `Selector`
* `Svg`
* `TouchEvents`
* `Track`
* `Video`
* `Viewport`
* `WebWorkers`
* `Xhr`
=======
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
>>>>>>> V32

### Installation

```
$ npm install 51degrees --save
```

### License

<<<<<<< HEAD
[MPL](License.txt)

=======
[MPL](License.txt)
>>>>>>> V32
