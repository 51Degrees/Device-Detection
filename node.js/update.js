var fs = require("fs");
var zlib = require("zlib");
var crypto = require("crypto")
var https = require("https");
var dataSetNextUpdateDate;
var update = function (provider, callback) {
    var config = provider.config;
    //----------------------------------------------
    //possible log message when updating
    var options = {
        host: "51degrees.com",
        port: 443,
        path: "/products/downloads/premium?LicenseKeys=" + config.Licence + "&Type=BinaryV32&Download=True&Product=Premium",
        headers: {
            "accept-encoding": "gzip"
        }
    };

    var request = https.get(options);

    request.on("response", function (response) {
        var zippedOutput = fs.createWriteStream(config.dataFile + ".gz");
        response.pipe(zippedOutput);
        fs.close(zippedOutput);
        response.on("end", function () {
            fs.readFile(config.dataFile + ".gz", function (err, zippedFile) {
                // Possibly use piped hashing?
                if (err) {
                    console.log(err);
                    return false;
                }
                var hash = crypto.createHash("md5").update(zippedFile).digest("hex");
                if (hash === response.headers["content-md5"]) {
                    zlib.unzip(zippedFile, function (err, data) {
                        fs.writeFile(config.dataFile, data, function (err) {
                            // insert error handling for disk writing.
                            fs.unlink(config.dataFile + ".gz");
                            callback();
                        });
                    });
                } else {
                    callback("error");
                    // create event listener for errors.
                }
            })
        })
    })
}


module.exports = function (provider) {
    var config = provider.config;
    //check if file exists
    dataSetNextUpdateDate = new Date(provider.getDataSetNextUpdateDate());

    var timer = setInterval(function () {
        if (new Date() > dataSetNextUpdateDate) {
            update(provider, function (err) {
                if (err) {
                    return false
                } else {
                    provider.reloadFromFile();
                    dataSetNextUpdateDate = new Date(provider.getDataSetNextUpdateDate());
                }
            });
        }
    }, 1000);
}