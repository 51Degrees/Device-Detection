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
fiftyonedegrees.match(propertyname).

FUNCTIONS
=========

start
-----

Prototype
        ::

                start(STRING DS)
Return value
	VOID
Description
	Initialises the 51Degrees provider with the data set DS.
Example
        ::

                fiftyonedegrees.start("/path/to/51Degrees.dat");

match
-----

Prototype
        ::

                match(STRING P)
Return value
	STRING
Description
	Returns the value of property P for the requesting device.
Example
        ::

                set req.http.X-IsMobile = fiftyonedegrees.match("IsMobile");

INSTALLATION
============

The module structure and installation methods are based on the
libvmod-example example.

The source tree is based on autotools to configure the building, and
does also have the necessary bits in place to do functional unit tests
using the varnishtest tool.

Usage::

 ./configure VARNISHSRC=DIR [VMODDIR=DIR]

`VARNISHSRC` is the directory of the Varnish source tree for which to
compile your vmod. Both the `VARNISHSRC` and `VARNISHSRC/include`
will be added to the include search paths for your module.

Optionally you can also set the vmod install directory by adding
`VMODDIR=DIR` (defaults to the pkg-config discovered directory from your
Varnish installation).

Make targets:

* make - builds the vmod
* make install - installs your vmod in `VMODDIR`

In your VCL you could then use this vmod along the following lines::
        
        import fiftyonedegrees;

        sub vcl_deliver {
                # This sets resp.http.X-IsMobile to "True"/"False".
                set resp.http.X-IsMobile = fiftyonedegrees.match("IsMobile");
        }

	sub vcl_init {
		# Initialise the provider with the data file.
		fiftyonedegrees.start("/home/51Degrees/data/51Degrees.dat");
	}
