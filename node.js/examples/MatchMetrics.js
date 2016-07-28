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
<li>Set the configuration using a json object
<p><pre class="prettyprint lang-js">
var config = {"dataFile" : "../data/51Degrees-LiteV3.2.dat",
              "properties" : "IsMobile",
              "cacheSize" : 10000,
              "poolSize" : 4
};
</pre></p>
<li>Instantiate the 51Degrees device detection provider with this
connfiguration
<p><pre class="prettyprint lang-js">
var provider = new FiftyOneDegreesV3.provider(config);
</pre></p>
<li>Produce a match for a single HTTP User-Agent header
<p><pre class="prettyprint lang-js">
var match = provider.getMatch(userAgent);
</pre><p>
<li>Obtain device Id: consists of four components separated by a hyphen 
symbol: Hardware-Platform-Browser-IsCrawler where each Component 
represents an ID of the corresponding Profile.
<p><pre class="prettyprint lang-js">match.getDeviceId()</pre>
<li>Obtain match method: provides information about the 
algorithm that was used to perform detection for a particular User-Agent. 
For more information on what each method means please see: 
<a href="https://51degrees.com/support/documentation/pattern">
How device detection works</a>
<p><pre class="prettyprint lang-js">match.getMethod()</pre>
<li>Obtain difference:  used when detection method is not Exact or None. 
This is an integer value and the larger the value the less confident the 
detector is in this result.
<p><pre class="prettyprint lang-js">match.getDifference()</pre>
<li>Obtain signature rank: an integer value that indicates how popular 
the device is. The lower the rank the more popular the signature.
<p><pre class="prettyprint lang-js">match.getRank()</pre>
</ol>
This example should be run in the examples directory as the path
to the data file is relative.
</tutorial>
*/

// Snippet Start
// Include 51Degrees.
var FiftyOneDegrees = require("../FiftyOneDegreesV3");

// Set the config.
var config = {"dataFile" : "../data/51Degrees-LiteV3.2.dat",
              "properties" : "IsMobile",
              "cacheSize" : 10000,
              "poolSize" : 4,
              "logLevel" : "none"
             };

/*
outputMetrics function. Takes a match object as an argument and
prints the match metrics relating to the specific match.
*/
var outputMetrics = function(match) {
    console.log("   Id: " + match.getDeviceId());
    console.log("   Match Method: " + match.getMethod());
    console.log("   Difference: " + match.getDifference());
    console.log("   Rank: " + match.getRank());
}

// User-Agent string of an iPhone mobile device.
var mobileUserAgent = "Mozilla/5.0 (iPhone; CPU iPhone OS 7_1 like Mac OS X) AppleWebKit/537.51.2 (KHTML, like Gecko) 'Version/7.0 Mobile/11D167 Safari/9537.53";

// User-Agent string of Firefox Web browser version 41 on desktop.
var desktopUserAgent = "Mozilla/5.0 (Windows NT 6.3; WOW64; rv:41.0) Gecko/20100101 Firefox/41.0";

// User-Agent string of a MediaHub device.
var mediaHubUserAgent = "Mozilla/5.0 (Linux; Android 4.4.2; X7 Quad Core Build/KOT49H) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/30.0.0.0 Safari/537.36";

console.log("Starting Match Metrics Example.\n");

/*
Initialises the device detection provider with settings from the config.
By default this will use the included Lite data file For more info see:
<a href="https://51degrees.com/compare-data-options">compare data options
</a>
*/
var provider = new FiftyOneDegrees.provider(config);

// Carries out a match with a mobile User-Agent.
console.log("Mobile User-Agent: " + mobileUserAgent);
var match = provider.getMatch(mobileUserAgent);
outputMetrics(match);
match.close();

// Carries out a match with a desktop User-Agent.
console.log("Desktop User-Agent: " + desktopUserAgent);
var match = provider.getMatch(desktopUserAgent);
outputMetrics(match);
match.close();

// Carries out a match with a MediaHub User-Agent.
console.log("Media Hub User-Agent: " + mediaHubUserAgent);
var match = provider.getMatch(mediaHubUserAgent);
outputMetrics(match);
match.close();
// Snippet End
