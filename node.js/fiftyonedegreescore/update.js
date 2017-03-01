var fs = require("fs");
var zlib = require("zlib");
var crypto = require("crypto");
var https = require("https");
var dataSetNextUpdateDate;
var querystring = require("querystring");
var updating;
// Default the product name to Lite.
var product = 'Lite';
// Regular expression to check for a valid license key.
var validLicenseRegEx = new RegExp("^[A-Z\\d]+$");
// Update function called when an update is due.
var update = function (provider, errorHandler) {
    // Load config and make the querystring parameter and http request options.
    var config = provider.config,
        parameters = {
            LicenseKeys: config.License,
            Type: config.Type,
            Download: 'True',
            Product: product,
        };
    // Check the license key is not empty
    if (config.License.length === 0) {
        errorHandler('At least one valid license key is required ' + 'to update device data. See https://51degrees.com/' + 'compare-data-options to acquire valid license keys.');
        return false;
    }
    // Check the license key is a valid format.
    if (validLicenseRegEx.exec(config.License) === null) {
        errorHandler('The license key(s) provided were invalid. See ' + 'https://51degrees.com/compare-data-options to acquire ' + 'valid license keys.');
        return false;
    }
    // Set the request options to get the update from.
    var requestOptions = {
        host: "distributor.51degrees.com",
        port: 443,
        path: "/api/v2/download?" + querystring.stringify(parameters)
    };
    var request = https.get(requestOptions);
    // When recieving response, if gzip download file, if not, return error.
    request.on("response", function (response) {
        // If the response code is not 200, then throw an error as the
        // download will not happen.
        if (response.statusCode !== 200) {
            switch (response.statusCode) {
            case 429:
                errorHandler('Too many attempts have been made to ' + 'download a data file from this public IP ' + 'address or with this license key. Try again ' + 'after a period of time.');
                return false;
            case 403:
                errorHandler('Data not downloaded. The license key is not' + 'valid');
                return false;
            default:
                errorHandler('An error occurred fetching the data file. ' + 'Try again incase the error is temporary, ' + 'or validate license key and network ' + 'configuration.');
                return false;
            }
        }
        // If the response is not gzip encoded then return an error.
        if (response.headers["content-encoding"] && response.headers["content-encoding"].indexOf("gzip") === -1) {
            errorHandler("The response encoding was " + response.headers['content-encoding']);
            return false;
        }
        // Set updating flag to true so that another update process does not start.
        updating = true;
        // Stream contents of the gzip file into temp file on disk.
        var zippedOutput = fs.createWriteStream(config.dataFile + ".gz");
        response.pipe(zippedOutput);
        response.on("end", function () {
            // When the stream completes, read zipped file.
            fs.readFile(config.dataFile + ".gz", function (err, zippedFile) {
                if (err) {
                    // There was an error reading the donwloaded file.
                    errorHandler(err);
                    return false;
                }
                // Check the hash of the zipped file against the md5 from the request.
                var hash = crypto.createHash("md5").update(zippedFile).digest("hex");
                if (hash === response.headers["content-md5"]) {
                    // The hashes match, so unzip the file.
                    zlib.unzip(zippedFile, function (err, data) {
                        // Now write it to file.
                        fs.writeFile(config.dataFile, data, function (err) {
                            if (err) {
                                // There was an error writing to file.
                                errorHandler(err);
                                return false;
                            }
                            // If writing to file succedes delete the gzip file.
                            fs.unlink(config.dataFile + ".gz", function (err) {
                                if (err) {
                                    // There was an error deleting the file.
                                    errorHandler(err);
                                }
                                else {
                                    // The file was delted, so return
                                    // without error.
                                    errorHandler();
                                }
                            });
                        });
                    });
                }
                else {
                    // The hashes did not match.
                    errorHandler("Data was downloaded but the MD5 hash failed");
                }
            });
        });
    });
};
module.exports = function (provider, FOD) {
    var config = provider.config;
    // Get the next update date of the data file (only called once on init).
    dataSetNextUpdateDate = new Date(provider.getDataSetNextUpdateDate());
    // Get the product name of the data file which is being used. If a lite
    // file is being used then emit an info event stating automatic updates
    // are not supported.
    if (provider.getDataSetName().indexOf('Premium') !== -1) {
        product = 'Premium';
    }
    else if (provider.getDataSetName().indexOf('Enterprise') !== -1) {
        product = 'Enterprise';
    }
    else if (provider.getDataSetName().indexOf('Trie') !== -1) {
        // Trie data file does not contain the product name, so use
        // the file name instead.
        if (provider.config.fileName.indexOf('Premium') !== -1) {
            product = 'Premium';
        }
        else if (provider.config.fileName.indexOf('Enterprise') !== -1) {
            product = 'Enterprise';
        }
        else {
            FOD.log.emit('info', '[' + provider.Id + '] ' + 'Lite data file does not support automatic' + ' updates. See https://51degrees.com/compare-data-' + 'options for more information.');
        }
    }
    else {
        FOD.log.emit('info', '[' + provider.Id + '] ' + 'Lite data file does not support automatic' + ' updates. See https://51degrees.com/compare-data-' + 'options for more information.');
    }
    // Regularly check if the data file is up to date against the current time.
    var timer = setInterval(function () {
        if (updating) {
            return false;
        }
        if (new Date() > dataSetNextUpdateDate) {
            // Run update function.
            update(provider, function (err) {
                if (err) {
                    // If failed, output log the error and unset the updating flag.
                    FOD.log.emit("info", '[' + provider.Id + '] ' + 'Could not update the data file ' + 'reason: ' + err);
                    updating = false;
                    return false;
                }
                else {
                    // If updated successfully, reload the provider using the new data file,
                    // set the new update date, and unset the updating flag.
                    provider.reloadFromFile();
                    dataSetNextUpdateDate = new Date(provider.getDataSetNextUpdateDate());
                    FOD.log.emit('info', '[' + provider.Id + '] ' + 'Automatically updated data file ' + config.dataFile + ' with version published ' + 'on ' + provider.getDataSetPublishedDate());
                    updating = false;
                }
            });
        }
        else {
            FOD.log.emit('info', '[' + provider.Id + '] ' + 'Could not update the data file reason: ' + 'The data file is current and does not need to be ' + 'updated');
        }
        // Atempt to update every 30 minutes.
    }, 1800000);
    FOD.log.emit('info', '[' + provider.Id + '] ' + 'Auto updater started. Next update date ' + provider.getDataSetNextUpdateDate());
};