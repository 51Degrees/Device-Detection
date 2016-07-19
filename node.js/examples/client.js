var http = require("http");
var FiftyOneDegrees = require("../FiftyOneDegreesV3");

// Initialise a new Provider.
if (process.argv[2] === "pattern") {
    var provider = new FiftyOneDegrees.provider("../pattern/config.json");
    console.log("Started new Pattern provider.")
}
else if (process.argv[2] === "trie") {
    var provider = new FiftyOneDegrees.provider("../trie/config.json");
    console.log("Started new Trie provider.")
}
else {
    var provider = new FiftyOneDegrees.provider("../pattern/config.json");
    console.log("Started new Pattern provider (default).")
}

// Enabling this means booleans and Object.keys can be used when
// getting properties from the device.
provider.config.nodeify = true;

var server = http.createServer(function (req, res) {
    // Get a match.
    provider.getMatchForRequest(req);

    // Print the type of device.
    if (req.IsMobile) {
        res.write("This is a mobile device.\n");
    }
    else {
        res.write("This is a non-mobile device.\n");
    }

    // Print all the properties for the device.
    res.write("Here are all its properties:\n\n");
    provider.availableProperties.forEach(function(property) {
        res.write(property + " : " + req[property] + "\n");
    })
    res.end();
}).listen(3000, function () {
    console.log("Test HTTP server listening on port 3000");
});
