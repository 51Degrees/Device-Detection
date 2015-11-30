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
Getting started example of using 51Degrees device detection. The example
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
my $provider = new FiftyOneDegrees::PatternV3::Provider(<br>
	$dataFile,<br>
	$properties,<br>
	$cacheSize,<br>
	$poolSize);
</code></p>
<li>Produce a match for a single HTTP User-Agent header
<p><code>
my $match = $provider->getMatch($userAgent)
</code></p>
<li>Extract the value of the IsMobile property
<p><code>
$match->getValue('IsMobile')
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

say "Starting Getting Started Example.";

# Carries out a match with a mobile User-Agent.
say "\nMobile User-Agent: $mobileUserAgent";
my $match = $provider->getMatch($mobileUserAgent);
my $isMobile =  $match->getValue("IsMobile");
say "   IsMobile: $isMobile";

# Carries out a desktop with a mobile User-Agent.
say "\nDesktop User-Agent: $desktopUserAgent";
my $match = $provider->getMatch($desktopUserAgent);
my $isMobile =  $match->getValue("IsMobile");
say "   IsMobile: $isMobile";

# Carries out a MediaHub with a mobile User-Agent.
say "\nMediaHub User-Agent: $mediaHubUserAgent";
my $match = $provider->getMatch($mediaHubUserAgent);
my $isMobile =  $match->getValue("IsMobile");
say "   IsMobile: $isMobile";
# // Snippet End
