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
Match for device id example of using 51Degrees device detection. The example
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
<li>Produce a match for a single device id
<p><code>
my $match = $provider->getMatchForDeviceId($userAgent)
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
use FiftyOneDegrees::PatternV3;
use feature qw/say/;

my $filename = "../../data/51Degrees-LiteV3.2.dat";
my $propertyList = "";
my $cacheSize = 10000;
my $poolSize = 20;

# Device id of an iPhone mobile device.
my $mobileDeviceId = "12280-48866-24305-18092";

# Device id of of Firefox Web browser version 41 on dektop.
my $desktopDeviceId = "15364-21460-53251-18092";

# Device id of a MediaHub device.
my $mediaHubDeviceId = "41231-46303-24154-18092";


#Initialises the device detection provider with settings from the settings
#file. By default this will use the included Lite data file For more info
#see:
#<a href="https://51degrees.com/compare-data-options">compare data options
#</a>

my $provider = new FiftyOneDegrees::PatternV3::Provider(
	$filename,
	$propertyList,
	$cacheSize,
	$poolSize);


say "Starting Match With Device Id Example.";

# Carries out a match with a mobile device id.
say "\nMobile Device Id: $mobileDeviceId";
my $match = $provider->getMatchForDeviceId($mobileDeviceId);
my $isMobile =  $match->getValue("IsMobile");
say "   IsMobile: $isMobile";

# Carries out a match with a desktop device id.
say "\nDesktop Device Id: $desktopDeviceId";
my $match = $provider->getMatchForDeviceId($desktopDeviceId);
my $isMobile =  $match->getValue("IsMobile");
say "   IsMobile: $isMobile";

# Carries out a match with a MediaHub device id.
say "\nMobile Device Id: $mediaHubDeviceId";
my $match = $provider->getMatchForDeviceId($mediaHubDeviceId);
my $isMobile =  $match->getValue("IsMobile");
say "   IsMobile: $isMobile";
