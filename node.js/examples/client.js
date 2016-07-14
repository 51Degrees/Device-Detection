var http = require("http");
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

http.createServer(function (req, res) {
    try {
        // Get a match.
        var match = new provider.getMatchForRequest(req);
        // Print the type of device.
        if (match.getValue("IsMobile") === "True") {
            res.write("This is a mobile device.\n");
        } else {
            res.write("This is a non-mobile device.\n");
        }
        // Print all the properties for the device.
        res.write("Here are all its properties:\n\n");
        provider.availableProperties.forEach(function(property) {
            var values = match.getValues(property);
            // Some properties can have multiple values, so get them all.
            for (var i = 0; i < values.size(); i++) {
                
                res.write(property + " : " + values.get(i) + "\n");
            }
        })
    } finally {
        // Dispose of the Match object/
        match.dispose();
    }
    res.end();
}).listen(3000, function() {
    console.log("Test HTTP server listening on port 3000");
});
