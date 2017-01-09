====================
vmod_fiftyonedegrees
====================

------------------------
Varnish 51Degrees Module
------------------------

:Author: Ben Shillito
:Date: 2016-11-30
:Version: 1.0
:Manual section: 3

SYNOPSIS
========

import fiftyonedegrees;

DESCRIPTION
===========

Prototype 51Degrees device detection module for Varnish 4.0.

Implements the 51Degrees match functionality as a VMOD.
Initialise the data set with
fiftyonedegrees.start(datafilepath),
then get a match property from a requesting device with
fiftyonedegrees.match_all(propertyname).

FUNCTIONS
=========

To review functions and usage, use
 man vmod_fiftyonedegrees
after installing.


INSTALLATION
============

The module structure and installation methods are based on the
libvmod-example example.

The quick shortcut way to install is to run:
 ./build
to install the module to an existing Varnish 4.0 service.

Which can the be run on port 99 with the example config with:
 ./run

To view the manual run:
 man vmod_fiftyonedegrees

The source tree is based on autotools to configure the building, and
does also have the necessary bits in place to do functional unit tests
using the varnishtest tool.

Manual install steps::

 .autogen.sh
 ./configure VARNISHSRC=DIR [VMODDIR=DIR]
 make
 sudo make install

In your VCL you could then use this vmod along the following lines::
        
        import fiftyonedegrees;

        sub vcl_deliver {
                # This sets resp.http.X-IsMobile to "True"/"False".
                set resp.http.X-IsMobile = fiftyonedegrees.match_all("IsMobile");
        }

	sub vcl_init {
		# Initialise the provider with the data file.
		fiftyonedegrees.start("/home/51Degrees/data/51Degrees.dat");
	}

TESTING
=======

After installing you can tests the module by running
 make check

This will run all the tests defined in src/tests
