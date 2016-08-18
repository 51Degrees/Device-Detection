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
