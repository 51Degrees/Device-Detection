![51Degrees](https://51degrees.com/DesktopModules/FiftyOne/Distributor/Logo.ashx?utm_source=github&utm_medium=repository&utm_content=readme_main&utm_campaign=varnish-open-source "THE Fastest and Most Accurate Device Detection") **Device Detection in C** Varnish module

[Supported Databases](https://51degrees.com/compare-data-options?utm_source=github&utm_medium=repository&utm_content=compare-data-options&utm_campaign=varnish-open-source "Different device databases which can be used with 51Degrees device detection") | [Developer Documention](https://51degrees.com/support/documentation?utm_source=github&utm_medium=repository&utm_content=documentation&utm_campaign=varnish-open-source "Full getting started guide and advanced developer documentation") | [Available Properties](https://51degrees.com/resources/property-dictionary?utm_source=github&utm_medium=repository&utm_content=property_dictionary&utm_campaign=varnish-open-source "View all available properties and values")

<sup>Need [.NET](https://github.com/51Degrees/.NET-Device-Detection "THE Fastest and most Accurate device detection for .NET") | [Java](https://github.com/51Degrees/Java-Device-Detection "THE Fastest and most Accurate device detection for Java") | [PHP Script](https://github.com/51Degrees/51Degrees-PHP)?</sup>

## Introduction

In a VCL file, use like...
```varnish
import fiftyonedegrees;

sub vcl_deliver {
    # This sets resp.http.X-IsMobile to "True"/"False".
    set resp.http.X-IsMobile = fiftyonedegrees.match_all("IsMobile");
}

sub vcl_init {
    # Initialise the provider with the data file.
	fiftyonedegrees.start("/home/51Degrees/data/51Degrees.dat");
}
```
... to turn User-Agent HTTP headers into useful information about physical screen size, device price and type of device.

Use this project to detect device properties using HTTP browser User-Agents as input using the patented pattern detection method.

Detection method:

Two detection methods are supported.

**Hash:** A small binary data file containing User-Agents stored as Hash sequences. Very fast detection time.

**Pattern:**  Searches for device signatures in a useragent returning metrics about the validity of the results. Does NOT use regular expressions.

All methods use an external data file which can easilly be updated.

## Dependencies
- gcc
- autoconf
- automake
- libvarnishapi-dev
- Varnish source

For Ubuntu based distributions gcc can be found on apt, use
```
$ sudo apt-get install gcc autoconf automake libvarnishapi-dev varnish
```
and the Varnish source will be automatically downloaded by the make file.

## Install

### Linux

#### For an existing Varnish deployment

##### Enhanced Device Data
By default the module will be built with the Lite Pattern data file.

The build scripts will copy data files available in ..\data\ into the varnish directory. Copy your enhanced data file into this location and amend `src/Makefile.am` to use the name of your Premium or Enterprise data file.

##### Static Module
To install the module into an existing Varnish deployment,
first clone 51Degrees/Device-Detection repository with
```
$ git clone https://github.com/51Degrees/Device-Detection.git
```
Move to the Varnish directory with
```
$ cd Device-Detection/varnish
```
and install the module with
```
$ .autogen.sh
$ ./configure --with-detection=pattern/hash VARNISHSRC=DIR [VMODDIR=DIR]
$ make
$ sudo make install

```
Where `--with-detection` [optional] sets the detection method you would prefer to use. Defaults to Pattern.
`VARNISHSRC` [optional] is set to the source for the correct varnish version when installed in a non standard directory and `VMODDIR` is the directory to install the module to.

Then run the included tests with:
```
$ make check
```
which will all pass if the local installation was successful.

## Configure
Before start matching user agents, you may wish to configure the solution to use a different database for example.

### Settings

#### General Settings
These settings should be set before calling `fiftyonedegrees.start`.
 - ``set_cache`` (defaults to ``1000``). Sets the size of the workset cache.
 
 - ``set_pool`` (defaults to ``20``). Sets the size of the workset pool.
 
 - ``set_delimiter`` (defaults to ``','``). Sets the delimiter to separate values with.
 
 - ``set_properties`` (defults to all properties). Sets the properties to initilaise.

## Usage
The easiest way to view full usage information is to run the command:
```
man vmod_fiftyonedegrees
```
to display all the functions with examples.

### The VCL File
An example configuration file is included in this repository. It shows how to add device information to HTTP headers.

#### Initialising the Provider
In the init block is where you should set any settings and initialise the provider.
```
import fiftyonedegrees;

sub vcl_init {
    # Initialise the provider with the data file.
    fiftyonedegrees.start("/home/51Degrees/data/51Degrees.dat");
}
```

#### Matching
Matching can be done is two ways.

##### User-Agent match
To get properties using the device's User-Agent use:
```
sub vcl_deliver {
        # This sets resp.http.X-IsMobile to "True"/"False".
        set resp.http.X-IsMobile = fiftyonedegrees.match_single(req.http.user-agent, "IsMobile");
}
```

##### Multiple HTTP header matches
To get properties from all the relevant HTTP headers from the device use:
```
sub vcl_deliver {
        # This sets resp.http.X-IsMobile to "True"/"False".
        set resp.http.X-IsMobile = fiftyonedegrees.match_all("IsMobile");
}
```

##### Output Format
The value of the header is set to a comma separated list of values (comma delimited is the default behaviour, but the delimiter can be set explicitly with ``set_delimiter``), these are in the same order the properties are listed in the config file. So setting a header with the line:
```
set resp.http.X-Device = fiftyonedegrees.match_all("HardwareName,BrowserName,PlatformName");
```
will give a header named ``X-Device`` with a value like ``Desktop,Firefox,Ubuntu``. Alternatively, headers can be set individually like:
```
set resp.http.X-HardwareName = fiftyonedegrees.match_all("HardwareName");
set resp.http.X-BrowserName = fiftyonedegrees.match_all("BrowserName");
set resp.http.X-PlatformName = fiftyonedegrees.match_all("PlatformName");
```
giving three seperate headers.

### Example
Start Varnish using the example VCL within the Device-Detection Varnish directory with:
```
$ varnishd -f example.vcl -a localhost:8080 -d
```
In a Linux environment, the headers can be viewed with the command:
```
$ curl localhost:99 -I -A [SOME USER-AGENT]
```
which will give the following response:
```
HTTP/1.1 200 OK
...
x-device: Desktop,Firefox,Windows
x-mobile: False
x-tablet: False
x-smartphone: False
x-metrics: 15364-21460-53251-18092,Exact,0,249
```
