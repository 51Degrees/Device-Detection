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
Getting started example of using 51Degrees device detection match metrics 
information. The example shows how to:
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
<li>Produce a match for a single HTTP User-Agent header
<p><pre class="prettyprint lang-pl">
my $match = $provider->getMatch($userAgent)
</pre></p>
<li>Obtain device Id: consists of four components separated by a hyphen 
symbol: Hardware-Platform-Browser-IsCrawler where each Component 
represents an ID of the corresponding Profile.
<p><pre class="prettyprint lang-pl">match->getDeviceId();</pre>
<li>obtain match method: provides information about the 
algorithm that was used to perform detection for a particular User-Agent. 
For more information on what each method means please see: 
<a href="https://51degrees.com/support/documentation/pattern">
How device detection works</a>
<p><pre class="prettyprint lang-pl">match->getMethod();</pre>
<li>obtain difference:  used when detection method is not Exact or None. 
This is an integer value and the larger the value the less confident the 
detector is in this result.
<p><pre class="prettyprint lang-pl">match->getDifference();</pre>
<li>obtain signature rank: an integer value that indicates how popular 
the device is. The lower the rank the more popular the signature.
<p><pre class="prettyprint lang-pl">match->getRank();</pre>
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
my $propertyList = "IsMobile";
my $cacheSize = 10000;
my $poolSize = 20;

# User-Agent string of an iPhone mobile device.
my $mobileUserAgent = "Mozilla/5.0 (iPhone; CPU iPhone OS 7_1 like Mac OS X) ".
"AppleWebKit/537.51.2 (KHTML, like Gecko) 'Version/7.0 ".
"Mobile/11D167 Safari/9537.53";

# User-Agent string of Firefox Web browser version 41 on dektop.
my $desktopUserAgent = "Mozilla/5.0 (Windows NT 6.3; WOW64; rv:41.0) ".
"Gecko/20100101 Firefox/41.0";

# User-Agent string of a MediaHub device.
my $mediaHubUserAgent = "Mozilla/5.0 (Linux; Android 4.4.2; X7 Quad Core ".
"Build/KOT49H) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 ".
"Chrome/30.0.0.0 Safari/537.36";

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

# output_metrics function. Takes a match object as an argument and
# prints the match metrics relating to the specific match.

sub output_metrics {
	my $match = $_[0];
	my $deviceId = $match->getDeviceId();
	my $method = $match->getMethod();
	my $difference = $match->getDifference();
	my $rank = $match->getRank();

	say "   Device Id: $deviceId";
	say "   Match Method: $method";
	say "   Difference: $difference";
	say "   Rank: $rank";

	return;
}

say "Staring Gettings Started Match Metrics Example.";

# Carries out a match with a mobile User-Agent.
say "\nMobile User-Agent: $mobileUserAgent";
my $match = $provider->getMatch($mobileUserAgent);
output_metrics($match);

# Carries out a desktop with a mobile User-Agent.
say "\nDesktop User-Agent: $desktopUserAgent";
my $match = $provider->getMatch($desktopUserAgent);
output_metrics($match);

# Carries out a MediaHub with a mobile User-Agent.
say "\nMediaHub User-Agent: $mediaHubUserAgent";
my $match = $provider->getMatch($mediaHubUserAgent);
output_metrics($match);
# // Snippet End
