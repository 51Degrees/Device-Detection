/* *********************************************************************
 * This Source Code Form is copyright of 51Degrees Mobile Experts Limited.
 * Copyright 2017 51Degrees Mobile Experts Limited, 5 Charlotte Close,
 * Caversham, Reading, Berkshire, United Kingdom RG4 7BY
 *
 * This Source Code Form is the subject of the following patents and patent
 * applications, owned by 51Degrees Mobile Experts Limited of 5 Charlotte
 * Close, Caversham, Reading, Berkshire, United Kingdom RG4 7BY:
 * European Patent No. 2871816;
 * European Patent Application No. 17184134.9;
 * United States Patent Nos. 9,332,086 and 9,350,823; and
 * United States Patent Application No. 15/686,066.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0.
 *
 * If a copy of the MPL was not distributed with this file, You can obtain
 * one at http://mozilla.org/MPL/2.0/.
 *
 * This Source Code Form is "Incompatible With Secondary Licenses", as
 * defined by the Mozilla Public License, v. 2.0.
 ********************************************************************** */
var eventEmitter = require("events");
var fs = require("fs");
var path = require("path");
// 51Degrees object to return.
var FiftyOneDegrees = {};
var providerId = 1;
// Create the logger.
FiftyOneDegrees.log = new eventEmitter();
// Catch and print errors if there is no listener.
FiftyOneDegrees.log.on('error', function (err) {
    if (FiftyOneDegrees.log.listenerCount('error') <= 1) {
        console.log(err);
    }
});
// Return the Provider object initialised with the supplied config file.
FiftyOneDegrees.provider = function (configuration) {
    // Read the configuration.
    var config;
    if (typeof (configuration) === "string") {
        // The configuration is a string so parse it.
        var rawConfig = fs.readFileSync(configuration, "UTF8"),
            config = JSON.parse(rawConfig);
    }
    else if (typeof (configuration) === "object") {
        // The configuration is already a json object,so there is no
        // need to do anything.
        config = configuration;
    }
    // The core 51Degrees library and the provider that will be returned
    // from the call to this function.
    var FODcore, returnedProvider;
    var updater = undefined;
    var usageSharer = undefined;
    // Set the default data file if not supplied.
    if (!config.dataFile) {
        config.dataFile = require('fiftyonedegreeslitepattern');
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
        config.Type = "HashTrieV34";
        FODcore = require(__dirname + '/build/Release/FiftyOneDegreesTrieV3');
    }
    else {
        // The file does not have the correct extension, so return null.
        FiftyOneDegrees.log.emit('error', "Error: Invalid data file extension " + config.dataFile);
        return null;
    }
    // Initialise the Provider. Account for all variations here as the node SWIG interface
    // treats undefined as a value. If undefined is entered it will be passed as a value
    // so the constructor with the maximum arguments will be called and break. This block
    // Fixes that.
    FiftyOneDegrees.log.emit('info', 'Creating provider [' + providerId + '] from data file ' + config.dataFile);
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
        FiftyOneDegrees.log.emit('error', err);
    }
    // The provider has been successfully created, so say so.
    FiftyOneDegrees.log.emit('info', 'Created provider [' + providerId + '] from data file ' + config.dataFile);
    // Set the provider's id so the log is readable with multiple
    // providers.
    returnedProvider.Id = providerId;
    providerId++;
    // Get the important headers from the data set, this is used when matching
    // with HTTP headers accounting the case of the header names.
    // Note: these are converted to lowercase to match how headers are named
    // in node.
    returnedProvider.getHttpHeadersLower = function () {
        var importantHeaders = {};
        for (var i = 0; i < returnedProvider.getHttpHeaders().size(); i++) {
            var currentHeader = returnedProvider.getHttpHeaders().get(i);
            importantHeaders[returnedProvider.getHttpHeaders().get(i).toLowerCase()] = returnedProvider.getHttpHeaders().get(i);
        }
        return importantHeaders;
    };
    // Define a getter for the property where 'True' and 'False' strings are
    // converted to boolean. Getters are set without getting the value so no
    // unneccessary computation is carried out.
    var defineTypedGetter = function (match, property) {
        Object.defineProperty(match, property, {
            get: function () {
                // Get the property values from the this match object.
                var values = this.getValues(property);
                if (values.size() > 1) {
                    // This property has multiple values, so put them in
                    // an array.
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
            }
        });
    };
    // Define a getter for the property where 'True' and 'False' strings are
    // not converted to boolean. Getters are set without getting the value so
    // no unneccessary computation is carried out.
    var defineNonTypedGetter = function (match, property) {
        Object.defineProperty(match, property, {
            get: function () {
                // Get the property values from this match object.
                var values = this.getValues(property);
                if (values.size() > 1) {
                    // This property has multiple values, so put them in
                    // an array.
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
            }
        });
    };
    // Go through all the available properties and set the getters for them.
    // This makes returning arrays much simpler, see define*Getter functions
    // to see how it is done.
    // Getters are set without getting the value so no unneccessary computation
    // is carried out.
    var defineGetter;
    var setGetters = function (match) {
        // Define getter functions so properties are accessible
        // addGetters and stronglyTyped are both checked inside this
        // function so they are changable at runtime.
        if (returnedProvider.config.addGetters !== false) {
            if (returnedProvider.config.stronglyTyped !== false) {
                // Convert boolean values.
                defineGetter = defineTypedGetter;
            }
            else {
                // Don't convert boolean values.
                defineGetter = defineNonTypedGetter;
            }
            // Loop through all the properties the provider has made available.
            returnedProvider.availableProperties.forEach(function (property) {
                // Define the getter for this property.
                defineGetter(match, property);
            });
            // If the API is Pattern then set the match metrics getters.
            if (returnedProvider.config.Type !== 'HashTrieV34') {
                Object.defineProperty(match, 'Id', {
                    get: function () {
                        return this.getDeviceId();
                    }
                });
                Object.defineProperty(match, 'Rank', {
                    get: function () {
                        return this.getRank();
                    }
                });
                Object.defineProperty(match, 'Difference', {
                    get: function () {
                        return this.getDifference();
                    }
                });
                Object.defineProperty(match, 'Method', {
                    get: function () {
                        return this.getMethod();
                    }
                });
            }
        }
    };
    if (config.Type !== 'HashTrieV34') {
        // Wrap the find profiles function to make it more node friendly.
        var nativeFindProfiles = returnedProvider.findProfiles;
        returnedProvider.findProfiles = function (property, value) {
            // Get the profiles object from the native function.
            var profiles = nativeFindProfiles.apply(this, arguments);
            // Add a count getter which calls getCount().
            Object.defineProperty(profiles, 'count', {
                get: function () {
                    return this.getCount();
                }
            });
            // Add a getMatch function so a match can be returned from a
            // profile.
            profiles.getMatch = function (index) {
                var id = this.getProfileId(index);
                if (id >= 0) {
                    return returnedProvider.getMatchForDeviceId(id.toString());
                }
                else {
                    return undefined;
                }
            };
            // Return the profiles object.
            return profiles;
        };
    }
    // Converts a headers JSON object to as MapStringString to be passed
    // into the getMatch function.
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
            });
        });
        return headersMap;
    };
    // Override the getMatch function to ensure the correct mapping of argument
    // types.
    var getMatchNative = returnedProvider.getMatch;
    returnedProvider.getMatch = function () {
        var match;
        if (arguments && arguments[0]) {
            if (typeof (arguments[0]) === 'string') {
                // The argument is just a User-Agent so call the native
                // match function.
                match = getMatchNative.apply(this, arguments);
            }
            else if (arguments[0].headers) {
                // The argument has a headers property, so it must be an HTTP
                // request.
                var req = arguments[0];
                if (config.UsageSharingEnabled !== false) {
                    // Share usage is enabled, so record the device.
                    usageSharer.recordNewDevice(req);
                }
                // If the request object already has a match attached,
                // close it.
                if (req.fiftyoneDevice) {
                    req.fiftyoneDevice.close();
                }
                // Map the headers to the correct type.
                var headersMap = getHeadersMap(req.headers);
                // Get a match for the headers and attach it to the request
                // object.
                req.fiftyoneDevice = getMatchNative.apply(this, [headersMap]);
                // Attach the close method to request events so that whatever
                // happens, the match will be released to the workset pool.
                req.on('end', function () {
                    if (this.fiftyoneDevice) {
                        this.fiftyoneDevice.close();
                        delete this.fiftyoneDevice;
                    }
                });
                req.on('abort', function () {
                    if (this.fiftyoneDevice) {
                        this.fiftyoneDevice.close();
                        delete this.fiftyoneDevice;
                    }
                });
                req.on('aborted', function () {
                    if (this.fiftyoneDevice) {
                        this.fiftyoneDevice.close();
                        delete this.fiftyoneDevice;
                    }
                });
                // Set the match object so it can be returned as well as
                // set in req.
                match = req.fiftyoneDevice;
            }
            else {
                // The argument is not a string and does not have a headers
                // property, so it must be a headers object.
                var headers = arguments[0];
                // Map the headers to the correct type.
                var headersMap = getHeadersMap(headers);
                // Get a match for the HTTP headers.
                match = getMatchNative.apply(this, [headersMap]);
            }
        }
        if (match) {
            // Try to set the getters if config.setGetters is set to true.
            setGetters(match);
        }
        // Return the match object.
        return match;
    };
    // Wrap the native getMatchForDeviceId function so getters can be set.
    var getMatchForDeviceId = returnedProvider.getMatchForDeviceId;
    returnedProvider.getMatchForDeviceId = function (deviceId) {
        // Get a match for the device id.
        var match = getMatchForDeviceId.apply(this, arguments);
        // Try to set the getters if config.setGetters is set to true.
        setGetters(match);
        // Return the match object.
        return match;
    };
    // Store the important headers for use by the getMatch function.
    var importantHeaders = returnedProvider.getHttpHeadersLower();
    FiftyOneDegrees.log.emit('debug', '[' + returnedProvider.Id + '] Set the important headers');
    // Expose the config for extrernal use.
    returnedProvider.config = config;
    // Copy the available propeties to a node array to be more easily
    // available outside the module.
    var nativeAvailableProperties = returnedProvider.getAvailableProperties();
    returnedProvider.availableProperties = new Array(nativeAvailableProperties.size());
    // For all the available properties add them to the array.
    for (var i = 0; i < nativeAvailableProperties.size(); i++) {
        returnedProvider.availableProperties[i] = nativeAvailableProperties.get(i);
    }
    FiftyOneDegrees.log.emit('debug', '[' + returnedProvider.Id + '] Got the availbale properties');
    // Start the auto update process in the background.
    if (config.License) {
        FiftyOneDegrees.log.emit('info', '[' + returnedProvider.Id + '] Starting auto updater');
        updater = require(__dirname + "/update")(returnedProvider, FiftyOneDegrees);
    }
    // Start the share usage process.
    if (config.UsageSharingEnabled !== false) {
        FiftyOneDegrees.log.emit('info', '[' + returnedProvider.Id + '] Starting usage sharer');
        usageSharer = require(__dirname + "/shareUsage")(returnedProvider, FiftyOneDegrees);
    }
    
    returnedProvider.close = function() {
        FiftyOneDegrees.log.emit('info', '[' + returnedProvider.Id + '] Closing provider');
        if (updater !== undefined) {
            FiftyOneDegrees.log.emit('info', '[' + returnedProvider.Id + '] Ending auto updater');
            clearInterval(updater);
        }
    };
    
    // Return the provider object just created in this function.
    return returnedProvider;
};
// Export the FiftyOneDegrees object.
module.exports = FiftyOneDegrees;