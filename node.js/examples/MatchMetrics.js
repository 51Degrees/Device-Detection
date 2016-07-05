// Include 51Degrees.
var FiftyOneDegrees = require("../pattern/FiftyOneDegreesPatternV3");

// Set the config.
var config = {"dataFile" : "../../data/51Degrees-LiteV3.2.dat",
 "properties" : "IsMobile",
 "cacheSize" : 10000,
 "poolSize" : 4
};

// outputMetrics function. Takes a match object as an argument and
// prints the match metrics associated with it.
var outputMetrics = function(match) {
    console.log("   Id: " + match.getDeviceId());
    console.log("   Match Method: " + match.getMethod());
    console.log("   Difference: " + match.getDifference());
    console.log("   Rank: " + match.getRank());
}

// User-Agent string of an iPhone mobile device.
var mobileUserAgent = "Mozilla/5.0 (iPhone; CPU iPhone OS 7_1 like Mac OS X) AppleWebKit/537.51.2 (KHTML, like Gecko) 'Version/7.0 Mobile/11D167 Safari/9537.53";

// User-Agent string of Firefox Web browser version 41 on desktop.
var desktopUserAgent = "Mozilla/5.0 (Windows NT 6.3; WOW64; rv:41.0) Gecko/20100101 Firefox/41.0";

// User-Agent string of a MediaHub device.
var mediaHubUserAgent = "Mozilla/5.0 (Linux; Android 4.4.2; X7 Quad Core Build/KOT49H) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/30.0.0.0 Safari/537.36";

console.log("Starting Match Metrics Example.\n");

// Initialise a new Provider.
var provider = new FiftyOneDegrees.provider(config);

// Carries out a match with a mobile User-Agent.
try {
    console.log("Mobile User-Agent: " + mobileUserAgent);
    var match = provider.getMatch(mobileUserAgent);
    outputMetrics(match);
} finally {
    match.dispose();
}

// Carries out a match with a desktop User-Agent.
try {
    console.log("Desktop User-Agent: " + desktopUserAgent);
    var match = provider.getMatch(desktopUserAgent);
    outputMetrics(match);
} finally {
    match.dispose();
}

// Carries out a match with a MediaHub User-Agent.
try {
    console.log("Media Hub User-Agent: " + mediaHubUserAgent);
    var match = provider.getMatch(mediaHubUserAgent);
    outputMetrics(match);
} finally {
    match.dispose();
}