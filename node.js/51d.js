var fs = require("fs");
var FODcore = require(__dirname + '/build/Release/FiftyOneDegreesPatternV3');
FODcore.provider = function (configFile) {
    var rawConfig = fs.readFileSync(configFile, "UTF8");
    var config = JSON.parse(rawConfig);
    try {
        console.log("inprovider");
    var returnedProvider = new FODcore.Provider(config.dataFile, config.properties, config.cacheSize, config.poolSize);
        console.log(returnedProvider.getHttpHeaders().get(1));
    }
    catch(e) {
        console.log("error");
        return false;
        }
    var getImportantHeaders = function () {
        var i;
        var importantHeaders = {};
        for (i = 0; i < returnedProvider.getHttpHeaders().size(); i++) {
            var currentHeader = returnedProvider.getHttpHeaders().get(i);
            importantHeaders[returnedProvider.getHttpHeaders().get(i).toLowerCase()] = returnedProvider.getHttpHeaders().get(i);
        }
        return importantHeaders;
    }
    var importantHeaders = getImportantHeaders();
    returnedProvider.config = config;

    returnedProvider.getMatchForHttpHeaders = function (headers) {
        var headersMap = new FODcore.MapStringString();
        Object.keys(headers).forEach(function (key) {

            headersMap.set(key, headers[key]);
        })

        return returnedProvider.getMatch(headersMap);
    }

    if (config.Licence) {
       // require("./update")(returnedProvider);
    }
    
    return returnedProvider;
}

module.exports = FODcore;