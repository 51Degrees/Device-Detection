PHP Extension ReadMe
--------------------

The C code can be built into extension for use in PHP. This guide covers how to
build and install the extension on Debian/Ubuntu based systems, although others
should be very simliar.

To build, you will need GCC, make, PHP and PHP dev tools.

# apt-get install gcc
# apt-get install make
# apt-get install php5
# apt-get install php5-dev

Assuming no errors, you will then need to extract the contents of the C archive
and navigate to php subdirectory in a terminal.

The following commands will then build and install the extension:

# phpize5
# ./configure
# make install

This will build and copy the extension binary into your server's extension
directory.

Before using the extension you must add some new parameters to the php.ini file.
The first is the location of the extension binary (note that your location may
be different from the one here. Check phpinfo() for the location). The second
is the is the path to the 51Degrees data file. Make sure that it is in a
location that Apache has read permissions over.

extension=/usr/lib/php/fiftyone_degrees_detector.so
fiftyone_degrees.data_file=your/data_file_location.dat

This extension can be accessed through the function 'get_properties'. It takes
a single parameter, a useragent string, and returns an associative array
of the matched devices properties and their values in the same format as $_51d.

$properties = get_properties($_SERVER['HTTP_USER_AGENT']);
