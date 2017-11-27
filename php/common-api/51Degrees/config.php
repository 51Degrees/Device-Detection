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

/**
 * Settings array used when initialising a Provider object.
 *
 * * FiftyOneProvider - (string) - Cloud/TRIE/Pattern
 * * FiftyOneShareUsage - (boolean) - Enables or disabled usage sharing with 51Degrees.
 * * FiftyOneLicence - (string) - You licence key.
 * * FiftyOneLogLevel - (string) - debug/info/warn/fatal - defaults to fatal.
 * * FiftyOneLogFile - (string) - Log file location. - defaults to 51degrees.log in current directory
 * * FiftyOneUseSession - (string) - Whether to use sessions. Session must be started with PHP session_start() (Cloud only)
 * * FiftyOneUseSessionLifetime - (string) - How long a session should last (seconds) (Cloud only)
 * * FiftyOneProperties - (array) - FiftyOneProperties – Optional - An array of which properties to fetch (if left blank all are fetched). For the On-Premise version this is set in the extension properties. For more information about available properties see https://51degrees.com/resources/property-dictionary.
 **/

$settings = array(
    "FiftyOneProvider"=>"Cloud",
    "FiftyoneShareUsage"=>true,
    "FiftyOneLicence"=>"***YOUR_LICENCE_KEY***",
    "FiftyOneLogLevel"=>"fatal",
    "FiftyOneLogFile"=>"",
    "FiftyOneUseSession"=>true,
    "FiftyOneSessionLifetime"=>"",
    "FiftyOneProperties"=>""
);
?>