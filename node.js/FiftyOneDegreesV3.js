var eventEmitter = require("events");
var fs = require("fs");
var path = require("path");

// 51Degrees object to return.
var FiftyOneDegrees = {};

// The event logger and available logging levels.
var logError,
    logLevels = ['none', 'error', 'info', 'debug'];

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

    // Create the logger.
    FiftyOneDegrees.log = new eventEmitter();

    // Set the log formatting function.
    if (config.logFile) {
        // A log file has been specified, so log to this.
        logError = function (err) {
            fs.appendFile(config.logFile, new Date() + ' ' + err + '\n');
        }
    }
    else {
        // No log file has been specified, so log to the console.
        logError = function (err) {
            console.log(new Date() + ' ' + err);
        }
    }

    // The logging level has not been defined or is not a valid logging
    // level, so set the default.
    if (logLevels.indexOf(config.logLevel) === -1) {
        config.logLevel = 'info';
    }

    // Set the logging level.
    for (var i = 0; i < logLevels.length; i++) {
        if (config.logLevel === logLevels[0]) {
            // Log level is none, so don't set anything.
            break;
        }
        // Start an event listener for this logging level.
        FiftyOneDegrees.log.on('51' + logLevels[i], function (err) {
            logError(err);
        })
        if (logLevels[i] === config.logLevel) {
            // Do not log any deeper than this.
            break;
        }
    }

    // The core 51Degrees library and the provider that will be returned
    // from the call to this function.
    var FODcore,
        returnedProvider;

    // Load the correct 51Degrees node library by looking at the data
    // file extension.
    if (path.parse(config.dataFile).ext === ".dat") {
        // The data file is a Pattern data file, so use the Pattern library
        // and set the type for auto updating.
        config.Type = "BinaryV32";
        FODcore = require(__dirname + '/pattern/build/Release/FiftyOneDegreesPatternV3');
    }
    else if (path.parse(config.dataFile).ext === ".trie") {
        // The data file is a Trie data file, so use the Trie library and set
        // the type for auto updating.
        config.Type = "Trie";
        FODcore = require(__dirname + '/trie/build/Release/FiftyOneDegreesTrieV3');
    }
    else {
        // The file does not have the correct extension, so return null.
        FiftyOneDegrees.log.emit('51error', "Error: Invalid data file extension " +
            config.dataFile);
        return null;
    }

    // Initialise the Provider. Account for all variations here as the node SWIG interface
    // treats undefined as a value.
    FiftyOneDegrees.log.emit('51info', 'Creating provider from data file ' + config.dataFile);
    try {
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
    }
    catch (err) {
        // Initialisation of the provider failed, so return null.
        FiftyOneDegrees.log.emit('51error', err);
        return null;
    }

    // The provider has been successfully created, so say so.
    FiftyOneDegrees.log.emit('51info', 'Created provider from data file ' + config.dataFile);

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

    // Store the importand headers for use by the getMatchForHttpHeaders
    // function.
    var importantHeaders = returnedProvider.getHttpHeadersLower();
    FiftyOneDegrees.log.emit('51debug', 'Set the important headers')

    // Expose the config for extrernal use.
    returnedProvider.config = config;

    // Wrapper function to ensure matching with HTTP headers uses the
    // correct native function.
    returnedProvider.getMatchForHttpHeaders = function (headers) {
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

        // Return the Match object using the important headers.
        return returnedProvider.getMatch(headersMap);
    }

    // Wrapper function for matching HTML requests.    
    returnedProvider.getMatchForRequest = function (req) {
        if (config.UsageSharingEnabled !== false) {
            // Share usage is enabled, so record the device.
            shareUsage.recordNewDevice(req);
        }

        // Get a Match object using the headers from the supplied request.
        req.match =  returnedProvider.getMatchForHttpHeaders(req.headers);
        
        // Define getter functions so properties are accessible
        if (returnedProvider.config.addGetters !== false) {
            returnedProvider.availableProperties.forEach(function(property) {
                if (property.indexOf("JavascriptHardwareProfile") !== -1) {
                    // Skip this property as it will break the API.
                } else {
                    // Define the getter for this property.
                    req.__defineGetter__(property, function() {
                        // This property has multiple values, so put them in
                        // an array.
                        var values = this.match.getValues(property);
                        if (values.size() > 1) {
                            var valuesArray = new Array(values.size());
                            for (var i = 0; i < values.size(); i++) {
                                valuesArray[i] = values.get(i);
                            }
                            return valuesArray;
                        }
                        else {
                            // This property only has a single property, so just
                            // return it, converting to boolean if needed.
                            var value = values.get(0);
                            if (value === "True") {
                                return true;
                            } else if (value === "False") {
                                return false;
                            } else {
                                return value;
                            }
                        }
                    })
                }
            })
        }
        
        req.on('end', function() {
            if (this.match) {
                this.match.dispose()
                this.match = false
            }
        })
        req.on('abort', function() {
            if (this.match) {
                this.match.dispose()
                this.match = false
            }
        })
        req.on('aborted', function() {
            if (this.match) {
                this.match.dispose()
                this.match = false
            }
        })
        return;
    }

    // Copy the available propeties to a node array to be more easily
    // available outside the module.
    var nativeAvailableProperties = returnedProvider.getAvailableProperties();
    returnedProvider.availableProperties = new Array(nativeAvailableProperties.size());
    for (var i = 0; i < nativeAvailableProperties.size(); i++) {
        returnedProvider.availableProperties[i] = nativeAvailableProperties.get(i);
    }
    FiftyOneDegrees.log.emit('51debug', 'Got the availbale properties')

    // Start the auto update process in the background.
    if (config.Licence) {
        FiftyOneDegrees.log.emit('51info', 'Starting auto updater');
        require(__dirname + "/update")(returnedProvider, FiftyOneDegrees);
    }

    // Start the share usage process.
    if (config.UsageSharingEnabled !== false) {
        FiftyOneDegrees.log.emit('51info', 'Starting usage sharer');
        shareUsage = require(__dirname + "/shareUsage")(returnedProvider, FiftyOneDegrees);
    }

    // Return the provider object just created in this function.
    return returnedProvider;
}

// Export the FiftyOneDegrees object.
module.exports = FiftyOneDegrees;