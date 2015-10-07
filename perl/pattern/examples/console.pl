# *********************************************************************
# This Source Code Form is copyright of 51Degrees Mobile Experts Limited.
# Copyright 2015 51Degrees Mobile Experts Limited, 5 Charlotte Close,
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
use feature qw/say/;
use Getopt::Long;
use JSON;

#Declare the main variables in use. They can be passes in at the command line.
my $filename;
my $propertyList;
my $userAgentString;
my $httpHeaders;

# Example User-Agent.
$userAgentString = "Mozilla/5.0 (Linux; Android 4.4.2; en-us; SAMSUNG " .
	"SCH-I545 Build/KOT49H) AppleWebKit/537.36 (KHTML, like Gecko) " .
	"Version/1.5 Chrome/28.0.1500.94 Mobile Safari/537.36";

# Example set of HTTP headers.
$httpHeaders = new FiftyOneDegrees::PatternV3::MapStringString();
$httpHeaders->set("HTTP_USER_AGENT", "Opera/9.52 (X11; Linux i686; U; en)");
$httpHeaders->set("HTTP_DEVICE_STOCK_UA", $userAgentString);
$httpHeaders->set("HTTP_ACCEPT_TYPES", "*/*");

# List of 51Degrees properties to make available. Where a property is not
# supported in the provided data file it will not be available.
my $propertyList = "BrowserName,BrowserVendor,BrowserVersion," .
				"DeviceType,HardwareVendor,IsTablet,IsMobile," .
				"IsCrawler,ScreenInchesDiagonal,ScreenPixelsWidth";

# Replace with the following lines for Premium or Enterprise data.
#
# my $filename = "../../../data/51Degrees-PremiumV3.2.dat";
# my $filename = "../../../data/51Degrees-EnterpriseV3.2.dat";
#
# Premium and Enterprise data files contain more properties, are updated
# more frequently and are more accurate than the free Lite data.
#
# See https://51degrees.com/compare-data-options to get data files.
#
# Set the location of the source data file for the web server.
$filename = "../../../data/51Degrees-LiteV3.2.dat";

my $r = GetOptions('filename|f=s' => \$filename,
                   'properties|p=s' => \$propertyList,
                   'ua=s' => \$userAgentString);

sub output_headers {
	my $provider = shift;
	say "\r\n*** Supported HTTP Headers ***";
	my $httpHeaders = $provider->getHttpHeaders();
	foreach $httpHeader (@$httpHeaders) {
		say $httpHeader;
	}
}

sub output_match {
    my $provider = shift;
	my $match = shift;
	my $properties = $provider->getAvailableProperties();
	say "Id: " . $match->getDeviceId();
	foreach $property (@$properties) {
        my $values = $match->getValues($property);
        say "$property: " . join(', ', @$values);
	}
}

sub output_json {
    my %properties = %{ decode_json(shift) };
    while(($property, $values) = each(%properties)) {
        say "$property: $values";
    }
}

sub output_user_agent {
	my $provider = shift;
	say "\r\n*** User-Agent Data Input ***";
	say $userAgentString;
	my $match = $provider->getMatch($userAgentString);
	say "\r\n*** User-Agent - Match ***";
	output_match($provider, $match);
	say "\r\n*** User-Agent - JSON ***";
	output_json($provider->getMatchJson($userAgentString));
}

sub output_http_headers {
	my $provider = shift;
	say "\r\n*** HTTP Headers Data Input ***";
	my $availableHttpHeaders = $provider->getHttpHeaders();
	foreach $httpHeader (@$availableHttpHeaders) {
		if ($httpHeaders->has_key($httpHeader)) {
			say "$httpHeader: " . $httpHeaders->get($httpHeader);
		}
	}
	my $match = $provider->getMatch($httpHeaders);
	say "\r\n*** HTTP Headers - Match ***";
	output_match($provider, $match);
	say "\r\n*** HTTP Headers - JSON ***";
	output_json($provider->getMatchJson($httpHeaders));
}

eval {

	my $counter = 0;

	while ($counter < 1) {

        # For pattern a dataset pointer is needed. This will be passed into other
        # PatternV3 functions.
        my $provider = new FiftyOneDegrees::PatternV3::Provider(
        	$filename,
        	$propertyList);

		# Output the HTTP headers used for detection.
		output_headers($provider);

		# Perform a detection using only the User-Agent header.
		output_user_agent($provider);

		# Perform a detection using all the important HTTP headers.
		output_http_headers($provider);

		say "Finished loop: $counter";

		$counter++;

	}

    <STDIN>;

};
if ($@) {
    die "Error: $@\n";
}
