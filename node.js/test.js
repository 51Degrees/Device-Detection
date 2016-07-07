var FiftyOneDegrees = require("./FiftyOneDegreesV3.js");

// Read 20000 User Agents.csv into an array
var fs = require("fs"),
    readline = require("readline"),
    inputFile = "../data/20000 User Agents.csv",
    userAgents = new Array(20000),
    instream = fs.createReadStream(inputFile),
    rl = readline.createInterface(instream, null),
    i = 0;
rl.on('line', function (userAgent) {
    if (i < 20000) {
        userAgents[i] = userAgent;
    }
    i++;
})

var mobileUserAgent = "Mozilla/5.0 (Linux; Android 5.1; HTC One M9 Build/LMY47O) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/47.0.2526.83 Mobile Safari/537.36";


// Set either a pattern or trie config.
if (process.argv[3] === "--pattern") {
    var config = {"dataFile" : "../data/51Degrees-LiteV3.2.dat",
     "properties" : "IsMobile,BrowserName",
     "cacheSize" : 10000,
     "poolSize" : 4
    };
    console.log("Creating Pattern provider...");
} else if (process.argv[3] === "--trie") {
    var config = {"dataFile" : "../data/51Degrees-LiteV3.2.trie",
    "properties" : "IsMobile,BrowserName"
    };
    console.log("Creating Trie provider...");
} else {
    console.log(process.argv[3] + " is not a valid argument, use --pattern or --trie.");
    process.exit();
}

// Initialise the provider.
var provider = new FiftyOneDegrees.provider(config);

var assert = require("assert");
describe("Matching", function() {
    describe("Mobile User-Agent", function() {
        it("Should be matched as a mobile device", function() {
                var match = provider.getMatch(mobileUserAgent);
                assert.equal("True", match.getValue("IsMobile"), "IsMobile property was " + match.getValue("IsMobile"));
                match.dispose();
        })
    })

    describe("All Available Properties", function () {
        it("Should return valid properties for all User-Agents", function() {
            userAgents.forEach(function(userAgent) {
                var match = provider.getMatch(userAgent);
                assert.equal(true, match !== undefined, "Match object was undefined for User-Agent:" + userAgent);
                provider.availableProperties.forEach(function(property) {
                    assert.equal(true, match.getValue(property).length > 0, "Value of " + property + " returned was empty");
                });
                match.dispose();
            })
        })
    })
    
    describe("Unavailable Property", function() {
        it ("Should return no value for all User-Agents", function() {
            userAgents.forEach(function(userAgent) {
                var match = provider.getMatch(userAgent);
                assert.equal(true, match !== undefined, "Match object was undefined for User-Agent:" + userAgent);
                assert.equal(true, match.getValue("notaproperty").length === 0, "Match returned " + match.getValue("notaproperty") + " for nonexistant property");
                match.dispose();
            })
        })
    })
})
describe("Performance", function() {
    describe("Startup", function() {
        it("Should startup in <1.5s", function() {
            var start = new Date();
            var provider = new FiftyOneDegrees.provider(config);
            var end = new Date();
            var timeTaken = end - start;
            assert.equal(true, timeTaken < 150, "Startup time was " + timeTaken);
        })
    })
    
    var matchAllUserAgents = function(localProvider) {
        userAgents.forEach(function(userAgent) {
                var match = localProvider.getMatch(userAgent);
                assert.equal(true, match !== undefined, "Match object was undefined for User-Agent:" + userAgent);
                match.dispose();
            })
    }
    
    var baseTime = 0;
    describe("Detection Speed", function () {
        it("Should take < 0.01ms per detection", function() {
            var start = new Date()
            matchAllUserAgents(provider);
            var end = new Date();
            var timeTaken = end - start;
            baseTime = timeTaken;
            var timePerDetection = timeTaken / userAgents.length;
            var detectionsPerSecond = 1000 / timePerDetection;
            assert.equal(true, timePerDetection < 0.1, "Time per detection was " + timePerDetection + " ms");
        })
    })
    
    describe("Reload Penalty", function() {
        it("Should reload without costing more than 1s per 20000 matches", function() {
            var numberOfReloads = 5;
            // Use an independant provider.
            var reloadingProvider = new FiftyOneDegrees.provider(config);
            var start = new Date();
            for(var i = 0; i < numberOfReloads; i++) {
            matchAllUserAgents(reloadingProvider);
            reloadingProvider.reloadFromFile();
            }
            var end = new Date();
            var timeTaken = end - start;
            var difference = timeTaken - baseTime;
            var reloadPenalty = difference / numberOfReloads;
            assert.equal(true, reloadPenalty < 1000, "Time penalty for reloading was " + reloadPenalty);
        }).timeout(10000);
    })
})