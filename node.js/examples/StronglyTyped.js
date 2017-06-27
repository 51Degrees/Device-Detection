/*
This Source Code Form is copyright of 51Degrees Mobile Experts Limited.
Copyright (c) 2017 51Degrees Mobile Experts Limited, 5 Charlotte Close,
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
Getting started example of using 51Degrees device detection. The example
shows how to:
<ol>
<li>Set the configuration using a json object
<p><pre class="prettyprint lang-js">
var config = {"dataFile" : require("fiftyonedegreeslitepattern"),
              "properties" : "IsMobile",
              "stronglyTyped" : true
};
</pre></p>
<p>Note: stronglyTyped does not need to be set explicitly as true
is the default</p>
<li>Instantiate the 51Degrees device detection provider with this
connfiguration
<p><pre class="prettyprint lang-js">
var provider = new fiftyonedegrees.provider(config);
</pre></p>
<li>Produce a match for a single HTTP User-Agent header
<p><pre class="prettyprint lang-js">
var match = provider.getMatch(userAgent);
</pre><p>
<li>Extract the value of the IsMobile property as boolean
<p><pre class="prettyprint lang-js">
match.IsMobile;
</pre></p>
</ol>
</tutorial>
*/
// Snippet Start
var fiftyonedegrees = require("fiftyonedegreescore");

// Set the config.
var config = {"dataFile" : require("fiftyonedegreeslitepattern"),
              "properties" : "IsMobile",
              "stronglyTyped" : true
             };

var provider = new fiftyonedegrees.provider(config);

// User-Agent string of an iPhone mobile device.
var mobileUserAgent = ("Mozilla/5.0 (iPhone; CPU iPhone OS 7_1 like Mac OS X" +
") AppleWebKit/537.51.2 (KHTML, like Gecko) 'Version/7.0 Mobile/11D167 " +
"Safari/9537.53")

// User-Agent string of Firefox Web browser version 41 on desktop.
var desktopUserAgent = ("Mozilla/5.0 (Windows NT 6.3; WOW64; rv:41.0) " +
"Gecko/20100101 Firefox/41.0")
// User-Agent string of a MediaHub device.
var mediaHubUserAgent = ("Mozilla/5.0 (Linux; Android 4.4.2; X7 Quad Core " +
"Build/KOT49H) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 " +
"Chrome/30.0.0.0 Safari/537.36")

console.log('Unlike other examples, stronglyTyped is set to \'true\' ' +
            'in the config. This is the default if not set.\n');

console.log('Mobile User-Agent: ' + mobileUserAgent)

// Determines whether the mobile User-Agent is a mobile device.
var match = provider.getMatch(mobileUserAgent);
if (match.IsMobile){
    console.log('   Mobile\n');
} else {
    console.log('   Non-Mobile\n');
}
match.close();

console.log('Desktop User-Agent: ' + desktopUserAgent)

// Determines whether the desktop User-Agent is a mobile device.
var match = provider.getMatch(desktopUserAgent);
if (match.IsMobile){
    console.log('   Mobile\n');
} else {
    console.log('   Non-Mobile\n');
}
match.close();

console.log('MediaHub User-Agent: ' + mediaHubUserAgent)

// Determines whether the MediaHub User-Agent is a mobile device.
var match = provider.getMatch(mediaHubUserAgent);
if (match.IsMobile){
    console.log('   Mobile\n');
} else {
    console.log('   Non-Mobile\n');
}
match.close();
// Snippet End
