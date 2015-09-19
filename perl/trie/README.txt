FiftyOneDegrees-Trie Version 1.0.0.0
====================================

This is a Perl wrapper for 51Degrees Device Detection Trie library.

It requires a recent Trie database file from 51Degrees.

Lite Trie data is included in the data folder. Lite data updates are in the
Trie Perl Github repository page.
(https://github.com/51Degreesmobi/51Degrees-TrieWrapper-Perl)

For information Premium and Enterprise Trie data and their benefits, see our
website: https://51degrees.com/Products/DeviceDetection

INSTALLATION

To install this module run the following commands in the project directory:

   perl Makefile.PL
   make
   make test
   make install

DEPENDENCIES

JSON

API Documentation

For documentation go to https://51degrees.com/Support/Documentation/Perl

BUILDING FROM SOURCE

This project uses SWIG to create a wrapper around 51Degrees C source code.
To regenerate the wrapper, install SWIG 2.0 or greater, navigate to the
trie directory and run the following command:

	swig -perl -outdir ../src/lib/FiftyOneDegrees 51Degrees.i

This creates the a perl module in lib/FiftyOneDegrees. Run the installation
instructions to test it.

COPYRIGHT AND LICENSE

Code is Copyright 51Degrees 2014.
All code and Lite data is distributed under the Mozilla Public License Version 2.

CHANGE LOG

1.0.1.1

- Updated 51degrees-C Trie code.

1.0.0.0

- Initial release.
