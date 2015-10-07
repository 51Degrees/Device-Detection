![51Degrees](https://51degrees.com/DesktopModules/FiftyOne/Distributor/Logo.ashx?utm_source=Github&utm_medium=repository&utm_content=readme_main&utm_campaign=php-open-source "THE Fastest and Most Accurate Device Detection") **Device Detection in C** PHP C extension

[Recent Changes](#recent-changes "Review recent major changes") | [Supported Databases](https://51degrees.com/compare-data-options?utm_source=Github&utm_medium=repository&utm_content=readme_main&utm_campaign=php-open-source "Different device databases which can be used with 51Degrees device detection") | [Developer Documention](https://51degrees.com/support/documentation?utm_source=Github&utm_medium=repository&utm_content=readme_main&utm_campaign=php-open-source "Full getting started guide and advanced developer documentation") | [Available Properties](https://51degrees.com/resources/property-dictionary?utm_source=Github&utm_medium=repository&utm_content=readme_main&utm_campaign=php-open-source "View all available properties and values")

<sup>Need [.NET](https://github.com/51Degrees/.NET-Device-Detection "THE Fastest and most Accurate device detection for .NET") | [Java](https://github.com/51Degrees/Java-Device-Detection "THE Fastest and most Accurate device detection for Java") | [PHP Script](https://github.com/51Degrees/51Degrees-PHP)?</sup>

## Introduction

In PHP, use like...

```php
<?php
$properties = fiftyone_match_with_useragent($_SERVER['HTTP_USER_AGENT']);

// Print all properties.
echo "<pre>";
  var_dump($properties);
echo "</pre>";
// Access individual properties.
$isMobile = $properties['IsMobile'];
if ($isMobile === "True") {
	// Code for mobile device.
} else {
	// Code for Desktop.
}
echo "<p>You are using ".$properties['BrowserName']." browser of version".$properties['BrowserVersion'].".</p>";
echo "<p>The type of your device is: ".$properties['DeviceType'].".</p>";
?>
```

Use this project to detect device properties using HTTP browser User-Agents as input. It can be used to process server web log files, or for real time device detection to support web optimisation.

Two detection methods are supported.

**Pattern:**  Searches for device signatures in a useragent returning metrics about the validity of the results. Does NOT use regular expressions.

**Trie:** A large binary Trie (pronounced Try) populated with User-Agent signatures. Very fast.

All methods use an external data file which can easilly be updated.

## Dependencies

- gcc
- make
- php5
- php5-dev
- git

For Ubuntu based distributions these can be found on apt, use
```
$ sudo apt-get install gcc make php5 php5-dev git
```

## Install
### Linux
First, clone 51Degrees/Device-Detection repository using git:
```console
$ git clone https://github.com/51Degrees/Device-Detection.git
```
Choose the detection method you wish to use and navigate to the corresponding directory within the 'php' directory.
For Pattern:
```console
$ cd Device-Detection/php/pattern
```
For Trie:
```console
$ cd Device-Detection/php/trie
```
Install by running the following commands:
```console
phpize5
./configure
sudo make install
```
The phpize5 command will prepare the build environment for the PHP extension. The ./configure will check your environment and prepare the makefile. Finally the make install will build the detector module and place it in the PHP extensions directory. The build script should also tell you where the extension has been placed.

## Configure
Before using the extension you must supply the PHP environment with information on the location of the module and the module parameters. This should be done by editing your PHP.ini file.

If you are not sure where the PHP.ini file is located on your system you can create a .php page and place in your server directory. Add the following php code to the page 
```php
<?php
phpinfo();
?>
```
and look for the 'php.ini' file location.

Open the 'php.ini' file and add the following to the bottom. Please note that superuser privileges will most likely be required to edit the file.

The first is the location of the extension binary (note that your location may
be different from the one here. Check  for the location). The second
is the is the path to the 51Degrees data file. Third parameter defines the 
number of WorkSet items in the pool. Make sure that it is in a location that 
Apache has read permissions over.

```ini
extension=/usr/lib/php5/fiftyone_degrees_detector.so
fiftyone_degrees.data_file=path/to/51Degrees/data/file.dat
fiftyone_degrees.number_worksets=20
fiftyone_degrees.cache_size=10000
fiftyone_degrees.property_list=BrowserName,BrowserVendor,BrowserVersion,DeviceType,HardwareVendor,IsTablet,IsMobile,IsCrawler,ScreenInchesDiagonal,ScreenPixelsWidth
```

### Common settings
These settings are valid for both Pattern and Trie.

#### extension
This setting tells the PHP environment where the compiled extension is located.

#### fiftyone_degrees.property_list
This setting should be used with both Pattern and Trie. This is essentially a list of properties you wish the dataset to be initialized with. Premium data file provides over 130 properties and Enterprise over 160 properties. This setting allows you to only choose properties you are interested in. Leaving this setting line empty or not including it at all will return a full list of properties in detection results.

### Pattern settings
These settings should only be used with Pattern flavour of the extension.

#### fiftyone_degrees.data_file
This setting tells the extension where the 51Degrees Pattern data file is located. Pattern data files have the '.dat' file extension. Make sure the file is uncompressed and is accessible by your PHP environment.

#### fiftyone_degrees.number_worksets
This setting tells the detector how many worksets the detector should use. Worksets are used to keep the information associated with the request and detection results. A workset is drawn from the pool when a request commences and is released back to the pool upon request end.

#### fiftyone_degrees.cache_size
This setting tells the detector how many elements should be cached. Cache helps to speed up device detection by keeping a list of the most frequently encountered devices in memory, hence reducing the amount of requests to the device data.

### Trie Settings

#### fiftyone_degrees.data_file
This setting tells the extension where the 51Degrees Trie data file is located. Pattern data files have the '.trie' file extension. Make sure the file is uncompressed and is accessible by your PHP environment.

## Usage

### Detection methods
The extension provides several methods you can use for device detection.

#### fiftyone_match

This method does not require any input as it attempts to locate the '_SERVER' variable and use the relevant HTTP headers such as User-Agent to perform device detection. Returns an array of Property:Value pairs.

```php
<?php
$result = fiftyone_match();
?>
```

#### fiftyone_match_with_useragent

This method expects a User-Agent HTTP header to be supplied as a parameter. A User-Agent header is usually retrieved from the _SERVER['HTTP_USER_AGENT'] variable. Returns an array of Property:Value pairs.

```php
<?php
$result = fiftyone_match_with_useragent($_SERVER['HTTP_USER_AGENT']);
?>
```

#### fiftyone_match_with_headers

This method expects a string of HTTP headers where header name is separated from the header value by either a white space or a colon. In addition each header and value pair must be separated by the new line character '\n'. Returns an array of Property:Value pairs.
```php
<?php
$httpHeaders = "";
//Get all HTTP headers from $_SERVER
foreach ($_SERVER as $key => $value) {
  if (strpos($key, "HTTP_") !== false) {
    $httpHeaders = $httpHeaders.$key." ".$value."\n";
  }
}
$result = fiftyone_match_with_headers($httpHeaders);
?>
```

### Helper methods

#### fiftyone_info

This method returns a PHP array with diagnostics information related to the data file such as :
- Date published
- Next update date
- Type of file (Premium, Lite, Enterprise or Trie).
- Number of device combinations

```php
<?php
$info = fiftyone_info();
?>
```

#### fiftyone_get_http_headers
This method returns an array of HTTP headers that are important for device detection.

```php
<?php
$headers = fiftyone_get_http_headers();
?>
```

### Examples
You can find a PHP page with examples within the Device-Detection/php/examples directory. It demonstrates the output of each of the above function presented in a neat form that lists HTTP headers and provides useful links. To use this page copy it to your Web server public directory and access it through a Web browser.
