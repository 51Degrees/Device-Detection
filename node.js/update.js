var fs = require("fs");
var zlib = require("zlib");
var crypto = require("crypto")
var https = require("https");
var dataSetNextUpdateDate;
var update = function (config) {
//possible log message when updating
    var options = {
        host: "51degrees.com",
        port: 443,
        path: "/products/downloads/premium?LicenseKeys=" + config.Licence + "&Type=BinaryV32&Download=True&Product=Premium",
        headers: {
            "accept-encoding": "gzip,deflate"
        }
    };

    var request = https.get(options);

    request.on("response", function (response) {
        var zippedOutput = fs.createWriteStream(config.dataFile + ".gz");
        response.pipe(zippedOutput);
        response.on("end", function () {
            fs.readFile(config.dataFile + ".gz", function (err, zippedFile) {
                // Possibly use piped hashing?
                var hash = crypto.createHash("md5").update(zippedFile).digest("hex");
                if (hash === response.headers["content-md5"]) {
                    zlib.unzip(zippedFile, function (err, data) {
                        fs.writeFile(config.dataFile, data, function (err) {
                            // insert error handling for disk writing.
                            fs.unlink(config.dataFile + ".gz");
                        });
                    });
                } else {
                    console.error("Failed");
                }
            })
        })
    })
}


module.exports = function (config, Provider) {
//possible duplication
    //check if file exists
    if (fs.statSync(config.dataFile)) {
        dataSetNextUpdateDate = new Date(Provider.getDataSetNextUpdateDate());
    } else {
        update(config)
    }
    var timer = setInterval(function () {
        if (new Date() > dataSetNextUpdateDate) {
            update(config);
            Provider.reloadFromFile();
            dataSetNextUpdateDate = new Date(Provider.getDataSetNextUpdateDate());
        }
    }, 1000);
}