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
Match with device id example of using 51Degrees device detection. The example
shows how to:
<ol>
<li>Fetch a pointer to the 51Degrees device detection provider instance.
This is instantiated on server startup and uses settings from php.ini.
<p><pre class="prettyprint lang-php">
$provider = FiftyOneDegreesPatternV3::provider_get();
</pre></p>
<li>Produce a match for a single device id
<p><pre class="prettyprint lang-php">
my $match = $provider->getMatchForDeviceId($userAgent)
</pre></p>
<li>Extract the value of the IsMobile property
<p><pre class="prettyprint lang-php">
$match->getValue('IsMobile')
</pre></p>
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

// Fetches the device id for a mobile User-Agent.
$match = $provider->getMatch($mobileUserAgent);
$mobileDeviceId = $match->getDeviceId();

// Fetches the device id for a desktop User-Agent.
$match = $provider->getMatch($desktopUserAgent);
$desktopDeviceId = $match->getDeviceId();

// Fetches the device id for a MediaHub User-Agent.
$match = $provider->getMatch($mediaHubUserAgent);
$mediaHubDeviceId = $match->getDeviceId();

echo "Starting Match With Device Id Example<br>\n";

// Carries out a match with a mobile device id.
echo "<br>\nMobile Device Id: ".$mobileDeviceId."<br>\n";
$match = $provider->getMatchForDeviceId($mobileDeviceId);
echo "   IsMobile: ".$match->getValue("IsMobile")."<br>\n";

// Carries out a match with a desktop device id.
echo "<br>\nDesktop Device Id: ".$desktopDeviceId."<br>\n";
$match = $provider->getMatchForDeviceId($desktopDeviceId);
echo "   IsMobile: ".$match->getValue("IsMobile")."<br>\n";

// Carries out a match with a MediaHub device id.
echo "<br>\nMediaHub DeviceId: ".$mediaHubDeviceId."<br>\n";
$match = $provider->getMatchForDeviceId($mediaHubDeviceId);
echo "   IsMobile: ".$match->getValue("IsMobile")."<br>\n";
// Snippet End
?>
