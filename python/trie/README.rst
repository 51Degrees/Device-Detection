FiftyOneDegrees-Pattern for Python Version 3.1.1.1
==================================================

This is a Python wrapper for 51Degrees Device Detection library.

It requires a recent Pattern database file from 51Degrees.

Lite Pattern data is included in the data folder. Lite data updates are in the
Pattern Python Github repository page.
(https://github.com/51Degreesmobi/51Degrees-PatternWrapper-Python)

For information Premium and Enterprise Pattern data and their benefits, see our
website: https://51degrees.com/Products/DeviceDetection

INSTALLATION

This package is available on pip:

  pip install 51degrees-mobile-detector-v3-wrapper

DEPENDENCIES

- 51Degrees-Core-Python. This can be found on pip
  (pip install 51degrees-mobile-detector) and on GitHub:
  https://github.com/51Degreesmobi/51Degrees-Core-Python
  
API Documentation

For documentation go to https://51degrees.com/Support/Documentation/Python

BUILDING FROM SOURCE

To build from source navigate to the directory and run:

  python setup.py sdist

This will create a dist directory containing a pip package. To install, navigate
to it and run:

  pip install 51degrees-mobile-detector-v3-wrapper-3.1.tar.gz --upgrade

COPYRIGHT AND LICENSE

Code is Copyright 51Degrees 2014.
All code and Lite data is distributed under the Mozilla Public License Version 2.

CHANGE LOG

3.1.1.1

- Added more error messages to diagnose problems with data initialisation.

3.1.0.0

- Initial release.
