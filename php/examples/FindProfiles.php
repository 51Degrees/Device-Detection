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
Find profiles example of using 51Degrees device detection. The example
shows how to:
<ol>
<li>Fetch a pointer to the 51Degrees device detection provider instance.
This is instantiated on server startup and uses settings from php.ini.
<p><pre class="prettyprint lang-php">
$provider = FiftyOneDegreesPatternV3::provider_get();
</pre></p>
<li>Retrieve all the profiles in the data set with the specified property
value pair
<p><pre class="prettyprint lang-php">
$profiles = $provider->findProfiles("IsMobile", "True");
</pre></p>
<li>Search within a list of profiles for another property value pair.
<p><pre class="prettyprint lang-php">
$profiles = $provider->findProfilesInProfiles("ScreenPixelsWidth", "1080", $profiles);
</pre></p>
</ol>
This example assumes you have the 51Degrees PHP API installed correctly,
and have FiftyOneDegreesPatternV3.php in this directory.
</tutorial>
*/
// Snippet Start
require("../pattern/FiftyOneDegreesPatternV3.php");

$provider = FiftyOneDegreesPatternV3::provider_get();

echo "Starting Find Profiles Example.<br>\n";

// Retrive all the mobile profiles.
$profiles = $provider->findProfiles("IsMobile", "True");
echo "There are ".$profiles->getCount()." mobile profiles in the ".
	$provider->getDataSetName()." data set.<br>\n";
// Find how many have a screen width of 1080 pixels.
$profiles = $provider->findProfilesInProfiles("ScreenPixelsWidth", "1080", $profiles);
echo $profiles->getCount()." of them have a screen width of 1080 pixels.<br>\n";

// Retrive all the non-mobile profiles.
$profiles = $provider->findProfiles("IsMobile", "False");
echo "There are ".$profiles->getCount()." non-mobile profiles in the ".
	$provider->getDataSetName()." data set.<br>\n";
// Find how many have a screen width of 1080 pixels.
$profiles = $provider->findProfilesInProfiles("ScreenPixelsWidth", "1080", $profiles);
echo $profiles->getCount()." of them have a screen width of 1080 pixels.<br>\n";

// Snippet End
?>
