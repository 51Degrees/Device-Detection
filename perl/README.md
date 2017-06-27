![51Degrees](https://51degrees.com/DesktopModules/FiftyOne/Distributor/Logo.ashx?utm_source=github&utm_medium=repository&utm_content=readme_main&utm_campaign=perl-open-source "THE Fasstest and Most Accurate Device Detection") **Device Detection in C** Perl

[Supported Databases](https://51degrees.com/compare-data-options?utm_source=github&utm_medium=repository&utm_content=compare-data-options&utm_campaign=perl-open-source "Different device databases which can be used with 51Degrees device detection") | [Developer Documention](https://51degrees.com/support/documentation?utm_source=github&utm_medium=repository&utm_content=documentation&utm_campaign=perl-open-source "Full getting started guide and advanced developer documentation") | [Available Properties](https://51degrees.com/resources/property-dictionary?utm_source=github&utm_medium=repository&utm_content=property_dictionary&utm_campaign=perl-open-source "View all available properties and values")

<sup>Need [.NET](https://github.com/51Degrees/.NET-Device-Detection "THE Fastest and most Accurate device detection for .NET") | [Java](https://github.com/51Degrees/Java-Device-Detection "THE Fastest and most Accurate device detection for Java") | [PHP Script](https://github.com/51Degrees/51Degrees-PHP)?</sup>

Use PERL code like...

```perl
use FiftyOneDegrees::PatternV3;
my $provider = new FiftyOneDegrees::PatternV3::Provider($filename);
my $match = $provider->getMatch($httpHeaders);
print "IsMobile: " . $match->getValue("IsMobile") . "\r\n";
print "DeviceType: " . $match->getValue("DeviceType") . "\r\n";
```

... to turn User-Agent HTTP headers into useful information about physical screen size, device price and type of device.

**[Review All Properties](https://51degrees.com/resources/property-dictionary?utm_source=github&utm_medium=repository&utm_content=home-cta&utm_campaign=perl-open-source" View all available properties and values")**

## Introduction

Use this project to detect device properties using HTTP browser User-Agents as input. It can be used to process server web log files, or for real time device detection to support web optimisation.

Two detection methods are supported.

**Pattern:** Searches for device signatures in a useragent returning metrics about the validity of the results. Does NOT use regular expressions.

**Trie:** A large binary Trie (pronounced Try) populated with User-Agent signatures. Very fast.

All methods use an external data file which can easilly be updated.

## Dependencies
- make
- perl
- g++
- SWIG

For Ubuntu based distributions these can be found on apt, use
```
$ sudo apt-get update
$ sudo apt-get install make perl g++ swig
```

To use the examples the JSON and a simple web server need to be available. Use

```
$ sudo apt-get install libjson-perl
$ sudo perl -MCPAN -e shell
> install HTTP::Server::Simple::CGI
> install String::Buffer
```

## Install

#### Linux
To install, go to the directory for your chosen detection method (i.e. Pattern or Trie) with
```
$ cd Device-Detection/perl/[package name]
```

Then install with


```
$ perl Makefile.PL
$ sudo make install

```

## Usage

#### Basics
To check everything is set up correctly, try running the following PERL script.

```perl
use FiftyOneDegrees::PatternV3;

my $filename = "path/to/51Degrees-Lite.dat"
my $userAgent = "Mozilla/5.0 (iPad; CPU OS 5_1 like Mac OS X) AppleWebKit/534.46 (KHTML, like Gecko) Mobile/9B176"

my $provider = new FiftyOneDegrees::PatternV3::Provider($filename);
my $match = $provider->getMatch($userAgent);

print "IsMobile: " . $match->getValue("IsMobile") . "\r\n";
print "DeviceType: " . $match->getValue("DeviceType") . "\r\n";
```

#### Examples
In the examples folder, you can find examples of various functionalities that the 51Degrees detector has such as:
- Matching with a User-Agent
- Matching with a device id
- Evaluating match metrics
- Offline processing
- Strongly typed variables
- Finding profiles

A full explanation of these can be found within the files or at [Perl Tutorials](https://51Degrees/support/documentation/perl/tutorials).

Both Pattern and Trie also contain the following examples

**console.pl** which takes no input and returns matches from predefined User-Agent and HTTP headers seperately. Is run with
```
$ perl console.pl
```

**server.pl** which runs a server at localhost:8080 by default and returns matches from the web browsers User-Agent and all relevant HTTP headers seperately. Is run with
```
$ perl server.pl
```
s