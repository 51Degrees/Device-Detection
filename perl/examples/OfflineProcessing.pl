#!/usr/bin/env perl
=copyright

This Source Code Form is copyright of 51Degrees Mobile Experts Limited.
Copyright (c) 2015 51Degrees Mobile Experts Limited, 5 Charlotte Close,
Caversham, Reading, Berkshire, United Kingdom RG4 7BY

This Source Code Form is the subject of the following patent
applications, owned by 51Degrees Mobile Experts Limited of 5 Charlotte
Close, Caversham, Reading, Berkshire, United Kingdom RG4 7BY:
European Patent Application No. 13192291.6; and
United States Patent Application Nos. 14/085,223 and 14/085,301.

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0.

If a copy of the MPL was not distributed with this file, You can obtain
one at http://mozilla.org/MPL/2.0/.

This Source Code Form is "Incompatible With Secondary Licenses", as
defined by the Mozilla Public License, v. 2.0.
=cut

=tutorial
<tutorial>

Offline processing example of using 51Degrees device detection. The example
shows how to:
<ol>
<li>Set the various settings for 51Degrees detector
<p><code>
my $filename = "51Degrees-LiteV3.2.dat";<br>
my $propertyList = "IsMobile"<br>
my $cacheSize = 10000;<br>
my $poolSize = 20;
</code></p>
<li>Instantiate the 51Degrees device detection provider with these
properties
<p><code>
my $provider = new FiftyOneDegrees::PatternV3::Provider(
	$dataFile,
	$properties,
	$cacheSize,
	$poolSize);
</code></p>
<li>Open an input file with a list of User-Agents, and an output file,
<p><code>
open my $file_in, "../../data/20000 User Agents.csv";<br>
open file_out, ">output.csv";
</code></p>
<li>Write a header to the output file with the property names in '|'
separated CSV format ('|' sepparated because some User-Agents contain
commas)
<p><code>
print file_out "User-Agent";<br>
foreach $property (@$properties) {<br>
	print file_out "|".$property;<br>
}<br>
print file_out "\n";<br>
</code></p>
<li>For the first 20 User-Agents in the input file, performa match then
write the User-Agent along with the values for chosen properties to
the CSV.
<p><code>
while( my $userAgent = <$file_in>) {<br>
	chomp $userAgent;<br>
	my $match = $provider->getMatch($userAgent);<br>
	print file_out $userAgent;<br>
	foreach $property (@$properties) {<br>
		print file_out "|".$match->getValue($property);<br>
	}<br>
	print file_out "\n";<br>
	last if $. == 20;<br>
}<br>
</code></p>
</ol>
This example assumes you are running from the original subdirectory
i.e. Device-Detection/perl/examples/ and the 51Degrees Perl module
is installed. 
</tutorial>
=cut
# // Snippet Start
use FiftyOneDegrees::PatternV3;
use feature qw/say/;

my $filename = "../../data/51Degrees-LiteV3.2.dat";
my $propertyList = "IsMobile,PlatformName,PlatformVersion";
my $cacheSize = 10000;
my $poolSize = 20;

my $inputFile = "../../data/20000 User Agents.csv";
my $outputFile = "offlineProcessingOutput.csv";

# Initialises the device detection provider with the settings declared above.
# This uses Lite data file. For more info
# see:
# <a href="https://51degrees.com/compare-data-options">compare data options
# </a>

my $provider = new FiftyOneDegrees::PatternV3::Provider(
	$filename,
	$propertyList,
	$cacheSize,
	$poolSize);
# Fetches an array of available properties from the provider.
my $properties = $provider->getAvailableProperties();

say "Starting Offline Processing.";

# Opens input and output files, writes a header to the output file containing
# the column titles. Then reads the first 20 User-Agents from the input
# file, runs a match for each and writes results to the output file.

open my $file_in, $inputFile;
open file_out, ">$outputFile";

print file_out "User-Agent";
foreach $property (@$properties) {
	print file_out "|".$property;
}
print file_out "\n";

while( my $userAgent = <$file_in>) {
	chomp $userAgent;
	my $match = $provider->getMatch($userAgent);
	print file_out $userAgent;
	foreach $property (@$properties) {
		print file_out "|".$match->getValue($property);
	}
	print file_out "\n";
	last if $. == 20;
}
close $file_in;
close file_out;

say "Output Written to $outputFile";
# // Snippet End
