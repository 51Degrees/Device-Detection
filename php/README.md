![51Degrees](https://51degrees.com/DesktopModules/FiftyOne/Distributor/Logo.ashx?utm_source=Github&utm_medium=repository&utm_content=readme_main&utm_campaign=php-open-source "THE Fastest and Most Accurate Device Detection") **Device Detection in C** PHP C extension

[Supported Databases](https://51degrees.com/compare-data-options?utm_source=Github&utm_medium=repository&utm_content=compare-data-options&utm_campaign=php-open-source "Different device databases which can be used with 51Degrees device detection") | [Developer Documention](https://51degrees.com/support/documentation?utm_source=github&utm_medium=repository&utm_content=documentation&utm_campaign=php-open-source "Full getting started guide and advanced developer documentation") | [Available Properties](https://51degrees.com/resources/property-dictionary?utm_source=Github&utm_medium=repository&utm_content=property_dictionary&utm_campaign=php-open-source "View all available properties and values")

<sup>Need [.NET](https://github.com/51Degrees/.NET-Device-Detection "THE Fastest and most Accurate device detection for .NET") | [Java](https://github.com/51Degrees/Java-Device-Detection "THE Fastest and most Accurate device detection for Java") | [PHP Script](https://github.com/51Degrees/51Degrees-PHP)?</sup>

## Introduction

In PHP, use like...

```php
<?php
$match = $provider->getMatch($_SERVER['HTTP_USER_AGENT']);

// Access individual properties.
$isMobile = $match->getValue('IsMobile');
if ($isMobile === "True") {
	// Code for mobile device.
} else {
	// Code for Desktop.
}
echo "<p>You are using ".$match->getValue('BrowserName')." browser of version".$match->getValue('BrowserVersion').".</p>";
echo "<p>The type of your device is: ".$match->getValue('DeviceType').".</p>";
?>
```

Use this project to detect device properties using HTTP browser User-Agents as input. It can be used to process server web log files, or for real time device detection to support web optimisation.

Two detection methods are supported.

**Pattern:**  Searches for device signatures in a User-Agent returning metrics about the validity of the results. Does NOT use regular expressions.

**Trie:** A large binary Trie (pronounced Try) populated with User-Agent signatures. Very fast.

All methods use an external data file which can easilly be updated.

## Breaking Changes

The PHP API is now built from the same source as every other C based API. This means new features are introduced as soon as they are developed. It can still be used in nearly the same way but some of the namings and syntax has changed slightly. This readme will explain the usage fully.

## Dependencies

- g++
- make
- php5
- php5-dev
- git
- SWIG 2.0

For Ubuntu based distributions these can be found on apt, use
```
$ sudo apt-get install g++ make php5 php5-dev git swig2.0
```

## Install
<installation>
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

###Windows
First, clone 51Degrees/Device-Detection repository using git:
```console
$ git clone https://github.com/51Degrees/Device-Detection.git
```
Now download the php source (the same version as is installed on the target system) from http://windows.php.net/download and extract to either the Pattern or Trie directory and rename to php-src.

Download the necessary binary tools from http://windows.php.net/downloads/php-sdk/ and extract the bin subfolder to either the Pattern or Trie directory.
In a developer command prompt (for PHP 5.5/5.6 use Visual Studio 2012, for 7.0+ use 2015) and move to either the Pattern or Trie directory.
make the binary tools available in the system path:
```
$ bin\phpsdk_setvars.bat
```
Configure and build the php source by running the following in the php-src directory:
```
$ cd php-sdk
$ buildconf
$ configure --disable-all --enable-cli
$ nmake
```
The VisualStudio directory contains a project which can now be opened and built. This will then build the dll in the VisualStudio\Release directory.

Just include the dll in a suitable PHP directory and configure as below.
</installation>
## Configure
<configuration>
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
extension=/usr/lib/php5/FiftyOneDegreesPatternV3.so
FiftyOneDegreesPatternV3.data_file=path/to/51Degrees/data/file.dat
FiftyOneDegreesPatternV3.property_list=BrowserName,BrowserVendor,BrowserVersion,DeviceType,HardwareVendor,IsTablet,IsMobile,IsCrawler,ScreenInchesDiagonal,ScreenPixelsWidth
FiftyOneDegreesPatternV3.cache_size=10000
FiftyOneDegreesPatternV3.pool_size=20
```

In both Pattern and Trie directories, there is a php file which contains all the classes needed. So put this in a directory accessible by your web server, e.g. /var/www/html/51Degrees. This can then be included at the top of any php script using the detector with
```php5
require(path/to/FiftyOneDegreesPatternV3.php);
```
or
```php5
require(path/to/FiftyOneDegreesTrieV3.php);
```
### Common Settings
#### extension
This setting tells the PHP environment where the compiled extension is located.



### Pattern settings
These settings should only be used with Pattern flavour of the extension.

#### FiftyOneDegreesPatternV3.data_file
This setting tells the extension where the 51Degrees Pattern data file is located. Pattern data files have the '.dat' file extension. Make sure the file is uncompressed and is accessible by your PHP environment.

#### FiftyOneDegreesPatternV3.property_list
This is essentially a list of properties you wish the dataset to be initialized with. Premium data file provides over 130 properties and Enterprise over 160 properties. This setting allows you to only choose properties you are interested in. Leaving this setting line empty or not including it at all will return a full list of properties in detection results.

#### FiftyOneDegreesPatternV3.cache_size
This setting tells the detector how many elements should be cached. Cache helps to speed up device detection by keeping a list of the most frequently encountered devices in memory, hence reducing the amount of requests to the device data.

#### FiftyOneDegreesPatternV3.pool_size
This setting tells the detector how many worksets the detector should use. Worksets are used to keep the information associated with the request and detection results. A workset is drawn from the pool when a request commences and is released back to the pool upon request end.


### Trie Settings

#### FiftyOneDegreesTrieV3.data_file
This setting tells the extension where the 51Degrees Trie data file is located. Pattern data files have the '.trie' file extension. Make sure the file is uncompressed and is accessible by your PHP environment.

#### FiftyOneDegreesTrieV3.property_list
This is essentially a list of properties you wish the dataset to be initialized with. Premium data file provides over 130 properties and Enterprise over 160 properties. This setting allows you to only choose properties you are interested in. Leaving this setting line empty or not including it at all will return a full list of properties in detection results.
</configuration>
## Usage

To use the 51Degrees extension, start by fetching the provider that will have been initialised on server startup. Then this object can be called to process a new match object which can return properties of the specific matched device. This can be seen in the code below:
```php5
<?php
require("FiftyOneDegreesPatternV3");
$provider = FiftyOneDegreesPatternV3::provider_get();
$match = $provider->getMatch($_SERVER['HTTP_USER_AGENT']);
echo $match->getValue('IsMobile');
?>
```
### Examples
In the examples folder, you can find examples of various functionalities that the 51Degrees detector has such as:
- Matching with a User-Agent
- Matching with a device id
- Evaluating match metrics
- Offline processing
- Strongly typed variables
- Finding profiles

A full explanation of these can be found within the files or at [PHP Tutorials](https://51Degrees/support/documentation/php/tutorials).

You can find an example PHP page within the Device-Detection/php/examples directory called example.php. It demonstrates the output of each of the above function presented in a neat form that lists HTTP headers and provides useful links.

To use these examples, first make sure that all settings (see [Configuration](#configure) for Pattern/Trie settings) are set in `/etc/php5/cli/php.ini` and the ``enable_dl`` line reads ``enable_dl=On``. Then run
```
php -S localhost:8080
```
in the eamples directory to run start a server, and all examples will be available in a browser at, for example, `localhost:8080/example.php`. Or any of the other files in that directory.