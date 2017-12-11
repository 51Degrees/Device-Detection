/* *********************************************************************
 * This Source Code Form is copyright of 51Degrees Mobile Experts Limited.
 * Copyright 2017 51Degrees Mobile Experts Limited, 5 Charlotte Close,
 * Caversham, Reading, Berkshire, United Kingdom RG4 7BY
 *
 * This Source Code Form is the subject of the following patents and patent
 * applications, owned by 51Degrees Mobile Experts Limited of 5 Charlotte
 * Close, Caversham, Reading, Berkshire, United Kingdom RG4 7BY:
 * European Patent No. 2871816;
 * European Patent Application No. 17184134.9;
 * United States Patent Nos. 9,332,086 and 9,350,823; and
 * United States Patent Application No. 15/686,066.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0.
 *
 * If a copy of the MPL was not distributed with this file, You can obtain
 * one at http://mozilla.org/MPL/2.0/.
 *
 * This Source Code Form is "Incompatible With Secondary Licenses", as
 * defined by the Mozilla Public License, v. 2.0.
 ********************************************************************** */

/*
<tutorial>
Match with device id example of using 51Degrees device detection. The example
shows how to:
<ol>
<li>Set the configuration using a json object
<p><pre class="prettyprint lang-js">
var config = {"dataFile" : require("fiftyonedegreeslitepattern"),
              "properties" : "IsMobile",
              "cacheSize" : 10000,
              "poolSize" : 4
};
</pre></p>
<li>Instantiate the 51Degrees device detection provider with this
connfiguration
<p><pre class="prettyprint lang-js">
var provider = new fiftyonedegrees.provider(config);
</pre></p>
<li>Produce a match for a single device id
<p><pre class="prettyprint lang-js">
var match = provider.getMatchForDeviceId(deviceId)
</pre><p>
<li>Extract the value of the IsMobile property
<p><pre class="prettyprint lang-js">
match['IsMobile']
</pre></p>
</ol>
</tutorial>
*/

// Snippet Start
// Include 51Degrees.
var fiftyonedegrees = require("fiftyonedegreescore");

// Set the config.
var config = {"dataFile" : require("fiftyonedegreeslitepattern"),
              "properties" : "IsMobile",
              "cacheSize" : 10000,
              "poolSize" : 4,
              "stronglyTyped" :false
             };

// User-Agent string of an iPhone mobile device.
var mobileUserAgent = "Mozilla/5.0 (iPhone; CPU iPhone OS 7_1 like Mac OS X) AppleWebKit/537.51.2 (KHTML, like Gecko) 'Version/7.0 Mobile/11D167 Safari/9537.53";

// User-Agent string of Firefox Web browser version 41 on desktop.
var desktopUserAgent = "Mozilla/5.0 (Windows NT 6.3; WOW64; rv:41.0) Gecko/20100101 Firefox/41.0";

// User-Agent string of a MediaHub device.
var mediaHubUserAgent = "Mozilla/5.0 (Linux; Android 4.4.2; X7 Quad Core Build/KOT49H) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/30.0.0.0 Safari/537.36";

// Gets a match for the supplied User-Agent and returns the device id.
var getDeviceId = function(userAgent) {
    try {
        var match = provider.getMatch(userAgent);
        var deviceId = match.getDeviceId();
    }
    finally {
        match.close();
        return deviceId;
    }
}
console.log("Starting Match For Device Id Example.");

/*
Initialises the device detection provider with settings from the config.
By default this will use the packaged Lite data file. For more info see:
<a href="https://51degrees.com/compare-data-options">compare data options
</a>
*/
var provider = new fiftyonedegrees.provider(config);

// Get the device id for a selection of User-Agents.
var mobileDeviceId = getDeviceId(mobileUserAgent);
var desktopDeviceId = getDeviceId(desktopUserAgent);
var mediaHubDeviceId = getDeviceId(mediaHubUserAgent);

// Carries out a match for a mobile device id.
console.log("\nMobileDeviceId : " + mobileDeviceId);
var match = provider.getMatchForDeviceId(mobileDeviceId);
console.log("   IsMobile: " + match["IsMobile"]);
match.close();

// Carries out a match for a desktop device id.
console.log("\nDesktopDeviceId : " + desktopDeviceId);
var match = provider.getMatchForDeviceId(desktopDeviceId);
console.log("   IsMobile: " + match["IsMobile"]);
match.close();

// Carries out a match for a MediaHub device id.
console.log("\nMediaHubDeviceId : " + mediaHubDeviceId);
var match = provider.getMatchForDeviceId(mediaHubDeviceId);
console.log("   IsMobile: " + match["IsMobile"]);
match.close();