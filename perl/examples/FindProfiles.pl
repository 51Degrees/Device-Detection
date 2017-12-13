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
Find profiles example of using 51Degrees device detection. The example
shows how to:
<ol>
<li>Set the various settings for 51Degrees detector
<p><pre class="prettyprint lang-pl">
my $filename = "51Degrees-LiteV3.2.dat";
my $propertyList = "IsMobile";
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
<li>Retieve all the profiles fromt the data set which match the provided
property value pair
<p><pre class="prettyprint lang-pl">
my $profiles = $provider->findProfiles("IsMobile", "True");
</pre></p>
<li>Search within a list of profiles for another property value pair.
<p><pre class="prettyprint lang-pl">
my $profiles = $provider->findProfiles("ScreenPixelsWidth", "1080", $profiles);
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
my $propertyList = "IsMobile";
my $cacheSize = 10000;
my $poolSize = 20;

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

my $dataSetName = $provider->getDataSetName();

say "Starting Find Profiles Example.\n";

# Retrieve all the mobile profiles from the data set.
my $profiles = $provider->findProfiles("IsMobile", "True");
my $profilesCount = $profiles->getCount();
say "There are $profilesCount mobile profiles in the $dataSetName data set.";
# Find how many have a screen width of 1080 pixels.
my $profiles = $provider->findProfiles("ScreenPixelsWidth", "1080", $profiles);
my $profilesCount = $profiles->getCount();
say "$profilesCount of them have a screen width of 1080 pixels.";

# Retrieve all the non-mobile profiles from the data set.
my $profiles = $provider->findProfiles("IsMobile", "False");
my $profilesCount = $profiles->getCount();
say "There are $profilesCount non-mobile profiles in the $dataSetName data set.";
# Find how many have a screen width of 1080 pixels.
my $profiles = $provider->findProfiles("ScreenPixelsWidth", "1080", $profiles);
my $profilesCount = $profiles->getCount();
say "$profilesCount of them have a screen width of 1080 pixels.";
# // Snippet End
