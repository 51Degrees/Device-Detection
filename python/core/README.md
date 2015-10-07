![51Degrees](https://51degrees.com/DesktopModules/FiftyOne/Distributor/Logo.ashx?utm_source=github&utm_medium=repository&utm_content=readme_core&utm_campaign=python-open-source "THE Fasstest and Most Accurate Device Detection") **Device Detection in C** Python wrapper

[Supported Databases](https://51degrees.com/compare-data-options?utm_source=github&utm_medium=repository&utm_content=compare-data-options&utm_campaign=python-open-source "Different device databases which can be used with 51Degrees device detection") | [Developer Documention](https://51degrees.com/support/documentation?utm_source=github&utm_medium=repository&utm_content=home-menu&utm_campaign=python-open-source "Full getting started guide and advanced developer documentation") | [Available Properties](https://51degrees.com/resources/property-dictionary?utm_source=github&utm_medium=repository&utm_content=property_dictionary&utm_campaign=python-open-source "View all available properties and values")

<sup>Need [.NET](https://github.com/51Degrees/.NET-Device-Detection "THE Fastest and most Accurate device detection for .NET") | [Java](https://github.com/51Degrees/Java-Device-Detection "THE Fastest and most Accurate device detection for Java") | [PHP Script](https://github.com/51Degrees/51Degrees-PHP)?</sup>

# Included files

File Name | Description
------------ | -------------
Makefile | makes the distribution using setup.py, installs and cleans.
setupt.py | makes python package using setuptools.
MANIFEST.in | tells setup.py which files to include in the package.
examples/console.py | example console app. Returns properties from set User-Agent and http headers.
examples/server.py | example server app. Returns properties from browser User-Agent and http headers.
fiftyone_degrees/\__init\__.py | placeholder, tells setup.py this folder is used for the python module.
fiftyone_degrees/\__init\__.py | contains wrapper classes/functions, calls chosen detector.
fiftyone_degrees/runner.py | contains main module, interfaces with user and calls detector through \__init\__.py.
fiftyone_degrees/usage.py | shares detection data with 51Degrees to improve accuracy of detections.
fiftyone_degrees/conf/default.py | default settings for mobile detector.
fiftyone_degrees/conf/\__init__.py | used to load settings from default.py.
fiftyone_degrees/contrib/\__init__.py | placeholder, tells setup.py this contains python modules.
fiftyone_degrees/contrib/django/\__init\__.py | init file for Django middleware.
fiftyone_degrees/contrib/django/middleware.py | Django middleware, used to run mobile detector for django users.
fiftyone_degrees/contrib/django/context_processors.py | used to send requset to Django middleware.
django-example/manage.py | Django server example. See main README.
django-example/example/\__init\__.py | init file for Django server example.
django-example/example/settings.py | Django settings, adds middleware. See Django documentation.
django-example/example/urls.py | Django urls file. See Django documentation.
django-example/example/views.py | handles server request. See Django documentation.
django-example/example/wsgi.py | loads Django apps. See Django documentation
django-example/example/templates/base.html | html file which build the basic web page.
README.md | this readme file.
LICENCE.txt | a copy of the licence.
