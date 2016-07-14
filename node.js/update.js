var fs = require("fs");
var zlib = require("zlib");
var crypto = require("crypto")
var https = require("https");
var dataSetNextUpdateDate;
var querystring = require("querystring");
var updating;

// Regular expression to check for a valid licence key.
var validLicenceRegEx = new RegExp("^[A-Z\\d]+$");

// Update function called when an update is due.
var update = function (provider, errorHandler) {
    // Load config and make the querystring parameter and http request options.
    var config = provider.config,
        parameters = {
            LicenceKeys: config.Licence,
            Type: config.Type,
            Download: 'True',
        };
    
    // Check the licence key is not empty
    if (config.Licence.length === 0) {
        errorHandler('At least one valid licence key is required ' +
                     'to update device data. See https://51degrees.com/' +
                     'compare-data-options to acquire valid licence keys.');
        return false;
    }
    
    // Check the licence key is a valid format.
    if (validLicenceRegEx.exec(config.Licence) === null) {
        errorHandler('The license key(s) provided were invalid. See ' +
                     'https://51degrees.com/compare-data-options to acquire ' +
                     'valid licence keys.');
        return false;
     }
    
    // Set the request options to get the update from.
    var requestOptions = {
        host: "51degrees.com",
        port: 443,
        path: "/products/downloads/premium?" + querystring.stringify(parameters)
    };
    var request = https.get(requestOptions);

    // When recieving response, if gzip download file, if not, return error.
    request.on("response", function (response) {
        // If the response code is not 200, then throw an error as the
        // download will not happen.
        if (response.statusCode !== 200) {
            switch (response.statusCode) {
                case 429 :
                    errorHandler('Too many attempts have been made to ' +
                             'download a data file from this public IP ' +
                             'address or with this licence key. Try again ' +
                             'after a period of time.');
                    return false;
                case 403 :
                    errorHandler('Data not downloaded. The licence key is not' +
                             'valid');
                    return false;
                default :
                    errorHandler('An error occurred fetching the data file. ' +
                                 'Try again incase the error is temporary, ' +
                                 'or validate licence key and network ' +
                                'configuration.')
                    return false;
            }
        }
        
        // If the response is not gzip encoded then return an error.
        if (response.headers["transfer-encoding"] && response.headers["transfer-encoding"].indexOf("gzip") === -1) {
            errorHandler("The response encoding was " + 
                     response.headers['transfer-encoding']);
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
                                } else {
                                    // The file was delted, so return
                                    // without error.
                                    errorHandler();
                                }
                            });
                        });
                    });
                } else {
                    // The hashes did not match.
                    errorHandler("Data was downloaded but the MD5 hash failed");
                }
            })
        })
    })
}


module.exports = function (provider, FOD) {
    var config = provider.config;
    // Get the next update date of the data file (onnly called once on init).
    dataSetNextUpdateDate = new Date(provider.getDataSetNextUpdateDate());

    // Regularly check if the data file is up to date against the current time.
    // Note: it is possible to expose this to allow pausing and restarting.
    var timer = setInterval(function () {
        if (updating) {
            return false;
        }
        if (new Date() > dataSetNextUpdateDate) {
            // Run update function.
            update(provider, function (err) {
                if (err) {
                    // If failed, output log the error and unset the updating flag.
                    FOD.log.emit("51info", 'Could not update the data file ' +
                                 'reason: ' + err);
                    updating = false;
                    return false
                } else {
                    // If updated successfully, reload the provider using the new data file,
                    // set the new update date, and unset the updating flag.
                    provider.reloadFromFile();
                    dataSetNextUpdateDate = new Date(provider.getDataSetNextUpdateDate());
                    FOD.log.emit('51info', 'Automatically updated data file ' +
                                 config.dataFile + ' with version published ' +
                                 'on ' + provider.getDataSetPublishedDate());
                    updating = false;
                }
            });
        } else {
            FOD.log.emit('51info', 'Could not update the data file reason: ' +
                         'The data file is current and does not need to be ' +
                         'updated');
        }
        //Note, set to 1 second. Change this.
    }, 1000);
    
    FOD.log.emit('51info', 'Auto updater started. Next update date ' +
                 provider.getDataSetNextUpdateDate());
}