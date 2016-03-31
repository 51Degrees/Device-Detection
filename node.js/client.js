//var FOD = require("./51d");
//var provider = new FOD.provider("config.json");
var FODcore = require(__dirname + '/build/Release/FiftyOneDegreesPatternV3');
var provider = new FODcore.Provider("../data/51Degrees.dat");
process.on("uncaughtException", function(e) {
    console.trace();
})
var http = require("http");
http.createServer(function(req, res) {
    if (req.url === "/devicecheck") {
        req.match = new provider.getMatch("iji");
        //var match = provider.getMatchForHttpHeaders(req.headers);
        //res.write(match.getUserAgent());
        console.log("matched");
        res.write("hello");
        console.log("destroyed");
        req.match.destroy();
    }
    else {
        res.write("not hello");
    }
    res.end();
}).listen(3000);