<<<<<<< HEAD
=======
# *********************************************************************
# This Source Code Form is copyright of 51Degrees Mobile Experts Limited.
# Copyright 2014 51Degrees Mobile Experts Limited, 5 Charlotte Close,
# Caversham, Reading, Berkshire, United Kingdom RG4 7BY
#
# This Source Code Form is the subject of the following patent
# applications, owned by 51Degrees Mobile Experts Limited of 5 Charlotte
# Close, Caversham, Reading, Berkshire, United Kingdom RG4 7BY:
# European Patent Application No. 13192291.6; and
# United States Patent Application Nos. 14/085,223 and 14/085,301.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0.
#
# If a copy of the MPL was not distributed with this file, You can obtain
# one at http://mozilla.org/MPL/2.0/.
#
# This Source Code Form is "Incompatible With Secondary Licenses", as
# defined by the Mozilla Public License, v. 2.0.
# **********************************************************************

#This is test program for testing the PERL API
>>>>>>> V32
use FiftyOneDegrees::PatternV3;
use JSON;
use Data::Dumper;
use feature qw/say/;
use Getopt::Long;

<<<<<<< HEAD
=======

#Declare the main variables in use. They can be passes in at the command line
>>>>>>> V32
my $filename;
my $propertyList;
my $userAgentString;

<<<<<<< HEAD
my $r = GetOptions('filename|f=s' => \$filename,
                   'properties|p=s' => \$propertyList,
                   'ua=s' => \$userAgentString);

$userAgentString //= "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/35.0.1916.114 Safari/537.36";

$propertyList //= "Id,BrowserName,BrowserVendor,BrowserVersion,CookiesCapable,IsTablet,IsMobile,IsCrawler";

die "DAT file does not exist! usage: $0 -f <dat filename>\n" unless -e ("$filename");

# Initialize Pattern library

# For pattern a dataset pointer is needed. This will be passd into other
# PatternV3 functions.
eval {

	my $dsPointer = FiftyOneDegrees::PatternV3::dataSetInitWithPropertyString($filename, $propertyList);
	my $json = FiftyOneDegrees::PatternV3::getMatch($dsPointer, $userAgentString);
	
	say "=== Data Output ===";
	my %properties = %{ decode_json($json) };
	# Loop through and print all returned properties
	while (my ($key, $value) = each %properties) {
		say $key . ": " . $value;
	}

	FiftyOneDegrees::PatternV3::destroyDataset($dsPointer);
};
if ($@) {
    die "Error: $@\n";
}
=======
# This is the code to pass arguments at the command line
# my $r = GetOptions('filename|f=s' => \$filename,
#                   'properties|p=s' => \$propertyList,
#                   'ua=s' => \$userAgentString);

#Filename hard coded to data file in main C project
$filename //= "/home/fod/Device-Detection/data/51Degrees-LiteV3.2.dat";
#User Agent string can be changed here
$userAgentString = "Mozilla/5.0 (Linux; U; Android 4.1.2; de-de; GT-N8020 Build/JZO54K) AppleWebKit/534.30 (KHTML, like Gecko) Version/4.0 Safari/534.30";
#my $userHeaderString = "User-Agent Mozilla/5.0 (Linux; U; Android 4.1.2; de-de; GT-N8020 Build/JZO54K) AppleWebKit/534.30 (KHTML, like Gecko) Version/4.0 Safari/534.30";
my $userHeaderString = "HTTP_USER_AGENT Mozilla/5.0 (Linux; U; Android 4.1.2; de-de; GT-N8020 Build/JZO54K) AppleWebKit/534.30 (KHTML, like Gecko) Version/4.0 Safari/534.30";
#ProperyList can be changed here
$propertyList //= "Id,BrowserName,BrowserVendor,BrowserVersion,CookiesCapable,IsTablet,IsMobile,IsCrawler";

die "DATa file does not exist! usage: $filename -f <dat filename>\n" unless -e ("$filename");

# Initialize Pattern library

eval {
# For pattern a data set provider need to be created.
# Pass in data filename, property lis, cache size and pool size
FiftyOneDegrees::PatternV3::dataSetProvider($filename, $propertyList, 10, 20);


# After creating a data set provider, the getMatch method can be executed.
#$matchPointer = FiftyOneDegrees::PatternV3::getMatch($userAgentString);
#for ($i=1;$i<1000000;$i++){
$matchPointer = FiftyOneDegrees::PatternV3::getMatchWithHeaders($userHeaderString);
# get available properties


say "=== JSON Data Output ===";
my %properties = %{ decode_json($matchPointer) };

# Loop through and print all returned properties
    while (my ($key, $value) = each %properties) {
	say $key . ": " . $value;
    }
# Free the Match method references
#FiftyOneDegrees::PatternV3::freeMatch($matchPointer);
 #}       
# Free all the memory used by the data set provider
FiftyOneDegrees::PatternV3::freeDataset();
       

};
if ($@) {
    die "Error: $@\n";
}
>>>>>>> V32
