#!/usr/bin/env perl
=copyright
This Source Code Form is copyright of 51Degrees Mobile Experts Limited.
Copyright 2017 51Degrees Mobile Experts Limited, 5 Charlotte Close,
Caversham, Reading, Berkshire, United Kingdom RG4 7BY

This Source Code Form is the subject of the following patents and patent
applications, owned by 51Degrees Mobile Experts Limited of 5 Charlotte
Close, Caversham, Reading, Berkshire, United Kingdom RG4 7BY:
European Patent No. 2871816;
European Patent Application No. 17184134.9;
United States Patent Nos. 9,332,086 and 9,350,823; and
United States Patent Application No. 15/686,066.

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
<p><pre class="prettyprint lang-pl">
my $filename = "51Degrees-LiteV3.2.dat";
my $propertyList = "IsMobile"
my $cacheSize = 10000;
my $poolSize = 20;
</pre></p>
<li>Instantiate the 51Degrees device detection provider with these
properties
<p><pre class="prettyprint lang-pl">
my $provider = new FiftyOneDegrees::PatternV3::Provider(
	$dataFile,
	$properties,
	$cacheSize,
	$poolSize);
</pre></p>
<li>Open an input file with a list of User-Agents, and an output file,
<p><pre class="prettyprint lang-pl">
open my $file_in, "../../data/20000 User Agents.csv";
open file_out, ">output.csv";
</pre></p>
<li>Write a header to the output file with the property names in '|'
separated CSV format ('|' separated because some User-Agents contain
commas)
<p><pre class="prettyprint lang-pl">
print file_out "User-Agent";
foreach $property (@$properties) {
	print file_out "|".$property;
}
print file_out "\n";
</pre></p>
<li>For the first 20 User-Agents in the input file, perform a match then
write the User-Agent along with the values for chosen properties to
the CSV.
<p><pre class="prettyprint lang-pl">
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
</pre></p>
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
close $file_out;

say "Output Written to $outputFile";
# // Snippet End
