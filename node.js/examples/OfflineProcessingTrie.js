// Include 51Degrees.
var FiftyOneDegrees = require("../FiftyOneDegreesV3");
// Inlcude file modules.
var fs = require("fs"),
    readline = require("readline");

// Set the config.
var config = {"dataFile" : "../../data/51Degrees-LiteV3.2.trie",
 "properties" : "IsMobile,PlatformName,PlatformVersion"
};

var inputFile = "../../data/20000 User Agents.csv";
var outputFile = "offlineProcessingOutput.csv";

var outputOfflineProcessing = function() {
    fs.writeFile(outputFile, "");
    fs.appendFile(outputFile, "User-Agent");
    provider.availableProperties.forEach(function(property) {
        fs.appendFile(outputFile, "|" + property);
    })
    fs.appendFile(outputFile, "\n");
    
    var instream = fs.createReadStream(inputFile),
        rl = readline.createInterface(instream, null);
    var i = 0;
    rl.on('line', function (userAgent) {
        if (i < 20) {
            try {
                fs.appendFile(outputFile, userAgent);
                var match = provider.getMatch(userAgent);
                provider.availableProperties.forEach(function(property) {
                    fs.appendFile(outputFile, "|" + match.getValue(property));
                })
            } finally {
                fs.appendFile(outputFile, "\n");
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
console.log("Output written to " + outputFile);
