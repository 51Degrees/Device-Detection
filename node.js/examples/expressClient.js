var express = require("express");
var app = express();
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

app.get('/', function(req, res) {
    var deviceString = '';
    
    // Get a match.
    provider.getMatch(req);
    
    // Print the type of device.
    if (req.device.IsMobile) {
        deviceString += "This is a mobile device.<br>\n";
    } else {
        deviceString += "This is a non-mobile device.<br>\n";
    }
    // Print all the properties for the device.
    deviceString += "Here are all its properties:<br><br>\n\n";
    provider.availableProperties.forEach(function(property) {
        deviceString += property + " : " + req.device[property] + "<br>\n";
    })

    res.send(deviceString);
})

app.get('/metrics', function(req, res) {
    if (provider.getDataSetName() === "Trie") {
        // Trie does not support match metrics.
        res.send("Match metrics not available in Trie.<br>\n");
    } else {
        var metricsString = '';
        // Get a match.
        provider.getMatch(req);

        // Print the match metrics.
        metricsString += "Device Id : " + req.device.Id + "<br>\n";
        metricsString += "Method : " + req.device.Method + "<br>\n";
        metricsString += "Difference : " + req.device.Difference + "<br>\n";
        metricsString += "Rank : " + req.device.Rank + "<br>\n";
        
        res.send(metricsString);
    }
})

app.listen(3000, function() {
    console.log("Test express server listening on port 3000");
});
