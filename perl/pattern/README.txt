FiftyOneDegrees-Pattern Version 1.0.1.4
=======================================

This is a Perl wrapper for 51Degrees Device Detection Pattern library.

It requires a recent Pattern database file from 51Degrees.

Lite Pattern data is included in the data folder. Lite data updates are in the
Pattern Perl Github repository page.
(https://github.com/51Degreesmobi/51Degrees-PatternWrapper-Perl)

For information Premium and Enterprise data and their benefits, see our
website: https://51degrees.com/Products/DeviceDetection

INSTALLATION

To install this module type the following:

   perl Makefile.PL
   make
   make test
   make install

DEPENDENCIES

JSON

API Documentation

For documentation go to https://51degrees.com/Support/Documentation/Perl

BUILDING FROM SOURCE
<<<<<<< HEAD

=======
cd
>>>>>>> V32
This project uses SWIG to create a wrapper around 51Degrees C source code.
To regenerate the wrapper, install SWIG 2.0 or greater, navigate to the
src directory and run the following command:

	swig -perl -outdir ../lib/FiftyOneDegrees 51Degrees.i

This creates the a perl module in lib/FiftyOneDegrees. Run the installation
instructions to test it.

COPYRIGHT AND LICENSE

Code is Copyright 51Degrees 2014.
All code and Lite data is distributed under the Mozilla Public License Version 2.

1.0.1.4

- Fixed problem with VERSION_FROM and ABSTRACT_FROM in Makefile.PL.
- Added JSON as a dependency.
- Fixed a function returning char pointer to a local function array that was
  triggering a compiler warning. This may have led to a segfault.
- Updated 51Degrees C source code.

1.0.0.0

- Initial release.


