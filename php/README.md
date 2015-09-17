The C code can be built into extension for use in PHP. This guide covers how to
build and install the extension on Debian/Ubuntu based systems, although others
should be very similar.

To build, you will need GCC, make, PHP and PHP dev tools.

```console
apt-get install gcc
apt-get install make
apt-get install php5
apt-get install php5-dev
```

Assuming no errors, open Terminal and navigate to directory with PHP C extension
files. The following commands will then build and install the extension.

```console
phpize5
./configure
make install
```

This will build and copy the extension binary into your server's extension
directory.

Before using the extension you must add some new parameters to the php.ini file.
The first is the location of the extension binary (note that your location may
be different from the one here. Check phpinfo() for the location). The second
is the is the path to the 51Degrees data file. Third parameter defines the 
number of WorkSet items in the pool. Make sure that it is in a location that 
Apache has read permissions over.

```ini
extension=/usr/lib/php/fiftyone_degrees_detector.so
fiftyone_degrees.data_file=your/data_file_location.dat
fiftyone_degrees.number_worksets=20
```

This extension can be accessed through the function 'fiftyone_match'. It takes
a single parameter, a user agent string, and returns an associative array
of the matched devices properties and their values in the same format as $_51d.

```php
$properties = fiftyone_match($_SERVER['HTTP_USER_AGENT']);
```

As of V3.2 you can also invoke match by providing multiple HTTP headers to the
match function. Some Web browsers can set extra HTTP headers that provide 
additional information about the requesting device.

```php
$headers;
foreach ($_SERVER as $key => $value) {
  if (strpos($key, "HTTP_") !== false) {
    $headers = $headers.$key." ".$value."\n";
  }
}
$result = fiftyone_match_with_headers($headers);
```

You can print diagnostics information regarding the data file using:

```php
fiftyone_info();
```
