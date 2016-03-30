    console.log("----\n");
var FOD = require("./51d").init("config.json");
var provider = FOD.Provider;
var http = require("http");
var testHeaders = "HTTP_HOST localhost\nHTTP_USER_AGENT Mozilla\/5.0 (X11; Ubuntu; Linux x86_64; rv:44.0) Gecko\/20100101 Firefox\/44.0\nHTTP_ACCEPT text\/html,application\/xhtml+xml,application\/xml;q=0.9,*\/*;q=0.8\nHTTP_ACCEPT_LANGUAGE en-GB,en;q=0.5\nHTTP_ACCEPT_ENCODING gzip, deflate\nHTTP_DEVICE_STOCK_UA Android\nHTTP_CONNECTION keep-alive\nHTTP_CACHE_CONTROL max-age=0\n";

var headerConvert = function(headers) {
    console.log(headers);
    var headerString = "";
    Object.keys(headers).forEach(function(key) {
        headerString += key + " " + headers[key] + "\n";
    })
    console.log("----\n");
    console.log(headerString);
    return headerString;
}
http.createServer(function(req, res) {
    if (req.url === "/devicecheck") {

        //res.write(JSON.stringify(req.headers));
        var match = provider.getMatch(testHeaders);
        res.write(match.getUserAgent());
        //res.write(headerConvert(req.headers));
    }
    else {
        res.write("not hello");
    }
    res.end();
}).listen(3000);