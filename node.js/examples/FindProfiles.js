// Include 51Degrees.
var FiftyOneDegrees = require("../pattern/FiftyOneDegreesPatternV3");

// Set the config.
var config = {"dataFile" : "../../data/51Degrees-LiteV3.2.dat",
 "properties" : "IsMobile",
 "cacheSize" : 10000,
 "poolSize" : 4
};

console.log("Starting Find Profiles Example.\n");

// Initialise a new Provider.
var provider = new FiftyOneDegrees.provider(config);

// Retrieve all the mobile profiles in the data set.
var mobileProfiles = provider.findProfiles("IsMobile","True");
console.log("There are " + mobileProfiles.getCount() + " mobile profiles in the " + provider.getDataSetName() + " data set.");

// Retrieve all the non-mobile profiles in the data set.
mobileProfiles = provider.findProfiles("IsMobile","False");
console.log("There are " + mobileProfiles.getCount() + " non-mobile profiles in the " + provider.getDataSetName() + " data set.");