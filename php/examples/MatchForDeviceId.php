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
<p><code>
$provider = FiftyOneDegreesPatternV3::provider_get();
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
This example assumes you have the 51Degrees PHP API installed correctly,
and have FiftyOneDegreesPatternV3.php in this directory.
</tutorial>
*/

require("FiftyOneDegreesPatternV3.php");
$provider = FiftyOneDegreesPatternV3::provider_get();

// Device id string of an iPhone mobile device.
$mobileDeviceId = "12280-48866-24305-18092";

// Device id string of Firefox Web browser version 41 on dektop.
$desktopDeviceId = "15364-21460-53251-18092";

// Device id string of a MediaHub device.
$mediaHubDeviceId = "41231-46303-24154-18092";

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
?>
