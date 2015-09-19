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
use FiftyOneDegrees::PatternV3;
use JSON;
use Data::Dumper;
use feature qw/say/;
use Getopt::Long;

#Declare the main variables in use. They can be passes in at the command line.
my $filename;
my $propertyList;
my $userAgentString;

$userAgentString //= "Mozilla/5.0 (Linux; Android 4.4.2; en-us; SAMSUNG SCH-I545 Build/KOT49H) AppleWebKit/537.36 (KHTML, like Gecko) Version/1.5 Chrome/28.0.1500.94 Mobile Safari/537.36";

$httpHeadersString //= 	"User-Agent: Opera/9.52 (X11; Linux i686; U; en)\r\n" .
						"Device-Stock-UA: $userAgentString\r\n";

$propertyList //= "Id,BrowserName,BrowserVendor,BrowserVersion,CookiesCapable,IsTablet,IsMobile,IsCrawler";

$filename //= "../../../data/51Degrees-LiteV3.2.dat";

my $r = GetOptions('filename|f=s' => \$filename,
                   'properties|p=s' => \$propertyList,
                   'ua=s' => \$userAgentString);

die "DAT file does not exist! usage: $0 -f <dat filename>\n" unless -e ("$filename");

# Initialize Pattern library

# For pattern a dataset pointer is needed. This will be passd into other
# PatternV3 functions.
eval {

	my $json;
	my $dsPointer = FiftyOneDegrees::PatternV3::dataSetInitWithPropertyString($filename, $propertyList, 10, 10000);

	say "\r\n=== User-Agent Data Input ===";
	say $userAgentString;
	$json = FiftyOneDegrees::PatternV3::getMatch($dsPointer, $userAgentString);

	say "\r\n=== User-Agent Data Output ===";
	my %properties = %{ decode_json($json) };
	# Loop through and print all returned properties
	while (my ($key, $value) = each %properties) {
		say $key . ": " . $value;
	}

	say "\r\n=== HTTP Headers Data Input ===";
	say $httpHeadersString;
	$json = FiftyOneDegrees::PatternV3::getMatchWithHeaders($dsPointer, $httpHeadersString);

	say "\r\n=== HTTP Headers Data Output ===";
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
