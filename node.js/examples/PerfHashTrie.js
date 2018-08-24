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
Hash Trie Performance example of using 51Degrees device detection. The example
shows how to:
<ol>
<li>Set the configuration using a json object
<p><pre class="prettyprint lang-js">
var config = {"dataFile" : "../../data/51Degrees-LiteV3.4.trie",
              "properties" : "IsMobile",
              "stronglyTyped" : false
};
</pre></p>
<li>Instantiate the 51Degrees device detection provider with this
connfiguration
<p><pre class="prettyprint lang-js">
var provider = new fiftyonedegrees.provider(config);
</pre></p>
<li>Read user agents from a file and calculate the ammount of time it takes to 
match them all using the provider.
<p><pre class="prettyprint lang-js">
start = new Date().getTime();
for (var index = 0, len = array.length; index < len; index++){
    // Perform a match for the current user agent
    var match = provider.getMatch(array[index]);
}
var end = new Date().getTime() - start;
</pre></p>
</ol>
<p>This example assumes you have the 51Degrees Node.js API installed correctly,
see the instructions in the Node.js readme file in this repository:
(Device-Detection/node.js/README.md).</p>
<p>The examples also assumes you have access to a Hash Trie data file and
have set the path to "20000 User Agents.csv" correctly. Both of these files 
need to be available in the data folder in the root of this repository. Please 
see data/TRIE.txt for more details on downloading the Hash Trie data file.</p>
<p>A file containing 1-million User-Agents can be downloaded from
https://51Degrees.com/million.zip </p>
</tutorial>
*/

// Snippet Start
// Include 51Degrees.
var fiftyonedegrees = require("fiftyonedegreescore");

// Set the config.
var passes = 5

var config = {"dataFile" : require("fiftyonedegreeslitetrie"), //"../../data/51Degrees-LiteV3.4.trie"
              "properties" : "IsMobile",
              "stronglyTyped" : false
             };

var inputFile = __dirname + "/../../data/20000 User Agents.csv";

/*
Initialises the device detection provider with settings from the config.
By default this will use the packaged Lite data file. For more info see:
<a href="https://51degrees.com/compare-data-options">compare data options
</a>
*/
var provider = new fiftyonedegrees.provider(config);

// Perform the perfomance test. First calibrate by calculating the ammount of 
// time it takes to loop over all the User Agents and then remove this from the 
// final result.
var performanceTest = function(array) {
// Calibrate.
    var start = new Date().getTime();
    for (var index = 0, len = array.length; index < len; index++){
        // Do nothing.
    }
    var calibrate = new Date().getTime() - start;

// Perform test.
    start = new Date().getTime();
    for (var index = 0, len = array.length; index < len; index++){
        // Perform a match for the current user agent
        var match = provider.getMatch(array[index]);
    }
    var end = new Date().getTime() - start;

    // Calculate time spent preforming detections.
    var time = end - calibrate;
    return time; 
}

console.log("Starting PerfHashTrie.\n");

// Read user agents file into an array
var fs = require('fs');
var uas = fs.readFileSync(inputFile).toString().split("\n");

var cumulativeTime = 0;
// Perform peformance test for given number of passes.
for (var i = 0; i < passes; i++){
    var time = performanceTest(uas);
    cumulativeTime += time;
    console.log("Pass: %d - Detection Time %d", i, (time / 1000));
}

// Calculate the average time it takes to match all the User-Agents
var averageTime = cumulativeTime / passes;
// Caclulate the detections per second
var dps = uas.length / (averageTime /1000);
// Calculate the time per detection in ms.
var spd = Math.pow(dps, -1) * 1000;

console.log("Execution time: %d", (averageTime /1000));
console.log("Detections per second %d", dps)
console.log("Time for a single detection (ms): %d", spd);

