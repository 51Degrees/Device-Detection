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
    if (i < 10000) {
        userAgents[i] = userAgent;
    }
    i++;
})

// User-Agent string of an iPhone mobile device.
var mobileUserAgent = "Mozilla/5.0 (iPhone; CPU iPhone OS 7_1 like Mac OS X) AppleWebKit/537.51.2 (KHTML, like Gecko) 'Version/7.0 Mobile/11D167 Safari/9537.53";

// User-Agent string of Firefox Web browser version 41 on desktop.
var desktopUserAgent = "Mozilla/5.0 (Windows NT 6.3; WOW64; rv:41.0) Gecko/20100101 Firefox/41.0";

// User-Agent string of a MediaHub device.
var mediaHubUserAgent = "Mozilla/5.0 (Linux; Android 4.4.2; X7 Quad Core Build/KOT49H) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/30.0.0.0 Safari/537.36";


// Set either a pattern or trie config.
if (process.argv[3] === "--pattern") {
    var config = {"dataFile" : "../data/51Degrees-LiteV3.2.dat",
     "properties" : "IsMobile,BrowserName",
     "cacheSize" : 20000,
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
    
    describe("Desktop User-Agent", function() {
        it("Should be matched as a non-mobile device", function() {
        var match = provider.getMatch(desktopUserAgent);
        assert.equal("False", match.getValue("IsMobile"), "IsMobile property was " + match.getValue("IsMobile"));
        match.dispose();
        })
    })

    describe("Media Hub User-Agent", function() {
        it("Should be matched as a non-mobile device", function() {
        var match = provider.getMatch(mediaHubUserAgent);
        assert.equal("False", match.getValue("IsMobile"), "IsMobile property was " + match.getValue("IsMobile"));
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
    if(config.cacheSize !== undefined) {
        describe("Detection Speed With Cache", function() {
          it("Should be quicker when fetching matches which are already cached", function() {
              var cacheProvider = new FiftyOneDegrees.provider(config);
              var start = new Date()
              matchAllUserAgents(cacheProvider);
              var end = new Date();
              var timeTaken = end - start;
              
              var start = new Date();
              matchAllUserAgents(cacheProvider);
              var end = new Date();
              var timeTakenCache = end - start;
              assert.equal(true, timeTakenCache < timeTaken, "Detection speed when using the cache are more that without " + timeTakenCache + ">" + timeTaken);
          })       
        })
    }

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