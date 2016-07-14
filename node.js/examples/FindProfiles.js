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
Find profiles example of using 51Degrees device detection. The example
shows how to:
<ol>
<li>Set the configuration using a json object
<p><pre class="prettyprint lang-js">
var config = {"dataFile" : "../../data/51Degrees-LiteV3.2.dat",
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
<li>Retrieve all profiles from the data set which match the specified
property value pair
<p><pre class="prettyprint lang-js">
var profiles = provider.findProfiles("IsMobile", "True");
</pre>
</p>
<li>Search with a list of profiles for another property value pair.
<p><pre class="prettyprint lang-js">
profiles = provider.findProfiles("ScreenPixelsWidth", "1080", profiles)
</pre></p>
</ol>
This example should be run in the examples directory as the path
to the data file is relative.
</tutorial>
*/

// Snippet Start
// Include 51Degrees.
var FiftyOneDegrees = require("../FiftyOneDegreesV3");

// Set the config.
var config = {"dataFile" : "../../data/51Degrees-LiteV3.2.dat",
              "properties" : "IsMobile",
              "cacheSize" : 10000,
              "poolSize" : 4,
              "logLevel" : "none"
             };

console.log("Starting Find Profiles Example.\n");

/*
Initialises the device detection provider with settings from the config.
By default this will use the included Lite data file For more info see:
<a href="https://51degrees.com/compare-data-options">compare data options
</a>
*/
var provider = new FiftyOneDegrees.provider(config);

// Retrieve all the mobile profiles in the data set.
var profiles = provider.findProfiles("IsMobile", "True");
console.log("There are " + profiles.getCount() + " mobile profiles in the " + provider.getDataSetName() + " data file.");
profiles = provider.findProfiles("ScreenPixelsWidth", "1080", profiles);
console.log(profiles.getCount() +
            " of them have a screen width of 1080 pixels.");

// Retrieve all the non-mobile profiles in the data set.
profiles = provider.findProfiles("IsMobile", "False");
console.log("There are " + profiles.getCount() +
            " non-mobile profiles in the " + provider.getDataSetName() + 
            " data set.");
profiles = provider.findProfiles("ScreenPixelsWidth", "1080", profiles);
console.log(profiles.getCount() +
            " of them have a screen width of 1080 pixels.");
// Snippet End