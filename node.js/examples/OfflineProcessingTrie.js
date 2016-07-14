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
Offline processing example of using 51Degrees device detection. The example
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
<li>Open an input file with a list of User-Agents, and an output file,
<p><pre class="prettyprint lang-js">
var inStream = fs.createReadStream(inputFile);
var outStream = fs.createWriteStream(outputFile);
</pre></p>
<li>Write a header to the output file with the property names in '|'
separated CSV format ('|' separated because some User-Agents contain
commas)
<p><pre class="prettyprint lang-js">
outStream.write('User-Agent')
provider.availableProperties.forEach(function(property) {
    outStream.write('|' + property);
});
outStream.write('\n');
</pre></p>
<li>For the first 20 User-Agents in the input file, perform a match then
write the User-Agent along with the values for chosen properties to
the CSV.
<p><pre class="prettyprint lang-js">
rl.on('line', function (userAgent) {
    if (i < 20) {
        outStream.write(userAgent);
        var match = provider.getMatch(userAgent);
        provider.availableProperties.forEach(function(property) {
            outStream.write("|" + match.getValue(property));
        })
        outStream.write("\n");
        match.dispose();
    }
    i++;
});
</pre></p>
</ol>
This example should be run in the examples directory as the path
to the data file is relative.
</tutorial>
*/

// Snippet Start
// Include 51Degrees.
var FiftyOneDegrees = require("../FiftyOneDegreesV3");
// Inlcude file modules.
var fs = require("fs"),
    readline = require("readline");

// Set the config.
var config = {"dataFile" : "../../data/51Degrees-LiteV3.2.trie",
              "properties" : "IsMobile,PlatformName,PlatformVersion",
              "logLevel" : "none"
             };

var inputFile = "../../data/20000 User Agents.csv";
var outputFile = "offlineProcessingOutput.csv";

// Carries out match for first 20 User-Agents and prints results to
// output file.
var outputOfflineProcessing = function() {
    var inStream = fs.createReadStream(inputFile),
        rl = readline.createInterface(inStream, null),
        outStream = fs.createWriteStream(outputFile);
    inStream.on("close", function() {
        outStream.close();
        console.log("Output written to " + outputFile);
    })

    outStream.write("User-Agent");
    provider.availableProperties.forEach(function(property) {
        outStream.write("|" + property);
    })
    outStream.write("\n");

    var i = 0;
    rl.on('line', function (userAgent) {
        if (i < 20) {
            outStream.write(userAgent);
            var match = provider.getMatch(userAgent);
            provider.availableProperties.forEach(function(property) {
                outStream.write("|" + match.getValue(property));
            })
            outStream.write("\n");
            match.dispose();
        }
        i++;
    });
}

console.log("Starting Offline Processing Example.\n");

/*
Initialises the device detection provider with settings from the config.
By default this will use the included Lite data file For more info see:
<a href="https://51degrees.com/compare-data-options">compare data options
</a>
*/
var provider = new FiftyOneDegrees.provider(config);
outputOfflineProcessing();
// Snippet End