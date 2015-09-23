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
use FiftyOneDegrees::TrieV3;
use JSON;
use Data::Dumper;
use feature qw/say/;
use Getopt::Long;

#Declare the main variables in use. They can be passes in at the command line.
my $filename;
my $propertyList;
my $userAgentString;

$userAgentString //= "Mozilla/5.0 (Linux; Android 4.4.2; en-us; SAMSUNG SCH-I545 Build/KOT49H) AppleWebKit/537.36 (KHTML, like Gecko) Version/1.5 Chrome/28.0.1500.94 Mobile Safari/537.36";

$httpHeadersString //= 	"HTTP_USER_AGENT: Opera/9.52 (X11; Linux i686; U; en)\r\n" .
						"HTTP_DEVICE_STOCK_UA: $userAgentString\r\n";

$propertyList //= "Id,BrowserName,BrowserVendor,BrowserVersion,CookiesCapable,IsTablet,IsMobile,IsCrawler";

$filename //= "../../../data/51Degrees-LiteV3.2.trie";

my $r = GetOptions('filename|f=s' => \$filename,
                   'properties|p=s' => \$propertyList,
                   'ua=s' => \$userAgentString);

die "TRIE file does not exist! usage: $0 -f <dat filename>\n" unless -e ("$filename");

sub output_headers {
	my $httpHeader = FiftyOneDegrees::TrieV3::getHttpHeaderName($httpHeaderIndex);
	while ($httpHeader ne "") {
		push (@httpHeaders, $httpHeader);
		$httpHeaderIndex++;
		$httpHeader = FiftyOneDegrees::TrieV3::getHttpHeaderName($httpHeaderIndex);
	}
	say "\r\n=== Supported HTTP Headers ===";
	foreach $httpHeader (@httpHeaders) {
		say $httpHeader;
	}
}

sub output_properties {
	my %properties = %{ decode_json(shift) };
	# Loop through and print all returned properties
	while (my ($key, $value) = each %properties) {
		say $key . ": " . $value;
	}
}

sub output_user_agent {
	say "\r\n=== User-Agent Data Input ===";
	say $userAgentString;
	my $json = FiftyOneDegrees::TrieV3::getMatch($userAgentString);
	say "\r\n=== User-Agent Data Output ===";
	output_properties($json);
}

sub output_http_headers {
	say "\r\n=== HTTP Headers Data Input ===";
	say $httpHeadersString;
	my $json = FiftyOneDegrees::TrieV3::getMatchWithHeaders($httpHeadersString);
	say "\r\n=== HTTP Headers Data Output ===";
	output_properties($json);
}


# For pattern a dataset pointer is needed. This will be passd into other
# TrieV3 functions.
eval {

	# Initialise the static Trie data structures.
	FiftyOneDegrees::TrieV3::dataSetInitWithPropertyString($filename, $propertyList);

	# Output the HTTP headers used for detection.
	output_headers();

	# Perform a detection using only the User-Agent header.
	output_user_agent();

	# Perform a detection using all the important HTTP headers.
	output_http_headers();

	# Release the memory used for the trie data.
	FiftyOneDegrees::TrieV3::destroy();
};
if ($@) {
    die "Error: $@\n";
}
