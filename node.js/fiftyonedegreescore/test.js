// Import modules.
var FiftyOneDegrees = require("fiftyonedegreescore");
var assert = require("assert");;
// Set up the User-Agent file name and array.
var fs = require("fs"),
    readline = require("readline"),
    inputFileName = "/20000 User Agents.csv",
    userAgents = new Array(1000);
// Find the User Agents file.
if (fs.existsSync(__dirname + "/../../data" + inputFileName)) {
    // The module is in the Device-Detection git repository
    // so use the included file.
    var inputFile = __dirname + "/../../data" + inputFileName;
}
else if (fs.existsSync(process.cwd() + inputFileName)) {
    // The file is in the execution directory.
    var inputFile = process.cwd() + inputFileName;
}
else {
    // The file cannot be found, share a link and exit.
    console.log("No User-Agents file is present. Download from " + "https://github.com/51Degrees/Device-Detection/blob/master" + "/data/20000%20User%20Agents.csv and " + "place it in your execution directory.");
    process.exit();
}
// Read the User Agents file into an array.
var instream = fs.createReadStream(inputFile),
    rl = readline.createInterface(instream, null),
    i = 0;
rl.on('line', function (userAgent) {
    if (i < 1000) {
        userAgents[i] = userAgent;
    }
    i++;
});
// User-Agent string of an iPhone mobile device.
var mobileUserAgent = "Mozilla/5.0 (iPhone; CPU iPhone OS 7_1 like Mac OS X) AppleWebKit/537.51.2 (KHTML, like Gecko) 'Version/7.0 Mobile/11D167 Safari/9537.53";
// User-Agent string of Firefox Web browser version 41 on desktop.
var desktopUserAgent = "Mozilla/5.0 (Windows NT 6.3; WOW64; rv:41.0) Gecko/20100101 Firefox/41.0";
// User-Agent string of a MediaHub device.
var mediaHubUserAgent = "Mozilla/5.0 (Linux; Android 4.4.2; X7 Quad Core Build/KOT49H) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/30.0.0.0 Safari/537.36";
// Set either a pattern or trie config.
if (process.argv[3] === "--pattern") {
    var pattern = true;
    var config = {
        "dataFile": require("fiftyonedegreeslitepattern"),
        "properties": "IsMobile,BrowserName",
        "cacheSize": 10000,
        "poolSize": 4,
        "logLevel": "none",
        "UsageSharingEnabled": false
    };
    console.log("Creating Pattern provider...");
}
else if (process.argv[3] === "--trie") {
    var config = {
        "dataFile": "../data/51Degrees-LiteV3.4.trie",
        "properties": "IsMobile,BrowserName",
        "logLevel": "none",
        "UsageSharingEnabled": false
    };
    console.log("Creating Trie provider...");
}
else {
    console.log(process.argv[3] + " is not a valid argument, use --pattern or --trie.");
    process.exit();
}
// Initialise the provider.
var provider = new FiftyOneDegrees.provider(config);

/* Usage sharing tests have been removed as sharing over HTTPS to localhost
   will throw an error.
describe("usage sharing", function () {
    it("should share valid data", function (done) {
        var http = require('http');
        var zlib = require('zlib');
        var server = http.createServer(function (req, res) {
            var str = '';
            var gz = zlib.createGunzip();
            req.pipe(gz);
            res.write("connected to server");
            gz.on('data', function (chunk) {
                str += chunk;
            });
            gz.on('end', function () {
                assert.equal(true, str.indexOf('<Device>') !== -1);
                done();
            });
            res.end();
        });
        server.listen(1234);
        var usageConfig = JSON.parse(JSON.stringify(config));
        usageConfig.UsageSharingEnabled = true;
        usageConfig.UsageSharingDebug = true;
        var usageProvider = new FiftyOneDegrees.provider(usageConfig);
        var matchServer = http.createServer(function (req, res) {
            usageProvider.getMatch(req);
            res.end();
        });
        matchServer.listen(3000, function () {
            var options = {
                host: 'localhost',
                port: 3000,
                method: 'GET',
                headers: {
                    'user-agent': mobileUserAgent
                }
            };
            http.request(options, function (res) {}).end();
        });
    });
});
*/
// API tests.
describe("API", function () {
    describe("Mobile User-Agent", function () {
        before(function () {
            this.match = provider.getMatch(mobileUserAgent);
        });
        it("Should return a valid match object", function () {
            assert.equal(true, this.match !== undefined);
        });
        it("Should be matched as a mobile device", function () {
            assert.equal("True", this.match.getValue("IsMobile"), "IsMobile property was " + this.match.getValue("IsMobile"));
        });
        after(function () {
            this.match.close();
        });
    });
    describe("Desktop User-Agent", function () {
        before(function () {
            this.match = provider.getMatch(desktopUserAgent);
        });
        it("Should return a valid match object", function () {
            assert.equal(true, this.match !== undefined);
        });
        it("Should be matched as a non-mobile device", function () {
            assert.equal("False", this.match.getValue("IsMobile"), "IsMobile property was " + this.match.getValue("IsMobile"));
        });
        after(function () {
            this.match.close();
        });
    });
    describe("Media Hub User-Agent", function () {
        before(function () {
            this.match = provider.getMatch(mediaHubUserAgent);
        });
        it("Should return a valid match object", function () {
            assert.equal(true, this.match !== undefined);
        });
        it("Should be matched as a non-mobile device", function () {
            assert.equal("False", this.match.getValue("IsMobile"), "IsMobile property was " + this.match.getValue("IsMobile"));
        });
        after(function () {
            this.match.close();
        });
    });
    describe("All Available Properties", function () {
        it("Should return valid properties for all User-Agents", function () {
            userAgents.forEach(function (userAgent) {
                var match = provider.getMatch(userAgent);
                assert.equal(true, match !== undefined, "Match object was undefined for User-Agent:" + userAgent);
                provider.availableProperties.forEach(function (property) {
                    assert.equal(true, match.getValue(property).length > 0, "Value of " + property + " returned was empty");
                });
                match.close();
            });
        });
    });
    describe("Unavailable Property", function () {
        it("Should return no value for all User-Agents", function () {
            userAgents.forEach(function (userAgent) {
                var match = provider.getMatch(userAgent);
                assert.equal(true, match !== undefined, "Match object was undefined for User-Agent:" + userAgent);
                assert.equal(true, match.getValue("notaproperty").length === 0, "Match returned " + match.getValue("notaproperty") + " for nonexistant property");
                match.close();
            });
        });
    });
    if (pattern === true) {
        describe("Match for Device Id", function () {
            it("Should return the same match using getMatch and getMatchForDeviceId", function () {
                userAgents.forEach(function (userAgent) {
                    var match = provider.getMatch(userAgent);
                    assert.equal(true, match !== undefined, "Match object was undefined for User-Agent:" + userAgent);
                    var deviceId = match.getDeviceId();
                    var deviceMatch = provider.getMatchForDeviceId(deviceId);
                    assert.equal(true, deviceMatch !== undefined, "Match object was undefined for device id:" + deviceId);
                    provider.availableProperties.forEach(function (property) {
                        assert.equal(match.getValue(property), deviceMatch.getValue(property), "Match from matched device id did not match the original match for UserAgent " + userAgent);
                    });
                    deviceMatch.close();
                    match.close();
                });
            }).timeout(3000);
        });
        describe("Find Mobile Profiles", function () {
            it("Should find only mobile profiles", function () {
                var profiles = provider.findProfiles("IsMobile", "True");
                for (var i = 0; i < profiles.getCount(); i++) {
                    var match = provider.getMatchForDeviceId(profiles.getProfileId(i).toString());
                    assert.equal(true, match !== undefined, "Match object was undefined for profile id:" + profiles.getProfileId(i));
                    assert.equal("True", match.getValue("IsMobile"), "Profile with profile id " + profiles.getProfileId(i) + " is not a mobile profile");
                    match.close();
                }
            });
        });
        describe("Find Non-Mobile Profiles", function () {
            it("Should find only non-mobile profiles", function () {
                var profiles = provider.findProfiles("IsMobile", "False");
                for (var i = 0; i < profiles.getCount(); i++) {
                    var match = provider.getMatchForDeviceId(profiles.getProfileId(i).toString());
                    assert.equal(true, match !== undefined, "Match object was undefined for profile id:" + profiles.getProfileId(i));
                    assert.equal("False", match.getValue("IsMobile"), "Profile with profile id " + profiles.getProfileId(i) + " is a mobile profile");
                    match.close();
                }
            });
        });
    }
});
// Helper method tests.
describe("Helper Methods", function () {
    describe("Match Redirection", function () {
        var match;
        var headers = {
            "user-agent": mobileUserAgent
        };
        it("Should return User-Agent matches correctly", function () {
            match = provider.getMatch(mobileUserAgent);
            assert.equal(true, match.IsMobile);
            match.close();
        });
        it("Should return HTTP header matches correctly", function () {
            match = provider.getMatch(headers);
            assert.equal(true, match.IsMobile);
            match.close();
        });
        req = {
            "headers": headers
        };
        it("Should return request matches correctly", function () {
            req.on = function (name, func) {
                if (name === "end") {
                    req.end = func;
                }
            };
            provider.getMatch(req);
            assert.equal(true, req.fiftyoneDevice.IsMobile);
            req.fiftyoneDevice.close();
        });
        it("Should close of request matches automatically", function () {
            var closed = false;
            req.fiftyoneDevice.close = function () {
                closed = true;
            };
            req.end();
            assert.equal(true, closed);
        });
    });
    describe("Property Getters", function () {
        it("Should set propery getters to boolean", function () {
            var match = provider.getMatch(mobileUserAgent);
            assert.equal(true, typeof (match.IsMobile) === 'boolean');
        });
    });
    if (pattern === true) {
        describe("Matching From FindProfiles", function () {
            var profiles = provider.findProfiles("IsMobile", "True");
            it("Should return a match object with profiles.getMatch(i)", function () {
                for (var i = 0; i < profiles.count; i++) {
                    var match = profiles.getMatch(i);
                    assert.equal(true, match.IsMobile);
                    match.close();
                }
            });
            it("Should return undefined for an out of range profile", function () {
                var match = profiles.getMatch(profiles.count);
                assert.equal(true, match === undefined);
            });
            it("Should return undefined for a property from a different component", function () {
                var match = profiles.getMatch(0);
                assert.equal(true, match.BrowserName === undefined);
                match.close();
            });
        });
    }
    describe("Available Properties", function () {
        it("Should have an array of valid properties", function () {
            provider.availableProperties.forEach(function (property) {
                assert.equal(true, config.properties.indexOf(property) !== -1);
            });
        });
    });
});
// Performance tests.
describe("Performance", function () {
    // Time used to compare times with reload.
    var baseTime;
    // General matching function. Runs though all 20000 User-Agents
    // and throws an error is a match is not returned.
    var matchAllUserAgents = function () {
        userAgents.forEach(function (userAgent) {
            var match = provider.getMatch(userAgent);
            assert.equal(true, match !== undefined, "Match object was undefined for User-Agent:" + userAgent);
            match.close();
        });
    };
    describe("Startup", function () {
        it("Should startup in < 1.5s", function () {
            var start = new Date();
            var provider = new FiftyOneDegrees.provider(config);
            var end = new Date();
            var timeTaken = end - start;
            assert.equal(true, timeTaken < 150, "Startup time was " + timeTaken);
        });
    });
    describe("Detection Speed", function () {
        it("Should take < 0.1ms per detection", function () {
            var start = new Date();
            matchAllUserAgents();
            var end = new Date();
            var timeTaken = end - start;
            baseTime = timeTaken;
            var timePerDetection = timeTaken / userAgents.length;
            var detectionsPerSecond = 1000 / timePerDetection;
            assert.equal(true, timePerDetection < 0.1, "Time per detection was " + timePerDetection + " ms");
        });
    });
    describe("Reload Penalty", function () {
        it("Should reload without costing more than 1s per 20000 matches", function () {
            var numberOfReloads = 5;
            var start = new Date();
            for (var i = 0; i < numberOfReloads; i++) {
                matchAllUserAgents();
                provider.reloadFromFile();
            }
            var end = new Date();
            var timeTaken = end - start;
            var difference = timeTaken - baseTime;
            var reloadPenalty = difference / numberOfReloads;
            assert.equal(true, reloadPenalty < 1000, "Time penalty for reloading was " + reloadPenalty);
        }).timeout(10000);
    });
});
