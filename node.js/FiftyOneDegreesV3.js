var eventEmitter = require("events");
var fs = require("fs");
var path = require("path");

// 51Degrees object to return.
var FiftyOneDegrees = {};

// Create the logger.
FiftyOneDegrees.log = new eventEmitter();

// Listen for unchecked errors.
FiftyOneDegrees.log.on('error', function (err) {
    //todo prople should listen to for errors
})

// Return the Provider object initialised with the supplied config file.
FiftyOneDegrees.provider = function (configuration) {
    // Read the configuration.
    if (typeof (configuration) === "string") {
        // The configuration is a string so parse it.
        var rawConfig = fs.readFileSync(configuration, "UTF8"),
            config = JSON.parse(rawConfig)
    }
    else if (typeof (configuration) === "object") {
        // The configuration is already a json object,so there is no
        // need to do anything.
        var config = configuration;
    }

    // The core 51Degrees library and the provider that will be returned
    // from the call to this function.
    var FODcore,
        returnedProvider;

    // Set the default data file if not supplied.
    if (!config.dataFile) {
        config.dataFile = __dirname + '/data/51Degrees-LiteV3.2.dat';
    }

    // Load the correct 51Degrees node library by looking at the data
    // file extension.
    if (path.parse(config.dataFile).ext === ".dat") {
        // The data file is a Pattern data file, so use the Pattern library
        // and set the type for auto updating.
        config.Type = "BinaryV32";
        FODcore = require(__dirname + '/build/Release/FiftyOneDegreesPatternV3');
    }
    else if (path.parse(config.dataFile).ext === ".trie") {
        // The data file is a Trie data file, so use the Trie library and set
        // the type for auto updating.
        config.Type = "Trie";
        FODcore = require(__dirname + '/build/Release/FiftyOneDegreesTrieV3');
    }
    else {
        // The file does not have the correct extension, so return null.
        throw "Error: Invalid data file extension " + config.dataFile;
    }

    // Initialise the Provider. Account for all variations here as the node SWIG interface
    // treats undefined as a value.
    FiftyOneDegrees.log.emit('info', 'Creating provider from data file ' + config.dataFile);
    if (config.properties) {
        if (config.cacheSize && config.poolSize) {
            returnedProvider = new FODcore.Provider(config.dataFile, config.properties.toString(), config.cacheSize, config.poolSize);
        }
        else {
            returnedProvider = new FODcore.Provider(config.dataFile, config.properties);
        }
    }
    else {
        if (config.cacheSize && config.poolSize) {
            returnedProvider = new FODcore.Provider(config.dataFile, config.cacheSize, config.poolSize);
        }
        else {
            returnedProvider = new FODcore.Provider(config.dataFile);
        }
    }

    // The provider has been successfully created, so say so.
    FiftyOneDegrees.log.emit('info', 'Created provider from data file ' + config.dataFile);

    // Get the important headers from the data set, this is used when matching
    // with HTTP headers accounting the case of the header names.
    returnedProvider.getHttpHeadersLower = function () {
        var importantHeaders = {};
        for (var i = 0; i < returnedProvider.getHttpHeaders().size(); i++) {
            var currentHeader = returnedProvider.getHttpHeaders().get(i);
            importantHeaders[returnedProvider.getHttpHeaders().get(i).toLowerCase()] = returnedProvider.getHttpHeaders().get(i);
        }
        return importantHeaders;
    }

    var defineTypedGetter = function (match, property) {
        // This property has multiple values, so put them in
        // an array.
        match.__defineGetter__(property, function () {
            var values = this.getValues(property);
            if (values.size() > 1) {
                var valuesArray = new Array(values.size());
                for (var i = 0; i < values.size(); i++) {
                    valuesArray[i] = values.get(i);
                }
                return valuesArray;
            }
            else if (values.size() > 0) {
                // This property only has a single property, so just
                // return it, converting to boolean if needed.
                var value = values.get(0);
                if (value === "True") {
                    return true;
                }
                else if (value === "False") {
                    return false;
                }
                else {
                    return value;
                }
            }
            else {
                return undefined;
            }
        })
    }

    var defineNonTypedGetter = function (match, property) {
        // This property has multiple values, so put them in
        // an array.
        match.__defineGetter__(property, function () {
            var values = this.getValues(property);
            if (values.size() > 1) {
                var valuesArray = new Array(values.size());
                for (var i = 0; i < values.size(); i++) {
                    valuesArray[i] = values.get(i);
                }
                return valuesArray;
            }
            else if (values.size() > 0) {
                // This property only has a single property, so just
                // return it.
                return values.get(0);
            }
            else {
                return undefined;
            }
        })
    }

    var setGetters = function (match) {
        // Define getter functions so properties are accessible
        if (returnedProvider.config.addGetters !== false) {
            if (returnedProvider.config.stronglyTyped !== false) {
                var defineGetter = defineTypedGetter;
            }
            else {
                var defineGetter = defineNonTypedGetter;
            }
            returnedProvider.availableProperties.forEach(function (property) {
                if (property.indexOf("JavascriptHardwareProfile") !== -1) {
                    // Skip this property as it will break the API.
                }
                else {
                    // Define the getter for this property.
                    defineGetter(match, property);
                }
            })
            if (returnedProvider.config.Type !== 'Trie') {
                match.__defineGetter__('Id', function () {
                    return this.getDeviceId();
                });
                match.__defineGetter__('Rank', function () {
                    return this.getRank();
                });
                match.__defineGetter__('Difference', function () {
                    return this.getDifference();
                });
                match.__defineGetter__('Method', function () {
                    return this.getMethod();
                })
            }
        }
    }

    if (config.Type !== 'Trie') {
        var findProfiles = returnedProvider.findProfiles;
        returnedProvider.findProfiles = function (property, value) {
            var profiles = findProfiles.apply(this, arguments);
            profiles.__defineGetter__('count', function () {
                return this.getCount();
            })
            profiles.getMatch = function (index) {
                var id = this.getProfileId(index);
                if (id >= 0) {
                    return returnedProvider.getMatchForDeviceId(id.toString());
                }
                else {
                    return undefined;
                }
            }
            return profiles;
        }
    }

    var getHeadersMap = function (headers) {
        // Create a new string-string map to use for the match.
        var headersMap = new FODcore.MapStringString();

        // Find the important headers and add them to the map.
        Object.keys(headers).forEach(function (key) {
            Object.keys(importantHeaders).forEach(function (lowerKey) {
                if (lowerKey === key.toLowerCase()) {
                    // This header is important, so add it to the map.
                    headersMap.set(importantHeaders[lowerKey], headers[key]);
                }
            })
        })
        return headersMap;
    }

    var getMatchOld = returnedProvider.getMatch;
    returnedProvider.getMatch = function () {
        var match;
        if (arguments && arguments[0]) {
            if (typeof (arguments[0]) === 'string') {
                match = getMatchOld.apply(this, arguments);
            }
            else if (arguments[0].headers) {
                var req = arguments[0];
                if (config.UsageSharingEnabled !== false) {
                    // Share usage is enabled, so record the device.
                    shareUsage.recordNewDevice(req);
                }

                // Get a Match object using the headers from the supplied request.
                if (req.device) {
                    req.device.close();
                }

                var headersMap = getHeadersMap(req.headers);

                req.device = getMatchOld.apply(this, [headersMap]);

                req.on('end', function () {
                    if (this.device) {
                        this.device.close()
                        delete this.device;
                    }
                })
                req.on('abort', function () {
                    if (this.device) {
                        this.device.close()
                        delete this.device;
                    }
                })
                req.on('aborted', function () {
                    if (this.device) {
                        this.device.close()
                        delete this.device;
                    }
                })
                match = req.device;
            }
            else {
                var headers = arguments[0];
                var headersMap = getHeadersMap(headers);
                match = getMatchOld.apply(this, [headersMap]);
            }
        }
        if (match) {
            setGetters(match);
        }
        return match;
    }

    getMatchForDeviceId = returnedProvider.getMatchForDeviceId;
    returnedProvider.getMatchForDeviceId = function (deviceId) {
        var match = getMatchForDeviceId.apply(this, arguments);
        setGetters(match);
        return match;
    }

    // Store the importand headers for use by the getMatchForHttpHeaders
    // function.
    var importantHeaders = returnedProvider.getHttpHeadersLower();
    FiftyOneDegrees.log.emit('debug', 'Set the important headers')

    // Expose the config for extrernal use.
    returnedProvider.config = config;

    // Copy the available propeties to a node array to be more easily
    // available outside the module.
    var nativeAvailableProperties = returnedProvider.getAvailableProperties();
    returnedProvider.availableProperties = new Array(nativeAvailableProperties.size());
    for (var i = 0; i < nativeAvailableProperties.size(); i++) {
        returnedProvider.availableProperties[i] = nativeAvailableProperties.get(i);
    }
    FiftyOneDegrees.log.emit('debug', 'Got the availbale properties')

    // Start the auto update process in the background.
    if (config.Licence) {
        FiftyOneDegrees.log.emit('info', 'Starting auto updater');
        require(__dirname + "/update")(returnedProvider, FiftyOneDegrees);
    }

    // Start the share usage process.
    if (config.UsageSharingEnabled !== false) {
        FiftyOneDegrees.log.emit('info', 'Starting usage sharer');
        shareUsage = require(__dirname + "/shareUsage")(returnedProvider, FiftyOneDegrees);
    }

    // Return the provider object just created in this function.
    return returnedProvider
}

// Export the FiftyOneDegrees object.
module.exports = FiftyOneDegrees;