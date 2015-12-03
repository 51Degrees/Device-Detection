<?php
/*
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
*/

/*
<tutorial>
Getting started example of using 51Degrees device detection match metrics 
information. The example shows how to:
<ol>
<li>Fetch a pointer to the 51Degrees device detection provider instance.
This is instantiated on server startup and uses settings from php.ini.
<p><pre class="prettyprint lang-php">
$provider = FiftyOneDegreesPatternV3::provider_get();
</pre></p>
<li>Produce a match for a single HTTP User-Agent header
<p><pre class="prettyprint lang-php">
my $match = $provider->getMatch($userAgent)
</pre></p>
<li>Extract the value of the IsMobile property
<p><pre class="prettyprint lang-php">
$match->getValue('IsMobile')
</pre></p>
<li>Obtain device Id: consists of four components separated by a hyphen 
symbol: Hardware-Platform-Browser-IsCrawler where each Component 
represents an ID of the corresponding Profile.
<p><pre class="prettyprint lang-php">$match->getDeviceId();</pre>
<li>Obtain match method: provides information about the 
algorithm that was used to perform detection for a particular User-Agent. 
For more information on what each method means please see: 
<a href="https://51degrees.com/support/documentation/pattern">
How device detection works</a>
<p><pre class="prettyprint lang-php">$match->getMethod();</pre>
<li>Obtain difference:  used when detection method is not Exact or None. 
This is an integer value and the larger the value the less confident the 
detector is in this result.
<p><pre class="prettyprint lang-php">$match->getDifference();</pre>
<li>Obtain signature rank: an integer value that indicates how popular 
the device is. The lower the rank the more popular the signature.
<p><pre class="prettyprint lang-php">$match->getRank();</pre>
</ol>
This example assumes you have the 51Degrees PHP API installed correctly,
and have FiftyOneDegreesPatternV3.php in this directory.
</tutorial>
*/
// Snippet Start
require("FiftyOneDegreesPatternV3.php");

$provider = FiftyOneDegreesPatternV3::provider_get();

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

// output_metrics function. Takes a match object as an argument
// and print the match metrics relating to the specific match.

function output_metrics($match) {
echo "Device Id: ".$match->getDeviceId()."<br>\n";
echo "Method: ".$match->getMethod()."<br>\n";
echo "Difference: ".$match->getDifference()."<br>\n";
echo "Rank: ".$match->getRank()."<br>\n";
}

echo "Starting Getting Started Match Metrics Example.<br>\n";

// Carries out a match with a mobile User-Agent.
echo "<br>\nMobile User-Agent: ".$mobileUserAgent."<br>\n";
$match = $provider->getMatch($mobileUserAgent);
output_metrics($match);

// Carries out a match with a desktop User-Agent.
echo "<br>\nDesktop User-Agent: ".$desktopUserAgent."<br>\n";
$match = $provider->getMatch($desktopUserAgent);
output_metrics($match);

// Carries out a match with a MediaHub User-Agent.
echo "<br>\nMediaHub User-Agent: ".$mediaHubUserAgent."<br>\n";
$match = $provider->getMatch($mediaHubUserAgent);
output_metrics($match);
// Snippet End
?>
