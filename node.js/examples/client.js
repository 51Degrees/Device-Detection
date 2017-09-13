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
var http = require("http");
var fiftyonedegrees = require("fiftyonedegreescore");
var config = {};

// Get the path to a Lite data file.
if (process.argv[2] === "pattern") {
    config.dataFile = require('fiftyonedegreeslitepattern');
    console.log("Starting new Pattern provider.")
}
else if (process.argv[2] === "trie") {
    config.dataFile = require('fiftyonedegreeslitetrie');
    console.log("Starting new Trie provider.")
}
else {
    config.dataFile = require('fiftyonedegreeslitepattern');
    console.log("Starting new Pattern provider (default).");
}

// Initialise a new Provider.
var provider = new fiftyonedegrees.provider(config);

var server = http.createServer(function (req, res) {
    // Get a match.
    provider.getMatch(req);

    // Print the type of device.
    if (req.fiftyoneDevice.IsMobile) {
        res.write("This is a mobile device.\n");
    }
    else {
        res.write("This is a non-mobile device.\n");
    }

    // Print all the properties for the device.
    res.write("Here are all its properties:\n\n");
    provider.availableProperties.forEach(function(property) {
        res.write(property + " : " + req.fiftyoneDevice[property] + "\n");
    })
    res.end();
}).listen(3000, function () {
    console.log("Test HTTP server listening on port 3000");
});
