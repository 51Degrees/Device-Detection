var eventEmitter = require("events");
var fs = require("fs");
var path = require("path");

// 51Degrees object to return.
var FiftyOneDegrees = {};

// Return the Provider object initialised with the supplied config file.
FiftyOneDegrees.provider = function (configuration) {
    // Create the logger.
    FiftyOneDegrees.log = new eventEmitter();

    // todo this logging method is temporary.
    FiftyOneDegrees.log.on("51error", function(err) {
        console.log(err)
    })

    // Parse the configuration if neccessary.
    if (typeof(configuration) === "string") {
        var rawConfig = fs.readFileSync(configuration, "UTF8"),
            config = JSON.parse(rawConfig)
    }
    else if (typeof(configuration) === "object") {
        var config = configuration;
    }

    var FODcore,
        returnedProvider;
    // Require the correct 51Degrees node library by looking at the data
    // file extension.
    if (path.parse(config.dataFile).ext === ".dat") {
        config.Type = "BinaryV32";
        FODcore = require(__dirname + '/pattern/build/Release/FiftyOneDegreesPatternV3');
    } else if (path.parse(config.dataFile).ext === ".trie") {
        config.Type = "Trie";
        FODcore = require(__dirname + '/trie/build/Release/FiftyOneDegreesTrieV3');
    } else {
        // Throw an error if neither Pattern or Trie are specified.
        FiftyOneDegrees.log.Emit("Invalid data file format" + config.dataFile);
        throw "Invalid data file format " + config.dataFile;
    }

    // Initialise the Provider. Account for all variations here as the node SWIG interface
    // treats undefined as a value.
    try {
        if (config.properties) {
            if (config.cacheSize && config.poolSize) {
                returnedProvider = new FODcore.Provider(config.dataFile, config.properties.toString(), config.cacheSize, config.poolSize);
            } else {
                returnedProvider = new FODcore.Provider(config.dataFile, config.properties);
            }
        } else {
            if (config.cacheSize && config.poolSize) {
                returnedProvider = new FODcore.Provider(config.dataFile, config.cacheSize, config.poolSize);
            } else {
                returnedProvider = new FODcore.Provider(config.dataFile);
            }
        }
    } catch (err) {
        //TODO Add in proper error code.
        FiftyOneDegrees.log.Emit(err);
        throw err;
    }

    // Get the important headers from the data set, this is used when matching with HTTP headers accounting
    // the case of the header names.
    returnedProvider.getHttpHeadersLower = function () {
        var i;
        var importantHeaders = {};
        for (i = 0; i < returnedProvider.getHttpHeaders().size(); i++) {
            var currentHeader = returnedProvider.getHttpHeaders().get(i);
            importantHeaders[returnedProvider.getHttpHeaders().get(i).toLowerCase()] = returnedProvider.getHttpHeaders().get(i);
        }
        return importantHeaders;
    }
    var importantHeaders = returnedProvider.getHttpHeadersLower();

    // Expose the config for extrernal use.
    returnedProvider.config = config;

    // Wrapper function to ensure matching with HTTP headers uses the correct native function.
    returnedProvider.getMatchForHttpHeaders = function (headers) {
        var headersMap = new FODcore.MapStringString();
        Object.keys(headers).forEach(function (key) {
                Object.keys(importantHeaders).forEach(function (lowerKey) {
                    if (lowerKey === key.toLowerCase()) {
                        headersMap.set(importantHeaders[lowerKey], headers[key]);
                    }
                })
            })
        // Return the Match object.
        return returnedProvider.getMatch(headersMap);
    }

    // Wrapper function for matching HTML requests.
    returnedProvider.getMatchForRequest = function(req) {
        if (config.UsageSharingEnabled) {

            // Share usage is enabled, so record the device.
            shareUsage.recordNewDevice(req);
        }

        // Return the Match object.
        return returnedProvider.getMatchForHttpHeaders(req.headers);
    }    

    // Copy the available propeties to a node array to be more easily available.
    var nativeAvailableProperties = returnedProvider.getAvailableProperties();
    returnedProvider.availableProperties = new Array(nativeAvailableProperties.size());
    for (var i = 0; i < nativeAvailableProperties.size(); i++) {
        returnedProvider.availableProperties[i] = nativeAvailableProperties.get(i);
    }

    // Start the auto update process in the background.
    if (config.Licence) {
        require(__dirname + "/update")(returnedProvider, FiftyOneDegrees);
    }

    // Start the share usage process.
    if (config.UsageSharingEnabled) {
        shareUsage = require(__dirname + "/shareUsage")(returnedProvider, FiftyOneDegrees);
    }

    return returnedProvider;
}

module.exports = FiftyOneDegrees;