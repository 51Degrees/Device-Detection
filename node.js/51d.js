var FOD = require(__dirname + '/build/Release/FiftyOneDegreesPatternV3');
var fs = require("fs");

module.exports = {
    // Make async.
    init: function (configFile) {
        var rawConfig = fs.readFileSync(configFile, "UTF8");
        var config = JSON.parse(rawConfig);
        var returnedObject =  {};
        returnedObject.Provider =  new FOD.Provider(config.dataFile, config.properties, config.cacheSize, config.poolSize);
        /*var getMatchArray = returnedObject.Provider.getMatch;
        returnedObject.Provider.getMatch = function(headers) {
            if (Array.isArray(headers)) {
                console.log(headers);
                return getMatchArray(headers);
            }
            
        }*/
        returnedObject.config = config;
        if (config.Licence) {
            require("./update")(config, returnedObject.Provider);
        }
        return returnedObject;
    }
};