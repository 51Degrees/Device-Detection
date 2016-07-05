var http = require("http");
var FiftyOneDegrees = require("../FiftyOneDegreesV3");

// Initialise a new Provider.
var provider = new FiftyOneDegrees.provider("../trie/config.json");

http.createServer(function (req, res) {
    try {
        // Get a match.
        var match = new provider.getMatchForHttpHeaders(req.headers);
        provider.availableProperties.forEach(function(value) {
            res.write(value + " : " + match.getValue(value) + "\n");
        })        
    } finally {
        // Dispose of the Match object/
        match.dispose();
    }
    res.end();
}).listen(3000);
