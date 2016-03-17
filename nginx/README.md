![51Degrees](https://51degrees.com/DesktopModules/FiftyOne/Distributor/Logo.ashx?utm_source=github&utm_medium=repository&utm_content=readme_main&utm_campaign=nginx-open-source "THE Fastest and Most Accurate Device Detection") **Device Detection in C** Nginx module

[Supported Databases](https://51degrees.com/compare-data-options?utm_source=github&utm_medium=repository&utm_content=compare-data-options&utm_campaign=nginx-open-source "Different device databases which can be used with 51Degrees device detection") | [Developer Documention](https://51degrees.com/support/documentation?utm_source=github&utm_medium=repository&utm_content=documentation&utm_campaign=nginx-open-source "Full getting started guide and advanced developer documentation") | [Available Properties](https://51degrees.com/resources/property-dictionary?utm_source=github&utm_medium=repository&utm_content=property_dictionary&utm_campaign=nginx-open-source "View all available properties and values")

<sup>Need [.NET](https://github.com/51Degrees/.NET-Device-Detection "THE Fastest and most Accurate device detection for .NET") | [Java](https://github.com/51Degrees/Java-Device-Detection "THE Fastest and most Accurate device detection for Java") | [PHP Script](https://github.com/51Degrees/51Degrees-PHP)?</sup>

## Introduction

In nginx.config, use like...
```nginx
http {
  51D_filePath path/to/51Degrees.dat;
  51D_cache 10000;
  51D_pool 20;
  server {
    ...
    location /location/to/match/with/User-Agent/ {
      51D_single IsMobile,DeviceType;
      ...
    }
    location /location/to/match/with/multiple/http/headers/ {
      51D_multi IsMobile,DeviceType;
      ...
    }
  }
}
```
... to turn User-Agent HTTP headers into useful information about physical screen size, device price and type of device.

Use this project to detect device properties using HTTP browser User-Agents as input using the patented pattern detection method.

**Pattern:**  Searches for device signatures in a User-Agent returning metrics about the validity of the results. Does NOT use regular expressions. Uses an external data file which can easily be updated.

## Dependencies
- gcc
- Nginx source

For Ubuntu based distributions gcc can be found on apt, use
```
$ sudo apt-get install gcc
```
and the Nginx source will be automatically downloaded by the ``bootstrap`` script.

## Install
<installation>
### Linux
#### Test Installation
This is the quickest and easiest way to build Nginx in a local directory to try out the module. Just use
```
$ git clone https://github.com/51Degrees/Device-Detection.git
$ cd Device-Detection/nginx
$ ./install
```
To install to the build subdirectory.
#### For an existing Nginx deployment
To compile the module into an existing Nginx deployment,
first clone 51Degrees/Device-Detection repository with
```
$ git clone https://github.com/51Degrees/Device-Detection.git
```
Move to the Nginx directory with
```
$ cd Device-Detection/nginx
```
and put together the module directory with
```
$ ./bootstrap
```
Copy the module to the Nginx modules directory with
```
$ cp -r 51Degrees_module [MODULE DIRECTORY]
```
In the Nginx source directory, run ``./configure`` as normal and add the module, the linker option ``-lm`` and the definition ``FIFTYONEDEGREES_NGINX``. A basic example is,
```
$ CFLAGS="-DFIFTYONEDEGREES_NGINX" ./configure \
    --prefix=[NGINX INSTALL DIRECTORY] \
    --with-ld-opt="-lm" \
    --add-module=[MODULE DIRECTORY]/51Degrees_module
```
Then install with
```
$ sudo make install
```
</installation>
When you're done installing, check it's set up correctly by recreating the output in the Usage section below.
## Configure
<configuration>
Before start matching user agents, you may wish to configure the solution to use a different database for example.
### Settings
#### General Settings
These settings are valid in the main configuration block and should only be set once.
 - ``51D_filePath`` (defaults to ``'51Degrees.dat'``). Sets the location of the data file.

 - ``51D_cache`` (defaults to ``10000``). Sets the size of the workset cache.

 - ``51D_pool`` (defaults to ``20``). Sets the size of the workset pool.

#### Location Settings
These settings are valid in a location configuration block and should only be set once per location.
 - ``51D_single`` (defaults to disabled). Gets device properties using a User-Agent. Takes a comma separated list of properties to return.

- ``51D_multi`` (defaults to disabled). Gets device properties using multiple HTTP headers. Takes a comma separated list of properties to return.

## Usage
### The Config File
An example configuration file is included in this repository. It shows how to pass device information when using Nginx as a reverse proxy, and when passing to a fast-cgi provider.
#### HTTP Block
Within the HTTP block is where the detector settings are set, these should be set like
```
http {
  51D_filePath data/51Degrees.dat;
  51D_cache 10000;
  51D_pool 20;
  ...
}
```
#### Location Block
Within a location block is where the match settings are set. They can be set in one of two ways:
##### User-Agent match
To get properties using the device's User-Agent use:
```
location / {
  51D_single IsMobile,DeviceType,BrowserName;
  ...
}
```
##### Multiple HTTP header matches
To get properties from all the relevant HTTP headers from the device use:
```
location / {
  51D_multi IsMobile,DeviceType,BrowserName;
  ...
}
```
##### Proxy Passing
When using the ``proxy_pass`` directive in a location block where a match directive is used, the properties selected are passed as additional HTTP headers in the format ``51D-IsMobile``.
##### Fast-CGI
Using ``include fastcgi_params;`` makes these additional headers available via the ``$_SERVER`` variable in the format ``$_SERVER[HTTP_51D_ISMOBILE]``.
### Example
If installing to a local directory using ``./install``, the executable is set up to use the example configuration and can be easily tested. Take ``example.php``, which just print all request headers, and place it in apache's web directory (probably /var/www/html).
Now accessing ``localhost:8888/example.php`` will display all request headers which will include the device properties:
```
51D-IsMobile: False
51D-IsTablet: False
51D-BrowserVendor: Mozilla
51D-PlatformName: Ubuntu
```
