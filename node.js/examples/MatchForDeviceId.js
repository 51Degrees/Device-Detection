// Include 51Degrees.
var FiftyOneDegrees = require("../FiftyOneDegreesV3");

// Set the config.
var config = {"dataFile" : "../../data/51Degrees-LiteV3.2.dat",
 "properties" : "IsMobile",
 "cacheSize" : 10000,
 "poolSize" : 4
};

// User-Agent string of an iPhone mobile device.
var mobileUserAgent = "Mozilla/5.0 (iPhone; CPU iPhone OS 7_1 like Mac OS X) AppleWebKit/537.51.2 (KHTML, like Gecko) 'Version/7.0 Mobile/11D167 Safari/9537.53";

// User-Agent string of Firefox Web browser version 41 on desktop.
var desktopUserAgent = "Mozilla/5.0 (Windows NT 6.3; WOW64; rv:41.0) Gecko/20100101 Firefox/41.0";

// User-Agent string of a MediaHub device.
var mediaHubUserAgent = "Mozilla/5.0 (Linux; Android 4.4.2; X7 Quad Core Build/KOT49H) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/30.0.0.0 Safari/537.36";

// Gets a match for the supplied User-Agent and returns the device id.
var getDeviceId = function(userAgent) {
    try {
        var match = provider.getMatch(userAgent);
        var deviceId = match.getDeviceId();
    }
    finally {
        match.dispose();
        return deviceId;
    }
}
console.log("Starting Match For Device Id Example.");

// Initialise a new Provider.
var provider = new FiftyOneDegrees.provider(config);

// Get the device id for a selection of User-Agents.
var mobileDeviceId = getDeviceId(mobileUserAgent);
var desktopDeviceId = getDeviceId(desktopUserAgent);
var mediaHubDeviceId = getDeviceId(mediaHubUserAgent);

// Carries out a match for a mobile device id.
console.log("\nMobileDeviceId : " + mobileDeviceId);
var match = provider.getMatchForDeviceId(mobileDeviceId);
console.log("   IsMobile: " + match.getValue("IsMobile"));

// Carries out a match for a desktop device id.
console.log("\nDesktopDeviceId : " + desktopDeviceId);
var match = provider.getMatchForDeviceId(desktopDeviceId);
console.log("   IsMobile: " + match.getValue("IsMobile"));

// Carries out a match for a MediaHub device id.
console.log("\nMediaHubDeviceId : " + mediaHubDeviceId);
var match = provider.getMatchForDeviceId(mediaHubDeviceId);
console.log("   IsMobile: " + match.getValue("IsMobile"));