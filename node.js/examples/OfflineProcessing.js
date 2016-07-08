// Include 51Degrees.
var FiftyOneDegrees = require("../FiftyOneDegreesV3");
// Inlcude file modules.
var fs = require("fs"),
    readline = require("readline");

// Set the config.
var config = {"dataFile" : "../../data/51Degrees-LiteV3.2.dat",
 "properties" : "IsMobile,PlatformName,PlatformVersion",
 "cacheSize" : 10000,
 "poolSize" : 4
};

var inputFile = "../../data/20000 User Agents.csv";
var outputFile = "offlineProcessingOutput.csv";

var outputOfflineProcessing = function() {
    var inStream = fs.createReadStream(inputFile),
        rl = readline.createInterface(inStream, null),
        outStream = fs.createWriteStream(outputFile);
    inStream.on("close", function() {
        outStream.close();
        console.log("Output written to " + outputFile);
    })

    outStream.write("User-Agent");
    provider.availableProperties.forEach(function(property) {
        outStream.write("|" + property);
    })
    outStream.write("\n");

    var i = 0;
    rl.on('line', function (userAgent) {
        if (i < 20) {
            try {
                outStream.write(userAgent);
                var match = provider.getMatch(userAgent);
                provider.availableProperties.forEach(function(property) {
                    outStream.write("|" + match.getValue(property));
                })
            } finally {
                outStream.write("\n");
                match.dispose();
            }
        }
        i++;
    });
}

console.log("Starting Offline Processing Example.\n");

// Initialise a new Provider.
var provider = new FiftyOneDegrees.provider(config);
outputOfflineProcessing();