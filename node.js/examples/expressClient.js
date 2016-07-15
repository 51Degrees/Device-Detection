var express = require("express");
var app = express();
var FiftyOneDegrees = require("../FiftyOneDegreesV3");

// Initialise a new Provider.
if (process.argv[2] === "pattern") {
    var provider = new FiftyOneDegrees.provider("../pattern/config.json");
    console.log("Started new Pattern provider.")
} else if (process.argv[2] === "trie") {
    var provider = new FiftyOneDegrees.provider("../trie/config.json");
    console.log("Started new Trie provider.")
} else {
    var provider = new FiftyOneDegrees.provider("../pattern/config.json");
    console.log("Started new Pattern provider (default).")
}

// Enabling this means booleans and Object.keys can be used when
// getting properties from the device.
provider.config.nodeify = true;

app.get('/', function(req, res) {
    var deviceString = '';
    try {
        // Get a match.
        var device = new provider.getMatchForRequest(req);
        // Print the type of device.
        if (device.IsMobile) {
            deviceString += "This is a mobile device.<br>\n";
        } else {
            deviceString += "This is a non-mobile device.<br>\n";
        }
        // Print all the properties for the device.
        deviceString += "Here are all its properties:<br><br>\n\n";
        Object.keys(device).forEach(function(property) {
            deviceString += property + " : " + device[property] + "<br>\n";
        })
    } finally {
        // Dispose of the Match object and return the page.
        device.dispose();
        res.send(deviceString);
    }
})

app.get('/metrics', function(req, res) {
    if (provider.getDataSetName() === "Trie") {
        // Trie does not support match metrics.
        res.send("Match metrics not available in Trie.<br>\n");
    } else {
        var metricsString = '';
        try {
            // Get a match.
            var device = new provider.getMatchForRequest(req);

            // Print the match metrics.
            metricsString += "Device Id : " + device.getDeviceId() + "<br>\n";
            metricsString += "Method : " + device.getMethod() + "<br>\n";
            metricsString += "Difference : " + device.getDifference() + "<br>\n";
            metricsString += "Rank : " + device.getRank() + "<br>\n";
        } finally {
            // Dispose of the match object and return the page.
            device.dispose();
            res.send(metricsString);
        }
    }
})

app.listen(3000, function() {
    console.log("Test express server listening on port 3000");
});
