<?php
/*
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
*/

/*
<tutorial>
Getting started example of using the 51Degrees Common API for device detection. 
The example shows how to:
<ol>
<li>Start a session. When using the Cloud API, using PHP sessions allows you to 
persist variables between requests.
<p><pre class="prettyprint lang-php">
session_start();
</pre></p>
<li>Initialise the 51Degrees device detection provider instance.
When using the Cloud or On-Premise provider, this is instantiated when the page 
is loaded and uses settings from config.php. When using the On-Premise 
extension only, this is instantiated on server startup and uses settings from 
php.ini and config.php.
<p><pre class="prettyprint lang-php">
$provider = FiftyOneDegrees\FiftyOneDegreesGetProvider($settings);
</pre></p>
<li>Produce a match for a single HTTP User-Agent header
<p><pre class="prettyprint lang-php">
my $match = $provider->getMatch($userAgent)
</pre></p>
<li>Extract the value of the IsMobile property
<p><pre class="prettyprint lang-php">
$match->getValue('IsMobile')
</pre></p>
</ol>
This example assumes you have configured the 51Degrees PHP Common API correctly 
and if using On-Premise detection, have 
FiftyOneDegreesPatternV3.php/FiftyOneDegreesTrieV3.php in this directory. Also 
assumed is the examples directory has been copied from the repository to your 
webserver document root.
</tutorial>
*/
// Snippet Start
session_start();

// Load the Common API 
require("../51degrees.php");

// Load settings passed to the Common API, used to configure the provider.
require("../config.php");

// Create a pointer the the Common API provider.
$provider = FiftyOneDegrees\FiftyOneDegreesGetProvider($settings);

// User-Agent string of an iPhone mobile device.
$mobileUserAgent = "Mozilla/5.0 (iPhone; CPU iPhone OS 7_1 like Mac OS X) ".
"AppleWebKit/537.51.2 (KHTML, like Gecko) 'Version/7.0 Mobile/11D167 ".
"Safari/9537.53";

// User-Agent string of Firefox Web browser version 41 on desktop.
$desktopUserAgent = "Mozilla/5.0 (Windows NT 6.3; WOW64; rv:41.0) ".
"Gecko/20100101 Firefox/41.0";

// User-Agent string of a MediaHub device.
$mediaHubUserAgent = "Mozilla/5.0 (Linux; Android 4.4.2; X7 Quad Core ".
"Build/KOT49H) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 ".
"Chrome/30.0.0.0 Safari/537.36";

echo "Starting Getting Started Example.<br>\n";

// Carries out a match with a mobile User-Agent.
echo "<br>\nMobile User-Agent: ".$mobileUserAgent."<br>\n";
$match = $provider->getMatch($mobileUserAgent);
echo "IsMobile: ".$match->getValue("IsMobile")."<br>\n";

// Carries out a match with a desktop User-Agent.
echo "<br>\nDesktop User-Agent: ".$desktopUserAgent."<br>\n";
$match = $provider->getMatch($desktopUserAgent);
echo "IsMobile: ".$match->getValue("IsMobile")."<br>\n";

// Carries out a match with a MediaHub User-Agent.
echo "<br>\nMediaHub User-Agent: ".$mediaHubUserAgent."<br>\n";
$match = $provider->getMatch($mediaHubUserAgent);
echo "IsMobile: ".$match->getValue("IsMobile")."<br>\n";
// Snippet End
?>