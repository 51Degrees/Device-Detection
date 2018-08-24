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
Getting started example of using 51Degrees device detection. The example
shows how to:
<ol>
<li>Set the configuration using a json object
<p><pre class="prettyprint lang-js">
var config = {"dataFile" : "../data/51Degrees-LiteV3.4.trie",
              "properties" : "IsMobile"
};
</pre></p>
<li>Instantiate the 51Degrees device detection provider with this
connfiguration
<p><pre class="prettyprint lang-js">
var provider = new fiftyonedegrees.provider(config);
</pre></p>
<li>Produce a match for a single HTTP User-Agent header
<p><pre class="prettyprint lang-js">
var match = provider.getMatch(userAgent);
</pre><p>
<li>Extract the value of the IsMobile property
<p><pre class="prettyprint lang-js">
match['IsMobile'];
</pre></p>
</ol>
This example should be run in the examples directory as the path
to the data file is relative.
</tutorial>
*/

// Snippet Start
// Include 51Degrees.
var fiftyonedegrees = require("fiftyonedegreescore");

// Set the config.
var config = {"dataFile" : require("fiftyonedegreeslitetrie"), //"../../data/51Degrees-LiteV3.4.trie"
              "properties" : "IsMobile",
              "stronglyTyped": false
             };

// User-Agent string of an iPhone mobile device.
var mobileUserAgent = "Mozilla/5.0 (iPhone; CPU iPhone OS 7_1 like Mac OS X) AppleWebKit/537.51.2 (KHTML, like Gecko) 'Version/7.0 Mobile/11D167 Safari/9537.53";

// User-Agent string of Firefox Web browser version 41 on desktop.
var desktopUserAgent = "Mozilla/5.0 (Windows NT 6.3; WOW64; rv:41.0) Gecko/20100101 Firefox/41.0";

// User-Agent string of a MediaHub device.
var mediaHubUserAgent = "Mozilla/5.0 (Linux; Android 4.4.2; X7 Quad Core Build/KOT49H) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/30.0.0.0 Safari/537.36";

console.log("Starting Getting Started Example.");

/*
Initialises the device detection provider with settings from the config.
By default this will use the packaged Lite data file. For more info see:
<a href="https://51degrees.com/compare-data-options">compare data options
</a>
*/
var provider = new fiftyonedegrees.provider(config);

// Carries out a match with a mobile User-Agent.
console.log("\nMobile User-Agent: " + mobileUserAgent);
var match = provider.getMatch(mobileUserAgent);
console.log("   IsMobile: " + match.IsMobile);

// Carries out a match with a desktop User-Agent.
console.log("\nDesktop User-Agent: " + desktopUserAgent);
var match = provider.getMatch(desktopUserAgent);
console.log("   IsMobile: " + match.IsMobile);

// Carries out a match with a MediaHub User-Agent.
console.log("\nMediaHub User-Agent: " + mediaHubUserAgent);
var match = provider.getMatch(mediaHubUserAgent);
console.log("   IsMobile: " + match.IsMobile);
// Snippet End