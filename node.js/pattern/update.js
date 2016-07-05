var fs = require("fs");
var zlib = require("zlib");
var crypto = require("crypto")
var https = require("https");
var dataSetNextUpdateDate;
var querystring = require("querystring");
var updating;

var update = function (provider, callback) {
    // Load config and make the querystring parameter and http request options.
    var config = provider.config,
        parameters = {
            LicenceKeys: config.Licence,
            Type: config.Type,
            Download: "True",
            Product: provider.getDataSetName()
        };
    if (parameters.Product.indexOf("Premium") !== -1) {
        parameters.Product = "Premium";
    }
    var options = {
        host: "51degrees.com",
        port: 443,
        path: "/products/downloads/premium?" + querystring.stringify(parameters)
    };
    var request = https.get(options);

    // When recieving response, if gzip download file, if not, return error.
    request.on("response", function (response) {
        if (response.headers["transfer-encoding"] && response.headers["transfer-encoding"].indexOf("gzip") === -1) {
            callback("not gzip");
            return false;
        }
        // Set updating flag to true so that another update process does not start.
        updating = true;

        // Stream contents of gzip file into temp file on disk.
        var zippedOutput = fs.createWriteStream(config.dataFile + ".gz");
        response.pipe(zippedOutput);
        response.on("end", function () {
            // When stream completes, read zipped file.
            fs.readFile(config.dataFile + ".gz", function (err, zippedFile) {
                if (err) {
                    callback(err);
                    return false;
                }
                // Check the hash of the zipped file against the md5 from the request.
                var hash = crypto.createHash("md5").update(zippedFile).digest("hex");
                if (hash === response.headers["content-md5"]) {
                    // If hashes match, unzip the file.
                    zlib.unzip(zippedFile, function (err, data) {
                        // Once unzipped, write to file.
                        fs.writeFile(config.dataFile, data, function (err) {
                            if (err) {
                                callback(err);
                                return false;
                            }
                            // If writing to file succedes delete the gzip file.
                            fs.unlink(config.dataFile + ".gz", function (err) {
                                if (err) {
                                    callback(err);
                                } else {
                                    callback();
                                }
                            });
                        });
                    });
                } else {
                    callback("hash failed");
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
                    FOD.log.emit("51error", err);
                    updating = false;
                    return false
                } else {
                    // If uodated successfully, reload the provider using the new data file,
                    // set the new update date, and unset the updating flag.
                    provider.reloadFromFile();
                    dataSetNextUpdateDate = new Date(provider.getDataSetNextUpdateDate());
                    FOD.log.emit("updated", dataSetNextUpdateDate)
                    updating = false;
                }
            });
        }
        //Note, set to 1 second. Change this.
    }, 1000);
}