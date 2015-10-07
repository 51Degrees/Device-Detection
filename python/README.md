![51Degrees](https://51degrees.com/DesktopModules/FiftyOne/Distributor/Logo.ashx?utm_source=github&utm_medium=repository&utm_content=readme_main&utm_campaign=python-open-source "THE Fasstest and Most Accurate Device Detection") **Device Detection in C** Python wrapper

[Supported Databases](https://51degrees.com/compare-data-options?utm_source=github&utm_medium=repository&utm_content=compare-data-options&utm_campaign=python-open-source "Different device databases which can be used with 51Degrees device detection") | [Developer Documention](https://51degrees.com/support/documentation?utm_source=github&utm_medium=repository&utm_content=home-menu&utm_campaign=python-open-source "Full getting started guide and advanced developer documentation") | [Available Properties](https://51degrees.com/resources/property-dictionary?utm_source=github&utm_medium=repository&utm_content=property_dictionary&utm_campaign=python-open-source "View all available properties and values")

<sup>Need [.NET](https://github.com/51Degrees/.NET-Device-Detection "THE Fastest and most Accurate device detection for .NET") | [Java](https://github.com/51Degrees/Java-Device-Detection "THE Fastest and most Accurate device detection for Java") | [PHP Script](https://github.com/51Degrees/51Degrees-PHP)?</sup>

## Introduction

In Python, use like...
```python
>>> from FiftyOneDegrees import fiftyone_degrees_mobile_detector_v3_wrapper
>>> from fiftyone_degrees.mobile_detector.conf import settings
>>> provider = fiftyone_degrees_mobile_detector_v3_wrapper.Provider(settings.V3_WRAPPER_DATABASE,settings.PROPERTIES,settings.CACHE_SIZE, settings.POOL_SIZE)
>>> device = provider.getMatch(httpHeaders)
>>> print device.getValue('IsMobile')
>>> print device.getValue('DeviceType')
```
... to turn User-Agent HTTP headers into useful information about physical screen size, device price and type of device.

Use this project to detect device properties using HTTP browser User-Agents as input. It can be used to process server web log files, or for real time device detection to support web optimisation.

Two detection methods are supported.

**Pattern:**  Searches for device signatures in a useragent returning metrics about the validity of the results. Does NOT use regular expressions.

**Trie:** A large binary Trie (pronounced Try) populated with User-Agent signatures. Very fast.

All methods use an external data file which can easilly be updated.

## Dependencies
#### Essential

- python
- python-setuptools
- python-dev
- python-pip
- g++

For Ubuntu based distributions these can be found on apt, use
```
$ sudo apt-get install python python-setuptools python-dev python-pip g++
```

If building from this git repository you will also need
- make
- swig

For Ubuntu based distributions these can be found on apt, use
```
$ sudo apt-get install make swig
```

#### Optional
To use the Django example you will need
- django

For Ubuntu based distributions these can be found on apt, use
```
$ sudo pip install django
```
## Install
### Linux
#### From pip
This is the quickest and easiest way to install 51Degrees python detector. Just use
```
$ sudo pip install 51degrees-mobile-detector-v3-wrapper
```
or
```
$ sudo pip install 51degrees-mobile-detector-v3-trie-wrapper
```
*Note: 51degrees-mobile-detector will be installed as a dependency.*
#### From git
However, if you want to build the package yourself and use the examples too.
First clone 51Degrees/Device-Detection repository with
```
$ git clone https://github.com/51Degrees/Device-Detection.git
```
Move to the Core directory with
```
$ cd Device-Detection/python/core
```
and install with
```
$ sudo make install
```
Now move to the directory for you chosen detection method (i.e. Pattern or Trie) with
```
$ cd ../[package name]
```
Then install with

```
$ sudo make install
```

When you're done installing, check it's set up correctly by recreating the output in the Usage section below. Or if you've cloned this repository, try the examples.
## Configure

Before start matching user agents, you may wish to configure the solution to use a different datadase for example. You can easily generate a sample settings file running the following command
```
$ 51degrees-mobile-detector settings > ~/51degrees-mobile-detector.settings.py
```

Edit the generated settings and set your preferences. Ensure you set ``DETECTION_METHOD`` and ``V3_WRAPPER_DATABASE``, or``TRIE_V3_WRAPPER_DATABASE`` (if using the trie-based detection method).

Finally, link your settings file from the ``FIFTYONE_DEGREES_MOBILE_DETECTOR_SETTINGS`` environment variable
```
$ export FIFTYONE_DEGREES_MOBILE_DETECTOR_SETTINGS=~/51degrees-mobile-detector.settings.py
```

This can be permanently set by adding this line to your ``~/.profile`` file.

### Settings
#### General Settings
 - ``DETECTION_METHOD`` (defaults to ``'v3-wrapper'``). Sets the preferred mobile device detection method. Available options are ``v3-wrapper`` (requires 51degrees-mobile-detector-v3-wrapper package), ``v3-trie-wrapper`` (requires 51degrees-mobile-detector-v3-trie-wrapper package).

 - ``PROPERTIES`` (defaults to ``''``). List of case-sensitive property names to be fetched on every device detection. Leave empty to fetch all available properties.

 - ``LICENSE``. Your 51Degrees license key. This is required if you want to set up the automatic 51degrees-mobile-detector-premium-pattern-wrapper package updates.

#### Trie Detector Settings
 - ``V3_TRIE_WRAPPER_DATABASE``. Location of the Trie database file.

#### Pattern Detector Settings
 - ``V3_WRAPPER_DATABASE``. Location of the Pattern database file.

 - ``CACHE_SIZE`` (defaults to ``10000``). Sets the size of the workset cache.

 - ``POOL_SIZE`` (defaults to ``20``). Sets the size of the workset pool.

#### Usage Sharer Settings
 - ``USAGE_SHARER_ENABLED`` (defaults to ``True``). Indicates if usage data should be shared with 51Degrees.com. We recommended leaving this value unchanged to ensure we're improving the performance and accuracy of the solution.

 - Adavanced usage sharer settings are detailed in your settings file.

#### Automatic updates
If you want to set up automatic updates, add your license key to your settings and add the ``51degrees-mobile-detector update-premium-pattern-wrapper`` command to your cron.

## Usage
#### Core
By executing the following a useful help page will be displayed explaing basic usage.
```
$ 51degrees-mobile-detector
```
To check everything is set up , try fetching a match with
```
$ 51degrees-mobile-detector match "Mozilla/5.0 (iPad; CPU OS 5_1 like Mac OS X) AppleWebKit/534.46 (KHTML, like Gecko) Mobile/9B176"
```

Alternatively, open a Python console, load the module and start matching
```python
>>> from fiftyone_degrees import mobile_detector

>>> device = mobile_detector.match("Mozilla/5.0 (iPad; CPU OS 5_1 like Mac OS X) AppleWebKit/534.46 (KHTML, like Gecko) Mobile/9B176")

>>> device.Id
'15767-18117-17776-18092'

>>> device.BrowserName
'MobileSafari'

>>> device.ScreenPixelsWidth
'1024'

>>> device.method
'v3-wrapper'
```

#### Wrapper
To use your chosen wrapper without going through the core, open a Python console, load the module and start matching
```python
>>> from FiftyOneDegrees import fiftyone_degrees_mobile_detector_v3_wrapper

>>> from fiftyone_degrees.mobile_detector.conf import settings

>>> provider = fiftyone_degrees_mobile_detector_v3_wrapper.Provider(settings.V3_WRAPPER_DATABASE,settings.PROPERTIES,settings.CACHE_SIZE, settings.POOL_SIZE)

>>> device = provider.getMatch("Mozilla/5.0 (iPad; CPU OS 5_1 like Mac OS X) AppleWebKit/534.46 (KHTML, like Gecko) Mobile/9B176")

>>> device.getDeviceId()
'15767-18117-17776-18092'

>>> device.getValue('BrowserName')
'Mobile Safari'

>>> device.getValue('ScreenPixelsWidth')
'1024'

```

#### Examples
Both Pattern and Trie packages include the follow examples

*Note: If they are not already, be sure to mark these as executable first with*
```
$ chmod +x console.py server.py
```

**console.py** which takes no input and returns matches from predefined User-Agent and HTTP headers seperately. Is run with
```
$ ./console.py
```

**server.py** which runs a server at localhost:8080 by default and returns matches from the web browsers User-Agent and all relevant HTTP headers seperately. Is run with
```
$ ./server.py
```

#### Django
The core installation includes a Django middleware. A primative example can be found in core/django-example. To run use
```
$ python manage.py runserver
```
