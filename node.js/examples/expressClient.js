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

app.get('/', function(req, res) {
    var deviceString = '';
    try {
        // Get a match.
        var match = new provider.getMatchForRequest(req);
        // Print the type of device.
        if (match.getValue("IsMobile") === "True") {
            deviceString += "This is a mobile device.<br>\n";
        } else {
            deviceString += "This is a non-mobile device.<br>\n";
        }
        // Print all the properties for the device.
        deviceString += "Here are all its properties:<br><br>\n\n";
        provider.availableProperties.forEach(function(property) {
            var values = match.getValues(property);
            // Some properties can have multiple values, so get them all.
            for (var i = 0; i < values.size(); i++) {
                deviceString += property + " : " + values.get(i) + "<br>\n";
            }
        })
    } finally {
        // Dispose of the Match object and return the page.
        match.dispose();
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
            var match = new provider.getMatchForRequest(req);
            // Print the match metrics.
            metricsString += "Device Id : " + match.getDeviceId() + "<br>\n";
            metricsString += "Method : " + match.getMethod() + "<br>\n";
            metricsString += "Difference : " + match.getDifference() + "<br>\n";
            metricsString += "Rank : " + match.getRank() + "<br>\n";
        } finally {
            // Dispose of the match object and return the page.
            match.dispose();
            res.send(metricsString);
        }
    }
})

app.listen(3000, function() {
    console.log("Test express server listening on port 3000");
});
