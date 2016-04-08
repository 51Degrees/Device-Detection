var http = require("http");
var FiftyOneDegrees = require("./FiftyOneDegreesPatternV3");

// Initialise a new Provider.
var provider = new FiftyOneDegrees.provider("config.json");

http.createServer(function (req, res) {
    try {
        // Get a match.
        var match = new provider.getMatchForHttpHeaders(req.headers);
        
        // Print properties.
        res.write(match.getValue("BrowserName") + "\n");
        res.write(match.getValue("PlatformName") + "\n");
        res.write(match.getValue("DeviceType") + "\n");
        
    } finally {
        // Dispose of the Match object/
        match.dispose();
    }
    res.end();
}).listen(3000);